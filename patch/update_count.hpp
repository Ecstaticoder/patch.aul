/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include "macro.h"

#ifdef PATCH_SWITCH_UPDATE_COUNT

#include <aviutl.hpp>
#include <exedit.hpp>

#include "global.hpp"
#include "util.hpp"
#include "restorable_patch.hpp"

#include "config_rw.hpp"

namespace patch {
    // init at exedit load
    // トラックバーを動かすときなど、動かし初めしかset_undoされなかったりするため、変更が分からないことがある
    // ここではトラックの変化やチェックの変化などが検出できる（全ての変更は検出できないので他はundo_id_max等を使う）

    inline class update_count_t {

        bool enabled = true;
        bool enabled_i;

        inline static const char key[] = "update_count";

        inline static int count = 0;

    public:

        void init() {
            enabled_i = enabled;

            if (!enabled_i)return;

            /*
                100357ea 8b2da40f1e10       mov     ebp,dword ptr [ExEdit.ObjectArrayPointer]
                ↓
                100357ea 90                 nop
                100357eb e8XxXxXxXx         call    cursor

                cursor00 8b0dXxXxXxXx       mov     ecx,dword ptr [count]
                cursor06 8b2dXxXxXxxx       mov     ebp,dword ptr [ExEdit.ObjectArrayPointer]
                cursor0c 41                 inc     ecx
                cursor0d 890dXxXxXxXx       mov     dword ptr [count],ecx
                cursor13 c3                 ret
            */
            auto& cursor = GLOBAL::executable_memory_cursor;
            OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x357ea, 6);
            h.store_i16(0, '\x90\xe8');
            h.replaceNearJmp(2, cursor);

            store_i16(cursor, '\x8b\x0d'); cursor += 2;
            store_i32(cursor, &count); cursor += 4;
            store_i16(cursor, '\x8b\x2d'); cursor += 2;
            store_i32(cursor, GLOBAL::exedit_base + OFS::ExEdit::ObjectArrayPointer); cursor += 4;
            store_i32(cursor, '\x41\x89\x0d\x00'); cursor += 3;
            store_i32(cursor, &count); cursor += 4;
            store_i8(cursor, '\xc3'); cursor++;
        }

        int get() {
            return count;
        }

        void switching(bool flag) {
            enabled = flag;
        }

        bool is_enabled() { return enabled; }
        bool is_enabled_i() { return enabled_i; }

        void switch_load(ConfigReader& cr) {
            cr.regist(key, [this](json_value_s* value) {
                ConfigReader::load_variable(value, enabled);
                });
        }

        void switch_store(ConfigWriter& cw) {
            cw.append(key, enabled);
        }

    } update_count;
} // namespace patch
#endif // ifdef PATCH_SWITCH_UPDATE_COUNT
