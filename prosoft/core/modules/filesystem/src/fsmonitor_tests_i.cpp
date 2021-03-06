// Copyright © 2016, Prosoft Engineering, Inc. (A.K.A "Prosoft")
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

// DO NOT INCLUDE IN BUILD TARGET SOURCE.
// This is an implementation include file.

SECTION("notification") {
    auto state = std::make_shared<platform_state>();
    auto reg = change_manager::make_registration(state);
    CHECK(reg);
    auto note = change_manager::make_notification(PS_TEXT("test"), PS_TEXT(""), state.get(), change_event::created);
    CHECK(note.path() == path{PS_TEXT("test")});
    CHECK(note.renamed_to_path().empty());
    CHECK(note.event() == change_event::created);
    CHECK(note == reg);
    CHECK_FALSE(type_known(note));
    
    auto p = note.extract_path();
    CHECK(p.native() == PS_TEXT("test"));
    CHECK(note.path().empty());
    CHECK(note.event() == change_event::none);
    CHECK_FALSE(note == reg);
    
    note = change_manager::make_notification(PS_TEXT(""), PS_TEXT(""), nullptr, change_event::created, file_type::regular);
    CHECK(type_known(note));
    CHECK(created(note));
    
    note = change_manager::make_notification(PS_TEXT(""), PS_TEXT(""), nullptr, change_event::removed);
    CHECK(removed(note));
    
    note = change_manager::make_notification(PS_TEXT(""), PS_TEXT(""), nullptr, change_event::renamed);
    CHECK(renamed(note));
    
    note = change_manager::make_notification(PS_TEXT(""), PS_TEXT(""), nullptr, change_event::content_modified);
    CHECK(content_modified(note));
    CHECK(modified(note));
    
    note = change_manager::make_notification(PS_TEXT(""), PS_TEXT(""), nullptr, change_event::metadata_modified);
    CHECK(metadata_modified(note));
    CHECK(modified(note));
    
    note = change_manager::make_notification(PS_TEXT(""), PS_TEXT(""), nullptr, change_event::rescan_required);
    CHECK(rescan(note));
    CHECK(canceled(note));
    
    // test copy/move into vector
    change_notifications notes;
// Xcode 7&8 ASAN both fire "heap buffer overflow" for change_manager default copy and move if we use reserve.
// The output seems to point to some memory allocated by Catch, it may be the trigger as I cannot reproduce the problem with a simple test binary.
#if 0
    notes.reserve(2);
#endif
    note = change_manager::make_notification(PS_TEXT("test"), path{}, nullptr, change_event::rescan_required);
    notes.push_back(note);
    notes.emplace_back(change_manager::make_notification(PS_TEXT("test"), path{}, nullptr, change_event::rescan_required));
    CHECK(notes.size() == 2);
}
