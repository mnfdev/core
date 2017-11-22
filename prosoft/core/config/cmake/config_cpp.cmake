# Copyright © 2013-2017, Prosoft Engineering, Inc. (A.K.A "Prosoft")
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Prosoft nor the names of its contributors may be
#       used to endorse or promote products derived from this software without
#       specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL PROSOFT ENGINEERING, INC. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

if(NOT PSCONFIG)
	message(FATAL_ERROR, "PSCONFIG is missing!")
endif()

macro(ps_core_config_cpp_version TARGET_NAME)
	include(CheckCXXSourceCompiles)
	set_property(TARGET ${TARGET_NAME} PROPERTY CXX_EXTENSIONS OFF) # Don't use GNU extensions. They are not portable.
	if(NOT MSVC)
		include(CheckCXXCompilerFlag)
		check_cxx_compiler_flag("-std=c++11" HAVE_STD_CPP11_FLAG)
		if(HAVE_STD_CPP11_FLAG)
			# Cmake 3.8 adds C++17 support. CMake < 3.10 ignores this property for MSVC, so C++17 support in VS2017 would require manual flags.
			set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 14)
		else()
			message(FATAL_ERROR "Your compiler doesn't support C++11.")
		endif()
		check_cxx_compiler_flag("-stdlib=libc++" HAVE_STDLIB_LIBCPP_FLAG)
		if(HAVE_STDLIB_LIBCPP_FLAG)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
			target_link_libraries(${TARGET_NAME} PUBLIC "-stdlib=libc++")
		endif()
		# Turn on _GLIBCXX_USE_CXX11_ABI to use GCC 5.1 C++11 ABI
		if(CMAKE_COMPILER_IS_GNUCXX)
			target_compile_definitions(${TARGET_NAME} PRIVATE _GLIBCXX_USE_CXX11_ABI=1)
		endif()
	else()
		set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 14) # ignored until VS2017
	endif()
endmacro()
