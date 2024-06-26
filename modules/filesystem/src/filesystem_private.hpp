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

#ifndef PS_CORE_FILESYSTEM_PRIVATE_HPP
#define PS_CORE_FILESYSTEM_PRIVATE_HPP

#if _WIN32
#include <prosoft/core/include/unique_resource.hpp>
#endif

namespace fs = prosoft::filesystem::v1;

namespace {
    inline std::error_code einval() {
        return std::error_code{
    #if !_WIN32
            EINVAL,
    #else
            ERROR_INVALID_PARAMETER,
    #endif
            std::system_category()};
    }
} // anon

namespace prosoft {
namespace filesystem {
inline namespace v1 {
namespace ifilesystem {

#if !_WIN32
using native_path_type = u8string;
#else
using native_path_type = u16string;
#endif

using to_native_path = to_string<native_path_type, path::string_type>;

inline void error(int e, error_code& ec) noexcept {
    ec.assign(e, filesystem_category());
}

inline void system_error(error_code& ec) noexcept {
    system::system_error(ec);
}

inline error_code system_error() noexcept {
    return system::system_error();
}

// This takes a raw string because std::string may alloc mem and clear the global error state.
inline filesystem_error system_error(const char* msg) {
    auto ec = system_error(); // Ditto on temp var.
    return filesystem_error{msg, ec};
}

#if !_WIN32
constexpr const char* TMPDIR = "TMPDIR";
#endif

#if _WIN32
owner make_owner(const path&, error_code&) noexcept; // Implemented in ACL module

windows::Handle open(const path& p, DWORD accessMode, DWORD shareMode, DWORD createMode, DWORD flags, error_code&);

inline windows::Handle open(const path& p, DWORD accessMode, error_code& ec) {
    return open(p, accessMode, FILE_SHARE_READ|FILE_SHARE_WRITE, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, ec);
}

bool finfo(const path&, ::BY_HANDLE_FILE_INFORMATION*, error_code&);

struct FAttrs {
    DWORD attrs;
    
    FAttrs() noexcept : attrs(0) {}
    FAttrs(DWORD a) noexcept : attrs(a) {}
    PS_DEFAULT_COPY(FAttrs);
    
    explicit operator bool() const noexcept {
        return INVALID_FILE_ATTRIBUTES != attrs;
    }
    
    operator DWORD() const noexcept {
        return *this ? attrs : 0;
    }
};

inline FAttrs fattrs(const path& p, error_code& ec) {
    auto&& np = ifilesystem::to_native_path{}(p.native());
    const DWORD attrs = ::GetFileAttributesW(np.c_str());
    if (INVALID_FILE_ATTRIBUTES != attrs) {
        ;
    } else {
        system_error(ec);
    }
    return {attrs};
}

inline bool fattrs(const path& p, DWORD flags, error_code& ec) {
    PSASSERT(flags != 0, "BUG");
    return flags == (fattrs(p, ec) & flags);
}

inline bool fattrs(const path& p, DWORD flags) {
    error_code ec;
    return fattrs(p, flags, ec);
}

file_time_type to_filetime(const ::FILETIME&);

std::wstring first_unused_drive_letter(DWORD); // exposed for testing
std::wstring first_unused_drive_letter(error_code&);

#endif //_WIN32

// currently only implemented for macOS (in attrs.cpp)
bool is_mounted_readonly(const path&, error_code&);

} // ifilesystem

} // v1
} // filesystem
} // prosoft

#endif // PS_CORE_FILESYSTEM_PRIVATE_HPP
