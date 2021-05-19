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
#pragma once

#include "common/comm/l0/communicator/typed_base_communicator.hpp"
#include "common/comm/usm_visitor/usm_visitors.hpp"

namespace native {
struct thread_group_context;
}

class thread_device_group_ring_communicator
        : public typed_base_communicator<thread_device_group_ring_communicator,
                                         ccl::group_split_type::process,
                                         ccl::device_topology_type::ring,
                                         ccl::gpu_communicator_traits> {
public:
    using base_t = typed_base_communicator<thread_device_group_ring_communicator,
                                           ccl::group_split_type::process,
                                           ccl::device_topology_type::ring,
                                           ccl::gpu_communicator_traits>;

    using communication_devices_t = native::device_variant_t<native::ccl_gpu_comm,
                                                             native::ccl_virtual_gpu_comm
                                                             /*, TODO disabled t now,
                                                             native::ccl_numa_proxy<native::ccl_gpu_comm>,
                                                             native::ccl_numa_proxy<native::ccl_virtual_gpu_comm>*/>;

    thread_device_group_ring_communicator(ccl::unified_device_type&& device,
                                          ccl::unified_context_type&& ctx,
                                          size_t thread_idx,
                                          size_t process_idx,
                                          const ccl::comm_split_attr& attr);

    void visit(ccl::gpu_comm_attr& comm_attr) override;

    ccl::event barrier(const ccl::stream::impl_value_t& stream,
                       const ccl::barrier_attr& attr,
                       const ccl::vector_class<ccl::event>& deps) override;

    COMM_IMPL_DECLARATION
    COMM_IMPL_CLASS_DECLARATION
    COMM_IMPL_SPARSE_DECLARATION
    COMM_IMPL_SPARSE_CLASS_DECLARATION

    communication_devices_t& get_communication_device() {
        return communication_device;
    }

private:
    std::shared_ptr<native::thread_group_context> ctx;
    communication_devices_t communication_device;
};
