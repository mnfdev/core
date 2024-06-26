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

#include <prosoft/core/config/config_platform.h>

#include <string>

#include <prosoft/core/include/string/unicode_convert.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("string_convert") {
    SECTION("default") {
        using string = std::string;
        using to_string = prosoft::to_string<string, string>;
    
        string test{"test"};
        
        WHEN("default conversion is performed on an lvalue") {
            to_string to_string_converter;
            auto&& result = to_string_converter(test);
            THEN("the result is an lvalue reference to the same string") {
                CHECK(&result == &test);
            }
        }
        
        WHEN("default conversion is performed on an rvalue") {
            to_string to_string_converter;
            auto&& result = to_string_converter(std::move(test));
            THEN("the result is an lvalue reference to a new string") {
                CHECK(&result != &test);
                CHECK_FALSE(result.empty());
            }
            AND_THEN("the original string is empty") {
                CHECK(test.empty());
            }
        }
    }
    
    WHEN("u8string is converted to a std::string") {
        prosoft::u8string u8{"\xC3\xA1\xC3\xA1"};
        CHECK_FALSE(u8.empty());
        auto s = prosoft::to_string<std::string, prosoft::u8string>{}(u8);
        CHECK_FALSE(s.empty());
        CHECK_FALSE(u8.empty());
        
        s.clear();
        CHECK(s.empty());
        
        s = prosoft::to_string<std::string, prosoft::u8string>{}(std::move(u8));
        CHECK_FALSE(s.empty());
        CHECK(u8.empty());
    }
    
    // see case_convert_tests for tolower/toupper
}
