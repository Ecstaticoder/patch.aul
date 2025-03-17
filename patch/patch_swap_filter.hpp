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

#ifdef PATCH_SWITCH_SWAP_FILTER

#include <exedit.hpp>

#include "global.hpp"
#include "offset_address.hpp"
#include "util.hpp"

#include "config_rw.hpp"

namespace patch {
    // init at exedit load
    // 「フィルタ効果を上/下に移動」の操作でメディアオブジェクトのアニメーション効果のタイムライン表示名が正常ではなくなるのを修正

    inline class swap_filter_t {

        static void __stdcall prev_swap(int object_idx, int filter_idx, int filter_ofs);

        bool enabled = true;
        bool enabled_i;

        inline static const char key[] = "swap_filter";

        static void __cdecl asm_func();

    public:

        void init() {
            enabled_i = enabled;

            if (!enabled_i)return;


            {
                /*
                    10033b69 85c0               test    eax,eax
                    10033b6b 7c02               jl      skip,+02
                    10033b6d 8bf8               mov     edi,eax
                    ↓
                    10033b69 90                 nop
                    10033b6a e8XxXxXxXx         call    cursor
                    
                    10000000 85c0               test    eax,eax
                    10000000 7c02               jl      skip,+02
                    10000000 8bf8               mov     edi,eax
                    10000000 51                 push    ecx
                    10000000 56                 push    esi
                    10000000 55                 push    ebp
                    10000000 57                 push    edi
                    10000000 e8XxXxXxXx         call    newfunc
                    10000000 59                 pop     ecx
                    10000000 c3
                */
                constexpr int vp_begin = 0x33b69;
                OverWriteOnProtectHelper h(GLOBAL::exedit_base + vp_begin, 0x33b6f - vp_begin);
                h.store_i16(0x33b69 - vp_begin, '\x90\xe8');
                h.replaceNearJmp(0x33b6b - vp_begin, &asm_func);
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

    } swap_filter;
} // namespace patch
#endif // ifdef PATCH_SWITCH_SWAP_FILTER
