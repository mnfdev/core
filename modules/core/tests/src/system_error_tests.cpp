// Copyright © 2015-2024, Prosoft Engineering, Inc. (A.K.A "Prosoft")
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

#include <prosoft/core/include/system_error.hpp>

#include <catch2/catch_test_macros.hpp>

void set_posix_error(int e) {
    errno = e;
}

void set_error(int e) {
#if !_WIN32
    set_posix_error(e);
#else
    ::SetLastError(e);
#endif
}

TEST_CASE("system_error") {
    set_posix_error(EINVAL);
    const auto pe = prosoft::system::posix_error();
    CHECK(pe.value() == EINVAL);

    auto msg = pe.message();
    CHECK_FALSE(msg.empty());
    
    CHECK_NOTHROW(prosoft::system::posix_category());

#if !_WIN32
    CHECK(prosoft::system::system_error().value() == EINVAL);
#else
    set_error(ERROR_INVALID_HANDLE);
    const auto se = prosoft::system::system_error();
    CHECK(se.value() == ERROR_INVALID_HANDLE);
    msg = se.message();
    CHECK_FALSE(msg.empty());
#endif
}
