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

#include "sched/entry/entry.hpp"

class prologue_entry : public sched_entry {
public:
    static constexpr const char* class_name() noexcept {
        return "PROLOGUE";
    }

    prologue_entry() = delete;
    prologue_entry(ccl_sched* sched,
                   ccl::prologue_fn fn,
                   const ccl_buffer in_buf,
                   size_t in_cnt,
                   const ccl_datatype& in_dtype,
                   void** out_buf,
                   size_t* out_cnt,
                   ccl::datatype* out_dtype_idx)
            : sched_entry(sched),
              fn(fn),
              in_buf(in_buf),
              in_cnt(in_cnt),
              in_dtype(in_dtype),
              out_buf(out_buf),
              out_cnt(out_cnt),
              out_dtype_idx(out_dtype_idx) {}

    void start() override {
        size_t in_bytes = in_cnt * in_dtype.size();
        size_t offset = in_buf.get_offset();
        const ccl::fn_context context = { sched->coll_attr.match_id.c_str(), offset };
        fn(in_buf.get_ptr(in_bytes),
           in_cnt,
           in_dtype.idx(),
           out_buf,
           out_cnt,
           out_dtype_idx,
           &context);
        status = ccl_sched_entry_status_complete;
    }

    const char* name() const override {
        return class_name();
    }

protected:
    void dump_detail(std::stringstream& str) const override {
        ccl_logger::format(str,
                           "in_dt ",
                           ccl::global_data::get().dtypes->name(in_dtype),
                           ", in_cnt ",
                           in_cnt,
                           ", in_buf ",
                           in_buf,
                           ", out_dt ",
                           ccl::global_data::get().dtypes->name(*out_dtype_idx),
                           ", out_dtype_size ",
                           ccl::global_data::get().dtypes->get(*out_dtype_idx).size(),
                           ", out_cnt ",
                           out_cnt,
                           ", out_buf ",
                           out_buf,
                           ", fn ",
                           fn,
                           "\n");
    }

private:
    ccl::prologue_fn fn;
    ccl_buffer in_buf;
    size_t in_cnt;
    ccl_datatype in_dtype;
    void** out_buf;
    size_t* out_cnt;
    ccl::datatype* out_dtype_idx;
};
