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
    // init at aviutl or exedit load
    // トラックバーを動かすときなど、動かし初めしかset_undoされなかったりするため、変更が分からないことがある
    // ここではトラックの変化やチェックの変化などが検出できる（全ての変更は検出できないので他はundo_id_max等を使う）

    inline class update_count_t {

        bool enabled = true;
        bool enabled_i;

        inline static const char key[] = "update_count";

        inline static int count = 0;

        static int inc_count_return1() {
            count++;
            return 1;
        }

    public:

        void init_au() {
            enabled_i = enabled;

            if (!enabled_i)return;

            auto& cursor = GLOBAL::executable_memory_cursor;
            {
                /* 各プラグインのトラックバーやチェックなど
                    00430da9 b801000000         mov     eax,00000001
                    ↓
                    00430da9 e8XxXxXxXx         call    nesfunc_return_1
                */
                OverWriteOnProtectHelper h(GLOBAL::aviutl_base + 0x30da9, 5);
                h.store_i8(0, '\xe8');
                h.replaceNearJmp(1, &inc_count_return1);
            }
            { // フィルタの有効/無効
                {
                    /*
                        00413efb 837f4000           cmp     dword ptr [edi+40],+00
                        00413eff 7412               jz      413f13
                        00413f01
                        ↓
                        00413efb e9XxXxXxXx         jmp     cursor

                        cursor00 ff05XxXxXxXx       inc     dword ptr [count]
                        cursor06 837f4000           cmp     dword ptr [edi+40],+00
                        cursor0a 0f84XxXxXxXx       jz      au+13f13
                        cursor10 e9XxXxXxXx         jmp     au+13f01
                    */
                    OverWriteOnProtectHelper h(GLOBAL::aviutl_base + 0x13efb, 5);
                    h.store_i8(0, '\xe9');
                    h.replaceNearJmp(1, cursor);

                    store_i16(cursor, '\xff\x05'); cursor += 2;
                    store_i32(cursor, &count); cursor += 4;
                    store_i32(cursor, '\x83\x7f\x40\x00'); cursor += 4;
                    store_i16(cursor, '\x0f\x84'); cursor += 2;
                    store_i32(cursor, GLOBAL::aviutl_base + 0x13f13 - (int)cursor - 4); cursor += 4;
                    store_i8(cursor, '\xe9'); cursor++;
                    store_i32(cursor, GLOBAL::aviutl_base + 0x13f01 - (int)cursor - 4); cursor += 4;
                }
                {
                    /*
                        00430336 837e4000           cmp     dword ptr [esi+40],+00
                        0043033a 0f84d9f9ffff       jz      0042fd19
                        ↓
                        00430336 e9XxXxXxXx         jmp     cursor
                        00430340

                        cursor00 ff05XxXxXxXx       inc     dword ptr [count]
                        cursor06 837e4000           cmp     dword ptr [esi+40],+00
                        cursor0a 0f84XxXxXxXx       jz      au+2fd19
                        cursor10 e9XxXxXxXx         jmp     au+30340

                    */
                    OverWriteOnProtectHelper h(GLOBAL::aviutl_base + 0x30336, 5);
                    h.store_i8(0, '\xe9');
                    h.replaceNearJmp(1, cursor);

                    store_i16(cursor, '\xff\x05'); cursor += 2;
                    store_i32(cursor, &count); cursor += 4;
                    store_i32(cursor, '\x83\x7e\x40\x00'); cursor += 4;
                    store_i16(cursor, '\x0f\x84'); cursor += 2;
                    store_i32(cursor, GLOBAL::aviutl_base + 0x2fd19 - (int)cursor - 4); cursor += 4;
                    store_i8(cursor, '\xe9'); cursor++;
                    store_i32(cursor, GLOBAL::aviutl_base + 0x30340 - (int)cursor - 4); cursor += 4;
                }
            }
        }
        void init_ee() {

            if (!enabled_i)return;

            /* 拡張編集のトラックバーやチェックなど
                100357ea 8b2da40f1e10       mov     ebp,dword ptr [ExEdit.ObjectArrayPointer]
                ↓
                100357ea 90                 nop
                100357eb e8XxXxXxXx         call    cursor

                cursor00 8b2dXxXxXxxx       mov     ebp,dword ptr [ExEdit.ObjectArrayPointer]
                cursor06 ff05XxXxXxXx       inc     dword ptr [count]
                cursor0c c3                 ret

            */
            auto& cursor = GLOBAL::executable_memory_cursor;
            OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x357ea, 6);
            h.store_i16(0, '\x90\xe8');
            h.replaceNearJmp(2, cursor);

            store_i16(cursor, '\x8b\x2d'); cursor += 2;
            store_i32(cursor, GLOBAL::exedit_base + OFS::ExEdit::ObjectArrayPointer); cursor += 4;
            store_i16(cursor, '\xff\x05'); cursor += 2;
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
