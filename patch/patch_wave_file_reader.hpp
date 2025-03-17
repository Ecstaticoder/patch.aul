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

#ifdef PATCH_SWITCH_WAVE_FILE_READER

#include "global.hpp"
#include "offset_address.hpp"
#include "util.hpp"

#include "config_rw.hpp"

#include <mmreg.h>

namespace patch {

    // init at patch load
    // WaveFileReaderで読み込めない形式をスキップして別の入力プラグインに任せるようにする
    // WAVE_FORMAT_EXTENSIBLE + WAVE_FORMAT_ALAWが何故か読めなかったので強引な置き換えをやってみたが副作用は不明

    /*
        0x0001	WAVE_FORMAT_PCM	PCM	    → OK
        0x0002  WAVE_FORMAT_ADPCM       → OK
        0x0003	WAVE_FORMAT_IEEE_FLOAT	→ NG
        0x0006	WAVE_FORMAT_ALAW        → OK
        0x0007	WAVE_FORMAT_MULAW   	→ OK
        0x0011	WAVE_FORMAT_DVI_ADPCM   → OK
        0x0031	WAVE_FORMAT_GSM610      → OK
        0x0038	WAVE_FORMAT_NMS_VBXADPCM→ NG    L-SMASH WorksでもNG
        0x0040	WAVE_FORMAT_G721_ADPCM  → NG    L-SMASH Worksで音おかしい
        0xFFFE	WAVE_FORMAT_EXTENSIBLE	→ subformatから判定
    */
    
    inline static const short ng_wave_format[] = {
        WAVE_FORMAT_IEEE_FLOAT,
        WAVE_FORMAT_NMS_VBXADPCM,
        WAVE_FORMAT_G721_ADPCM
    };
    constexpr int _ng_wave_n = sizeof(ng_wave_format) / sizeof(*ng_wave_format); // インラインアセンブリではさらにenumにする必要あり

    inline class wave_file_reader_t {

        bool enabled = true;
        bool enabled_i;
        inline static const char key[] = "wave_file_reader";


        inline static struct _ofs {
            int32_t x28ca8 = 0x28ca8;
            int32_t x28d9d = 0x28d9d;
        }au;
        static void __cdecl asm_func();
        // add_base(GLOBAL::aviutl_base, &au, sizeof(au));

    public:
        void init() {
            enabled_i = enabled;
            
            if (!enabled_i)return;

            add_base(GLOBAL::aviutl_base, &au, sizeof(au));

            /*
                00428ca3 8b0e               mov     ecx,dword ptr [esi]
                00428ca5 8d45e0             lea     eax,dword ptr [ebp-20]
                00428ca8
                ↓
                00428ca3 e9XxXxXxXx         jmp     cursor
                00428ca8

                10000000 0fb74e10           movzx   ecx,dword ptr [esi+10] ; format_code
                10000000 81f9feff0000       cmp     ecx,0000fffe ; WAVE_FORMAT_EXTENSIBLE
                10000000 750e               jnz     skip,+0e
                10000000 0fb74e28           movzx   ecx,dword ptr [esi+28] ; subformat_code
                10000000 66894e10           mov     [esi+10],cx
                10000000 66c746200000       mov     word ptr [esi+20],0000
                10000000 baXxXxXxXx         mov     edx, &ng_wave
                10000000 33c0               xor     eax,eax
                10000000 668b02             mov     ax,[edx]
                10000000 3bc8               cmp     ecx,eax
                10000000 0f84XxXxXxXx       jz      aviutl + 28d9d
                10000000 83c202             add     edx,+02
                10000000 81faXxXxXxXx       cmp     edx, &ng_wave + sizeof(ng_wave)
                10000000 7cea               jl      back,16
                10000000 8b0e               mov     ecx,dword ptr [esi]
                10000000 8d45e0             lea     eax,dword ptr [ebp-20]
                10000000 e9XxXxXxXx         jmp     aviutl + 28ca8

            */
            
            OverWriteOnProtectHelper h(GLOBAL::aviutl_base + 0x028ca3, 5);
            h.store_i8(0, '\xe9');
            h.replaceNearJmp(1, &asm_func);
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
    } wave_file_reader;
} // namespace patch

#endif // ifdef PATCH_SWITCH_WAVE_FILE_READER
