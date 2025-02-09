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

#ifdef PATCH_SWITCH_LUA_LOAD
#include <memory>

#include "global.hpp"
#include "offset_address.hpp"
#include "util.hpp"
#include "config_rw.hpp"

#include "patch_fast_text.hpp"

namespace patch {

    // init at exedit load
    /* obj.load("text")にてサイズがおかしくなるバグの修正
    発生機序
        仕様として、高精度モードが有効の場合、フォントを2倍のサイズで生成してから元のサイズに縮小しつつ高精度化の処理をする

        obj.load("text")では高精度モードが有効であるものとして処理されるが、それは直前のテキストオブジェクトが高精度モードでない場合でも同様。
        つまりその場合、フォントを2倍のサイズにしていないのに縮小する処理が走る

    対処法
        fast.textのMyCreateFontにて最終の高精度モードフラグを保存しておき、obj.load("text")の高精度モードフラグに反映させる
    */

    inline class lua_load_t {
        bool enabled = true;
        bool enabled_i;
        inline static const char key[] = "lua.load";

    public:
        void init() {
            enabled_i = enabled;

            if (!enabled_i)return;
            if (!fast::text.is_enabled())return;

            { // obj.load("text")
                /*
                    1005f28f 6840000600         push    00060040
                    ↓
                    1005f28f e9XxXxXxXx         jmp     cursor

                    cursor00 833dXxXxXxXx00     cmp     dword ptr [&current_high_precision_flag],+00
                    cursor07 7407               jz      skip,07
                    cursor09 6840000600         push    00060040
                    cursor0e eb02               jmp     skip,02
                    cursor10 6a40               push    +40
                    cursor12 e9XxXxXxXx         jmp     ee+5f294
                */
                auto& cursor = GLOBAL::executable_memory_cursor;

                OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x05f28f, 5);
                h.store_i8(0, '\xe9');
                h.replaceNearJmp(1, cursor);

                store_i16(cursor, '\x83\x3d'); cursor += 2;
                store_i32(cursor, &fast::text.current_high_precision_flag); cursor += 4;
                store_i32(cursor, '\x00\x74\x07\x68'); cursor += 4;
                store_i32(cursor, '\x40\x00\x06\x00'); cursor += 4;
                store_i32(cursor, '\xeb\x02\x6a\x40'); cursor += 4;
                store_i16(cursor, '\xe9'); cursor++;
                store_i32(cursor, GLOBAL::exedit_base + 0x5f294 - (int)cursor - 4); cursor += 4;
            }
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
    } lua_load;
} // namespace patch

#endif // ifdef PATCH_SWITCH_LUA_LOAD
