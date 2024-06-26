// Copyright © 2014-2016, Prosoft Engineering, Inc. (A.K.A "Prosoft")
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

#ifndef PS_CORE_STRING_COMPONENT_HPP
#define PS_CORE_STRING_COMPONENT_HPP

#include <algorithm>
#include <type_traits>
#include <vector>

#include <prosoft/core/config/config.h>

namespace prosoft {

namespace istring {

template <class String>
inline bool starts_with(const String& str, const String& prefix) {
    return str.compare(0, prefix.length(), prefix) == 0;
}

template <class String>
bool ends_with(const String& str, const String& suffix) {
    if (str.length() >= suffix.length()) {
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
    return false;
}

} // istring

#if PS_HAVE_INLINE_NAMESPACES
inline namespace string {
#endif

template <class String, typename... Args>
inline bool starts_with(const String&) { return false; } // termination handler for variadic

template <class String, typename... Args>
bool starts_with(const String& str, const String& prefix, Args... args) {
    return istring::starts_with(str, prefix) ? true : starts_with<String>(str, args...);
}

template <class String, typename... Args>
inline bool ends_with(const String&) { return false; } // termination handler for variadic

template <class String, typename... Args>
bool ends_with(const String& str, const String& suffix, Args... args) {
    return istring::ends_with(str, suffix) ? true : ends_with<String>(str, args...);
}

template <class String, typename... Args>
inline String prefix(const String&) { return String{}; } // termination handler for variadic

template <class String, typename... Args>
String prefix(const String& str, const String& first, Args... args) {
    return istring::starts_with(str, first) ? String{first} : prefix<String>(str, args...);
}

template <class String, typename... Args>
inline String suffix(const String&) { return String{}; } // termination handler for variadic

template <class String, typename... Args>
String suffix(const String& str, const String& first, Args... args) {
    return istring::ends_with(str, first) ? String{first} : suffix<String>(str, args...);
}

enum class tokenize_options {
    none,
    trim_empty
};

template <class Iterator, class String, class TokenList>
void tokenize(const Iterator& first, const Iterator& last, const String& delimiters, TokenList& tokens, tokenize_options opt = tokenize_options::none) {
    auto i = first;
    auto j = i;
    typename Iterator::difference_type lastDelim = -1;
    auto end = last;
    for (size_t pos = 0; i != end; ++i, ++pos) {
        if (String::npos != delimiters.find(*i)) {
            PSASSERT(j != end, "BUG");
            if (pos > 0) {
                if (1 != (pos - lastDelim)) {
                    PSASSERT(static_cast<typename String::difference_type>(pos) > lastDelim, "BUG");
                    tokens.emplace_back(j, i);
                } else if (tokenize_options::trim_empty != opt) {
                    PSASSERT(String::npos != delimiters.find(*j), "BUG");
                    tokens.emplace_back();
                }
            }
            lastDelim = pos;
            j = i;
            ++j;
        }
    }
    if (i != j) {
        tokens.emplace_back(j, i);
    }
}

template <class String, class TokenList>
inline void tokenize(const String& str, const String& delimiters, TokenList& tokens, tokenize_options opt = tokenize_options::none) {
    return tokenize(str.begin(), str.end(), delimiters, tokens, opt);
}

template <class String, class TokenList = std::vector<String>>
inline TokenList split(const String& str, const String& delimiters, tokenize_options opt = tokenize_options::none) {
    TokenList items;
    tokenize(str, delimiters, items, opt);
    return items;
}

template <typename String, typename Container>
String join(const Container& container, const String& separator) {
    String str;
    typename Container::size_type i = 0;
    for (const auto& item : container) {
        str += item;
        if (i < (container.size() - 1)) {
            str += separator;
        }
        ++i;
    }
    return str;
}

template <typename C>
PS_CONSTEXPR inline bool is_whitespace(C ch) {
    return (ch == static_cast<C>(' ') || ch == static_cast<C>('\n') || ch == static_cast<C>('\r') || ch == static_cast<C>('\t'));
}

template <class String>
String& trim(String& str) {
    while (!str.empty()) {
        auto i = str.begin();
        if (!is_whitespace(*i)) {
            break;
        }
        str.erase(i);
    }
    while (!str.empty()) {
        auto i = --str.end();
        if (!is_whitespace(*i)) {
            break;
        }
        str.erase(i);
    }

    return str;
}

template <class String, typename = typename std::enable_if<!std::is_lvalue_reference<String>::value>::type>
String PS_WARN_UNUSED_RESULT trim(String&& str) {
    String s{std::move(str)};
    trim(s);
    return s;
}

template <class String>
String PS_WARN_UNUSED_RESULT trim(const String& str) {
    String s{str};
    trim(s);
    return s;
}

template <class String>
typename String::size_type replace_all(String& str, const String& findStr, const String& replaceStr, typename String::size_type pos = 0) {
    typename String::size_type count = 0;
    while ((pos = str.find(findStr, pos)) != String::npos) {
        str.replace(pos, findStr.length(), replaceStr);
        pos += replaceStr.length();
        ++count;
    }
    return count;
}

enum class for_each_options {
    none,
    want_empty_lines = 0x1,
};
PS_ENUM_BITMASK_OPS(for_each_options);

template <class Iterator>
using for_each_newline = typename std::iterator_traits<Iterator>::value_type;

template <class Iterator, class Function>
void for_each_line(Iterator first, Iterator last, Function callback, for_each_options opts = for_each_options::none, for_each_newline<Iterator> nl = '\n') {
    auto i = first;
    while (i != last) {
        auto eol = std::find(i, last, nl);
        if (eol > i) {
            callback(i, eol);
            i = eol;
        } else if (is_set(opts & for_each_options::want_empty_lines)) {
            callback(i, i);
        }
        if (eol != last) {
            ++i;
        }
    }
}

template <class String, class Function>
inline void for_each_line(String&& s, Function callback, for_each_options opts = for_each_options::none, for_each_newline<decltype(s.begin())> nl = '\n') {
    for_each_line(s.begin(), s.end(), callback, opts, nl);
}

#if PS_HAVE_INLINE_NAMESPACES
} // string
#endif

} // prosoft

#endif // PS_CORE_STRING_COMPONENT_HPP
