// Copyright © 2016-2018, Prosoft Engineering, Inc. (A.K.A "Prosoft")
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

#if !_WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <process.h> // _getpid()
#endif

#include <fstream>
#include <string>

namespace {
    prosoft::filesystem::path process_name() {
        const int mypid =
        #if !_WIN32
            ::getpid();
        #else
            ::_getpid();
        #endif
        using namespace prosoft::filesystem;
        return path{std::to_string(mypid)};
    }
    
    inline
    prosoft::filesystem::path process_name(const char* s) {
        return  process_name().concat("_").concat(s);
    }

    template<typename T>
    const T& create_file(const T& p, std::ios::openmode mode = std::ios::binary) {
        std::ofstream stream(p.c_str(), mode);
        REQUIRE(stream);
        return p;
    }
    
    // Microsoft's compiler offers std::ofstream with wide character support as an extension, but Mingw posix does not.
    // Since Mingw is currently only for compiling, always fail. Could use CreateFile() instead for both compilers.
#ifdef __MINGW32__
    template<>
    const filesystem::basic_path<prosoft::u16string>& create_file(const filesystem::basic_path<prosoft::u16string>& p, std::ios::openmode) {
        REQUIRE(false); // always fail
        return p;
    }
#endif
    
    class RAII_remove { // cleanup for test cases
        using path_type = prosoft::filesystem::path;
        const path_type& m_p;
    public:
        RAII_remove(const path_type& p) : m_p(p) {}
        RAII_remove(path_type&&) = delete;
        RAII_remove(const RAII_remove&) = delete;
        RAII_remove(RAII_remove&&) = delete;
        ~RAII_remove() {
            prosoft::filesystem::error_code ec;
            prosoft::filesystem::remove(m_p, ec);
            CHECK(!ec); // REQUIRE throws an exception on failure
        }
    };
    #define PS_RAII_REMOVE___(p,R,C) RAII_remove  R##C(p)
    #define PS_RAII_REMOVE__(p,R,C) PS_RAII_REMOVE___(p,R,C)
    #define PS_RAII_REMOVE(p) PS_RAII_REMOVE__(p, r, __COUNTER__)
}
