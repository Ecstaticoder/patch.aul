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

#ifdef PATCH_SWITCH_OBJ_IMAGELOOP
#include <memory>

#include <exedit.hpp>

#include "global.hpp"
#include "offset_address.hpp"
#include "util.hpp"

#include "config_rw.hpp"

namespace patch {

    // init at exedit load
    // サイズ0のオブジェクト（主に改行のみのテキスト）でオフセットアドレス5ab38、5ab5c、5a6bfが起こるのを修正
    // 画像ループにて個別オブジェクトにして何かしら効果を付けた時に、ループごとに元の画像に戻さず効果を付与し続けていくのを修正

    inline class obj_ImageLoop_t {

        static int __stdcall save_current_image(void* esp);
        static void __cdecl do_after_filter_effect_wrap(ExEdit::ObjectFilterIndex ofi, ExEdit::FilterProcInfo* efpip, int flag);


        bool enabled = true;
        bool enabled_i;
        inline static const char key[] = "obj_imageloop";

        inline static struct _ofs {
            int32_t x5a576 = 0x5a576;
        }ee;
        static void __cdecl asm_func_check_size();
        // add_base(GLOBAL::exedit_base, &ee, sizeof(ee));

    public:

        void init() {
            enabled_i = enabled;

            if (!enabled_i)return;


            add_base(GLOBAL::exedit_base, &ee, sizeof(ee));

            { // サイズ0のオブジェクト（主に改行のみのテキスト）でオフセットアドレス5ab38、5ab5c、5a6bfが起こるのを修正
                OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x05a570, 5);
                h.store_i8(0, '\xe9');
                h.replaceNearJmp(1, &asm_func_check_size);
                /*
                    1005a570 81ec8c000000       sub     esp,0000008c
                    ↓
                    1005a570 e9XxXxXxXx         call    cursor
                    1005a575 00

                    "\x8b\x4c\x24\x08"         // mov     ecx,dword ptr [esp+08] ; efpip
                    "\x33\xc0"                 // xor     eax,eax
                    "\x83\xc1\x70"             // add     ecx,+70
                    "\x39\x41\x44"             // cmp     dword ptr [ecx+44],eax ; efpip->obj_w
                    "\x7e\x05"                 // jng     skip,05
                    "\x39\x41\x48"             // cmp     dword ptr [ecx+48],eax
                    "\x7f\x02"                 // jg      skip,02
                    "\x40"                     // inc     eax
                    "\xc3"                     // ret
                    "\x81\xec\x8c\x00\x00\x00" // sub     esp,0000008c
                    "\xe9"                     // jmp     ee+5a576
                */
            }


            { // 画像ループにて個別オブジェクトにして何かしら効果を付けた時に、ループごとに元の画像に戻さず効果を付与し続けていくのを修正
                OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x05a80a, 6);
                h.store_i16(0, '\x54\xe8');
                h.replaceNearJmp(2, &save_current_image);
                /*
                    1005a80a 0f8e86010000    jle        1005a996
                    ; このjleで飛ぶことは無いはずなので置き換えてしまう
                    ↓

                    1005a80a 54              push    esp
                    1005a80b e8XxXxXxXx      call    stdcall_func_return0
                    ; ecxとedxは退避しなくても大丈夫そう
                    ; eaxは0に
                */

                ReplaceNearJmp(GLOBAL::exedit_base + 0x05a92c, &do_after_filter_effect_wrap);
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
    } ImageLoop;
} // namespace patch

#endif // ifdef PATCH_SWITCH_OBJ_IMAGELOOP
