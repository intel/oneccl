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

#ifndef CCL_PRODUCT_FULL
#error "Do not include this file directly. Please include 'ccl.hpp'"
#endif

#if defined(MULTI_GPU_SUPPORT) || defined(CCL_ENABLE_SYCL)

namespace ccl {
namespace details {

/**
 * Traits for stream attributes specializations
 */
template <>
struct ccl_api_type_attr_traits<stream_attr_id, stream_attr_id::version> {
    using type = ccl::library_version;
    using return_type = type;
};

template <>
struct ccl_api_type_attr_traits<stream_attr_id, stream_attr_id::native_handle> {
    using type = typename unified_stream_type::ccl_native_t;
    using handle_t = typename unified_stream_type::handle_t;
    using return_type = type;
};

template <>
struct ccl_api_type_attr_traits<stream_attr_id, stream_attr_id::device> {
    using type = typename unified_device_type::ccl_native_t;
    using handle_t = typename unified_device_type::handle_t;
    using return_type = type;
};

template <>
struct ccl_api_type_attr_traits<stream_attr_id, stream_attr_id::context> {
    using type = typename unified_device_context_type::ccl_native_t;
    using handle_t = typename unified_device_context_type::handle_t;
    using return_type = type;
};

template <>
struct ccl_api_type_attr_traits<stream_attr_id, stream_attr_id::ordinal> {
    using type = uint32_t;
    using return_type = type;
};

template <>
struct ccl_api_type_attr_traits<stream_attr_id, stream_attr_id::index> {
    using type = uint32_t;
    using return_type = type;
};

template <>
struct ccl_api_type_attr_traits<stream_attr_id, stream_attr_id::flags> {
    using type = size_t;
    using return_type = type;
};

template <>
struct ccl_api_type_attr_traits<stream_attr_id, stream_attr_id::mode> {
    using type = size_t;
    using return_type = type;
};

template <>
struct ccl_api_type_attr_traits<stream_attr_id, stream_attr_id::priority> {
    using type = size_t;
    using return_type = type;
};

} // namespace details
} // namespace ccl
#endif // defined(MULTI_GPU_SUPPORT) || defined(CCL_ENABLE_SYCL)
