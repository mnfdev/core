// Copyright © 2015-2016, Prosoft Engineering, Inc. (A.K.A "Prosoft")
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Prosoft nor the names of its contributors may be
//       used to endorse or promote products derived from this software without
//       specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PROSOFT ENGINEERING, INC. BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef PS_CORE_SYSTEM_UTILS_HPP
#define PS_CORE_SYSTEM_UTILS_HPP

#if _WIN32
#include <prosoft/core/config/config_windows.h>
#include <windows.h>
#endif

#include <prosoft/core/include/unique_resource.hpp>

namespace prosoft {
namespace system {

#if _WIN32
template <typename Data>
unique_malloc<Data> token_info(TOKEN_INFORMATION_CLASS tclass, HANDLE token, DWORD& err) {
    DWORD size = 0;
    ::GetTokenInformation(token, tclass, nullptr, size, &size);
    if ((err = ::GetLastError()) == ERROR_INSUFFICIENT_BUFFER && size > 0) {
        auto tinfo = make_malloc_throw<Data>(size);
        if (::GetTokenInformation(token, tclass, tinfo.get(), size, &size)) {
            err = 0;
            return std::move(tinfo);
        } else {
            err = ::GetLastError();
        }
    }
    return {};
}

#endif // WIN32
} // system
} // prosoft

#endif // PS_CORE_SYSTEM_UTILS_HPP
