/*
 Copyright 2016-2020 Intel Corporation
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
     http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/
#include "base.hpp"
#include "mpi.h"

void run_collective(const char* cmd_name,
                    const std::vector<float>& send_buf,
                    std::vector<float>& recv_buf,
                    ccl::communicator& comm,
                    ccl::reduce_attr& coll_attr) {
    std::chrono::system_clock::duration exec_time{ 0 };
    float expected = (comm.size() - 1) * (static_cast<float>(comm.size()) / 2);
    float received;

    comm.barrier();

    for (size_t idx = 0; idx < ITERS; ++idx) {
        auto start = std::chrono::system_clock::now();
        auto req = comm.reduce(send_buf.data(),
                               recv_buf.data(),
                               recv_buf.size(),
                               ccl::reduction::sum,
                               COLL_ROOT,
                               coll_attr);
        req->wait();
        exec_time += std::chrono::system_clock::now() - start;
    }

    for (size_t idx = 0; idx < recv_buf.size(); idx++) {
        received = recv_buf[idx];
        if ((comm.rank() == COLL_ROOT) && (received != expected)) {
            fprintf(stderr, "idx %zu, expected %4.4f, got %4.4f\n", idx, expected, received);

            std::cout << "FAILED" << std::endl;
            std::terminate();
        }
    }

    comm.barrier();

    std::cout << "avg time of " << cmd_name << ": "
              << std::chrono::duration_cast<std::chrono::microseconds>(exec_time).count() / ITERS
              << ", us" << std::endl;
}

int main() {
    MPI_Init(NULL, NULL);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    auto& env = ccl::environment::instance();
    (void)env;

    /* create CCL internal KVS */
    ccl::shared_ptr_class<ccl::kvs> kvs;
    ccl::kvs::address_type main_addr;
    if (rank == 0) {
        kvs = env.create_main_kvs();
        main_addr = kvs->get_address();
        MPI_Bcast((void*)main_addr.data(), main_addr.size(), MPI_BYTE, 0, MPI_COMM_WORLD);
    }
    else {
        MPI_Bcast((void*)main_addr.data(), main_addr.size(), MPI_BYTE, 0, MPI_COMM_WORLD);
        kvs = env.create_kvs(main_addr);
    }

    auto comm = env.create_communicator(size, rank, kvs);
    auto coll_attr = ccl::environment::instance().create_operation_attr<ccl::reduce_attr>();

    MSG_LOOP(comm, std::vector<float> send_buf(msg_count, static_cast<float>(comm.rank()));
             std::vector<float> recv_buf(msg_count);
             coll_attr.set<ccl::operation_attr_id::to_cache>(0);
             run_collective("warmup_reduce", send_buf, recv_buf, comm, coll_attr);
             coll_attr.set<ccl::operation_attr_id::to_cache>(1);
             run_collective("persistent_reduce", send_buf, recv_buf, comm, coll_attr);
             coll_attr.set<ccl::operation_attr_id::to_cache>(0);
             run_collective("regular_reduce", send_buf, recv_buf, comm, coll_attr););

    MPI_Finalize();
    return 0;
}
