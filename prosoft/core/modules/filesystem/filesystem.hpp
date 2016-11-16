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

// C++17 Filesystem implementation: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4100.pdf

#ifndef PS_CORE_FILESYSTEM_HPP
#define PS_CORE_FILESYSTEM_HPP

#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <system_error>

// clang-format off
namespace prosoft { namespace filesystem { inline namespace v1 {
using error_code = std::error_code; // Extension
}}}
// clang-format on

#include <prosoft/core/include/system_error.hpp>

#include "filesystem_path.hpp"
#include "filesystem_iterator.hpp"
#include "filesystem_acl.hpp"

namespace prosoft {
namespace filesystem {
inline namespace v1 {

using file_time_type = std::chrono::time_point<std::chrono::system_clock>;

inline void swap(path& lhs, path& rhs) noexcept(noexcept(std::declval<path>().swap(std::declval<path&>()))) {
    lhs.swap(rhs);
}

inline size_t hash_value(const path& p) {
    using hash = std::hash<typename path::string_type>;
    return hash{}(p);
}

inline bool operator<(const path& lhs, const path& rhs) {
    return lhs.compare(rhs) < 0;
}

inline bool operator<=(const path& lhs, const path& rhs) {
    return lhs.compare(rhs) <= 0;
}

inline bool operator>(const path& lhs, const path& rhs) {
    return lhs.compare(rhs) > 0;
}

inline bool operator>=(const path& lhs, const path& rhs) {
    return lhs.compare(rhs) >= 0;
}

inline bool operator==(const path& lhs, const path& rhs) {
    return lhs.compare(rhs) == 0;
}

inline bool operator!=(const path& lhs, const path& rhs) {
    return !(lhs == rhs);
}

inline path operator/(const path& lhs, const path& rhs) {
    return path{lhs} /= rhs;
}

template <class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const path& p) {
    os <<
#if PS_CPP14_STDLIB
        std::quoted(
#endif
            ifilesystem::to_string_type<std::basic_string<CharT, Traits>, path::string_type>{}(p.native())
#if PS_CPP14_STDLIB
                )
#endif
        ;
    return os;
}

template <class CharT, class Traits>
std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, path& p) {
    std::basic_string<CharT, Traits> s;
    is >>
#if PS_CPP14_STDLIB
        std::quoted(
#endif
            s
#if PS_CPP14_STDLIB
            )
#endif
        ;
    p = s;
    return is;
}

// Explicit conversion from raw UTF8 input.
template <class Source>
inline path u8path(const Source& s) {
    return path{u8string{s}};
}

template <class InputIterator>
inline path u8path(InputIterator first, InputIterator last) {
    return path{u8string{std::string{first, last}}};
}

inline const std::error_category& filesystem_category() noexcept { // Extension
    return system::error_category();
}

class filesystem_error : public std::system_error {
public:
    filesystem_error(const std::string& msg, error_code ec)
        : std::system_error(ec, msg) {}
    filesystem_error(const std::string& msg, path p, error_code ec)
        : std::system_error(ec, msg)
        , mPath1(p) {}
    filesystem_error(const std::string& msg, path p, path p2, error_code ec)
        : std::system_error(ec, msg)
        , mPath1(p)
        , mPath2(p2) {}
    virtual PS_DEFAULT_DESTRUCTOR(filesystem_error);

    PS_DEFAULT_COPY(filesystem_error);
    PS_DEFAULT_MOVE(filesystem_error);

    const path& path1() noexcept {
        return mPath1;
    }

    const path& path2() noexcept {
        return mPath2;
    }

private:
    path mPath1;
    path mPath2;
};

enum class file_type {
    none = 0,
    not_found = -1,
    regular = 1,
    directory = 2,
    symlink = 3,
    block = 4,
    character = 5,
    fifo = 6,
    socket = 7,
    unknown = 8
};

enum class perms {
    none = 0,
    owner_read = 0400,
    owner_write = 0200,
    owner_exec = 0100,
    owner_all = 0700,

    group_read = 040,
    group_write = 020,
    group_exec = 010,
    group_all = 070,

    others_read = 04,
    others_write = 02,
    others_exec = 01,
    others_all = 07,

    all = 0777,
    set_uid = 04000,
    set_gid = 02000,
    sticky_bit = 01000,

    mask = 07777,

    unknown = 0xffff,

    add_perms = 0x10000,
    remove_perms = 0x20000,
    resolve_symlinks = 0x40000,
};

PS_ENUM_BITMASK_OPS(perms);

// Extensions

class owner {
    using owner_type = std::pair<access_control_identity, access_control_identity>;
    owner_type m_owner;

public:
    template <class Identity>
    owner(Identity&& user, Identity&& group)
        : m_owner(std::forward<Identity>(user), std::forward<Identity>(group)) {}

    explicit owner()
        : owner(invalid_owner()) {}

    PS_DEFAULT_DESTRUCTOR(owner);
    PS_DEFAULT_COPY(owner);
    PS_DEFAULT_MOVE(owner);

    void swap(owner& other) noexcept {
        using std::swap;
        swap(m_owner, other.m_owner);
    }

    bool operator==(const owner& other) const {
        return m_owner == other.m_owner;
    }

    const access_control_identity& user() const noexcept {
        return m_owner.first;
    }

    const access_control_identity& group() const noexcept {
        return m_owner.second;
    }

    template <class Identity>
    void user(Identity&& user) {
        m_owner.first = std::forward<Identity>(user);
    }

    template <class Identity>
    void group(Identity&& group) {
        m_owner.second = std::forward<Identity>(group);
    }

    static owner process_owner() {
        return owner{access_control_identity::process_user(), access_control_identity::invalid_group()};
    }

    static owner invalid_owner() {
        return owner{access_control_identity::invalid_user(), access_control_identity::invalid_group()};
    }
};

inline void swap(owner& lhs, owner& rhs) {
    lhs.swap(rhs);
}

inline bool operator!=(const owner& lhs, const owner& rhs) {
    return !lhs.operator==(rhs);
}

class times {
    file_time_type m_modifyTime;
    file_time_type m_changeTime;
    file_time_type m_accessTime;
    file_time_type m_createTime;
    
    static constexpr file_time_type m_invalidTime = file_time_type::min();
    
public:
    times()
        : m_modifyTime(m_invalidTime)
        , m_changeTime(m_invalidTime)
        , m_accessTime(m_invalidTime)
        , m_createTime(m_invalidTime) {}
    PS_DEFAULT_COPY(times);
    PS_DEFAULT_MOVE(times);
    
    const file_time_type& modified() const noexcept {
        return m_modifyTime;
    }
    
    void modified(const file_time_type& val) {
        m_modifyTime = val;
    }
    
    bool has_modified() const noexcept {
        return m_modifyTime != m_invalidTime;
    }
    
    const file_time_type& metadata_modified() const noexcept {
        return m_changeTime;
    }
    
    void metadata_modified(const file_time_type& val) {
        m_changeTime = val;
    }
    
    bool has_metadata_modified() const noexcept {
        return m_changeTime != m_invalidTime;
    }
    
    const file_time_type& accessed() const noexcept {
        return m_accessTime;
    }
    
    void accessed(const file_time_type& val) {
        m_accessTime = val;
    }
    
    bool has_accessed() const noexcept {
        return m_accessTime != m_invalidTime;
    }
    
    const file_time_type& created() const noexcept {
        return m_createTime;
    }
    
    void created(const file_time_type& val) {
        m_createTime = val;
    }
    
    bool has_created() const noexcept {
        return m_createTime != m_invalidTime;
    }

    static constexpr file_time_type make_invalid() {
        return m_invalidTime;
    }
};

// Extensions

class file_status {
    using owner_type = filesystem::owner;
    using times_type = filesystem::times;
    
public:
    file_status(file_type ft, perms p, owner_type&& o, const times_type& t)
        : m_owner(std::move(o))
        , m_times(t)
        , m_type(ft)
        , m_perms(p) {}
    file_status(file_type ft, perms p, const owner_type& o, const times_type& t)
        : file_status(ft, p, owner_type{o}, t) {}
    explicit file_status(file_type ft = file_type::none, perms p = perms::unknown)
        : file_status(ft, p, owner_type::invalid_owner(), times_type()) {}
    PS_DEFAULT_DESTRUCTOR(file_status);
    PS_DEFAULT_COPY(file_status);
    PS_DEFAULT_MOVE(file_status);

    file_type type() const noexcept {
        return m_type;
    }

    void type(file_type ft) noexcept {
        m_type = ft;
    }

    perms permissions() const noexcept {
        return m_perms;
    }

    void permissions(perms p) noexcept {
        m_perms = p;
    }

    // Extensions

    const owner_type& owner() const noexcept {
        return m_owner;
    }

    void owner(const owner_type& o) {
        m_owner = o;
    }

    void owner(owner_type&& o) {
        m_owner = std::move(o);
    }
    
    const times_type& times() const noexcept {
        return m_times;
    }
    
    void times(const times_type& val) {
        m_times = val;
    }

private:
    owner_type m_owner;
    times_type m_times;
    file_type m_type;
    perms m_perms;
};

// operations
path current_path();
path current_path(error_code&);
void current_path(const path&);
void current_path(const path&, error_code&);

// As an extension, canonical will expand an initial '~' char to home_directory_path().
path canonical(const path&, const path& base = current_path());
path canonical(const path&, const path& base, error_code&);
inline path canonical(const path& p, error_code& ec) {
    return canonical(p, current_path(ec), ec);
}

// perms are an extension and ignored on Windows. perms:all is the std. default.
bool create_directories(const path&, perms perm = perms::all);
bool create_directories(const path&, perms, error_code&) noexcept;
inline bool create_directories(const path& p, error_code& ec) noexcept {
    return create_directories(p, perms::all, ec);
}

bool create_directory(const path&, perms perm = perms::all);
bool create_directory(const path&, perms, error_code&) noexcept;
inline bool create_directory(const path& p, error_code& ec) noexcept {
    return create_directory(p, perms::all, ec);
}

bool create_directory(const path&, const path& cloneFrom);
bool create_directory(const path&, const path& cloneFrom, error_code&) noexcept;

bool equivalent(const path&, const path&);
bool equivalent(const path&, const path&, error_code&) noexcept;

bool remove(const path&);
bool remove(const path&, error_code&) noexcept;

file_status status(const path&);
file_status status(const path&, error_code&) noexcept;

file_status symlink_status(const path&);
file_status symlink_status(const path&, error_code&) noexcept;

inline bool status_known(file_status s) noexcept {
    return s.type() != file_type::none;
}

inline bool exists(file_status s) noexcept {
    return status_known(s) && s.type() != file_type::not_found;
}
inline bool exists(const path& p) {
    return exists(status(p));
}
inline bool exists(const path& p, error_code& ec) noexcept {
    return exists(status(p, ec));
}

inline file_time_type last_write_time(const path& p) {
    return status(p).times().modified();
}

inline file_time_type last_write_time(const path& p, error_code& ec) {
    return status(p, ec).times().modified();
}

inline bool is_block_file(file_status s) noexcept {
    return s.type() == file_type::block;
}
inline bool is_block_file(const path& p) {
    return is_block_file(status(p));
}
inline bool is_block_file(const path& p, error_code& ec) noexcept {
    return is_block_file(status(p, ec));
}

inline bool is_character_file(file_status s) noexcept {
    return s.type() == file_type::character;
}
inline bool is_character_file(const path& p) {
    return is_character_file(status(p));
}
inline bool is_character_file(const path& p, error_code& ec) noexcept {
    return is_character_file(status(p, ec));
}

inline bool is_directory(file_status s) noexcept {
    return s.type() == file_type::directory;
}
inline bool is_directory(const path& p) {
    return is_directory(status(p));
}
inline bool is_directory(const path& p, error_code& ec) noexcept {
    return is_directory(status(p, ec));
}

inline bool is_fifo(file_status s) noexcept {
    return s.type() == file_type::fifo;
}
inline bool is_fifo(const path& p) {
    return is_fifo(status(p));
}
inline bool is_fifo(const path& p, error_code& ec) noexcept {
    return is_fifo(status(p, ec));
}

inline bool is_regular_file(file_status s) noexcept {
    return s.type() == file_type::regular;
}
inline bool is_regular_file(const path& p) {
    return is_regular_file(status(p));
}
inline bool is_regular_file(const path& p, error_code& ec) noexcept {
    return is_regular_file(status(p, ec));
}

inline bool is_socket(file_status s) noexcept {
    return s.type() == file_type::socket;
}
inline bool is_socket(const path& p) {
    return is_socket(status(p));
}
inline bool is_socket(const path& p, error_code& ec) noexcept {
    return is_socket(status(p, ec));
}

inline bool is_symlink(file_status s) noexcept {
    return s.type() == file_type::symlink;
}
inline bool is_symlink(const path& p) {
    return is_symlink(status(p));
}
inline bool is_symlink(const path& p, error_code& ec) noexcept {
    return is_symlink(status(p, ec));
}

inline bool is_other(file_status s) noexcept {
    return exists(s) && !is_regular_file(s) && !is_directory(s) && !is_symlink(s);
}
inline bool is_other(const path& p) {
    return is_other(status(p));
}
inline bool is_other(const path& p, error_code& ec) noexcept {
    return is_other(status(p, ec));
}

path temp_directory_path();
path temp_directory_path(error_code&);

// dir entry ops
inline file_status directory_entry::status() const {
    return prosoft::filesystem::status(get_path());
}
inline file_status directory_entry::status(error_code& ec) const noexcept {
    return prosoft::filesystem::status(get_path(), ec);
}
inline file_status directory_entry::symlink_status() const {
    return prosoft::filesystem::symlink_status(get_path());
}
inline file_status directory_entry::symlink_status(error_code& ec) const noexcept {
    return prosoft::filesystem::symlink_status(get_path(), ec);
}

inline bool operator==(const directory_entry& lhs, const directory_entry& rhs) {
    return rhs.path() == lhs.path();
}
inline bool operator!=(const directory_entry& lhs, const directory_entry& rhs) {
    return rhs.path() != lhs.path();
}
inline bool operator<(const directory_entry& lhs, const directory_entry& rhs) {
    return rhs.path() < lhs.path();
}
inline bool operator<=(const directory_entry& lhs, const directory_entry& rhs) {
    return rhs.path() <= lhs.path();
}
inline bool operator>(const directory_entry& lhs, const directory_entry& rhs) {
    return rhs.path() > lhs.path();
}
inline bool operator>=(const directory_entry& lhs, const directory_entry& rhs) {
    return rhs.path() >= lhs.path();
}

// iterators
template <class Traits>
basic_iterator<Traits>& basic_iterator<Traits>::operator++() {
    error_code ec;
    increment(ec);
    PS_THROW_IF(ec.value() != 0, filesystem_error("Could not increment iterator", root_or_empty(), ec));
    return *this;
}

template <class Traits>
basic_iterator<Traits>::basic_iterator(const path& p, directory_options opts) {
    error_code ec;
    m_i = ifilesystem::make_iterator_state(p, ifilesystem::make_options<Traits>(opts), ec, Traits{});
    PS_THROW_IF(ec.value() != 0, filesystem_error("Could not create iterator", p, ec));
}

using directory_iterator = basic_iterator<ifilesystem::iterator_traits>;
using recursive_directory_iterator = basic_iterator<ifilesystem::recursive_iterator_traits>;

// Extensions

// FreeBSD dropped block devices a while ago, Windows never supported block devices and Linux does not have disk char devices.
// Therefore in most cases all you should care about is if the file is a device file and not what type of device.

inline bool is_device_file(file_status s) noexcept {
    return is_character_file(s) || is_block_file(s);
}
inline bool is_device_file(const path& p) {
    return is_device_file(status(p));
}
inline bool is_device_file(const path& p, error_code& ec) noexcept {
    return is_device_file(status(p, ec));
}

bool is_mountpoint(const path&);
bool is_mountpoint(const path&, error_code&);

namespace ifilesystem {
path home_directory_path(const access_control_identity&, error_code&); // private as Windows only supports the current user
} // ifilesystem

path home_directory_path(); // XXX: UNIX daemon users may not have a home dir.
inline path home_directory_path(error_code& ec) {
    return ifilesystem::home_directory_path(access_control_identity::process_user(), ec);
}

// The filesystem mount point for the given path. This may differ from path.root_path().
path mount_path(const path&);
path mount_path(const path&, error_code&);

enum class domain {
    user, // XXX: UNIX daemon users may not have a home dir for relative sub-paths.
    user_local, // Windows only -- local to the machine, mapped to 'user' for other systems
    shared,
};

enum class standard_directory {
    app_data,
    cache,
};

enum class standard_directory_options {
    none = 0x0,
    create = 0x1
};
PS_ENUM_BITMASK_OPS(standard_directory_options);

path standard_directory_path(domain, standard_directory, standard_directory_options);
path standard_directory_path(domain, standard_directory, standard_directory_options, error_code&);
inline path standard_directory_path(standard_directory sd) {
    return standard_directory_path(domain::user, sd, standard_directory_options::create);
}
inline path standard_directory_path(standard_directory sd, error_code& ec) {
    return standard_directory_path(domain::user, sd, standard_directory_options::create, ec);
}

// For the cache we want to default to user_local.
inline path cache_directory_path() {
    return standard_directory_path(domain::user_local, standard_directory::cache, standard_directory_options::create);
}
inline path cache_directory_path(error_code& ec) {
    return standard_directory_path(domain::user_local, standard_directory::cache, standard_directory_options::create, ec);
}
// Extensions

} // v1
} // filesystem
} // prosoft

#endif // PS_CORE_FILESYSTEM_HPP
