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

#ifdef PATCH_SWITCH_AUDIO_LAYER_END

#include "global.hpp"
#include "util.hpp"
#include "restorable_patch.hpp"

#include "config_rw.hpp"

namespace patch {
    // init at exedit load
    // audio_func_mainでのlayer_end引数が機能していないのを修正

    inline class audio_layer_end_t {

        bool enabled = true;
        bool enabled_i;

        inline static const char key[] = "audio_layer_end";

        inline static struct _ofs {
            int32_t x49ebc = 0x49ebc;
            int32_t x4a008 = 0x4a008;
        }ee;
        static void __cdecl asm_func();
        // add_base(GLOBAL::exedit_base, &ee, sizeof(ee));

    public:

        void init() {
            enabled_i = enabled;

            if (!enabled_i)return;

            add_base(GLOBAL::exedit_base, &ee, sizeof(ee));

            /*
                10049eb6 0f854c010000       jnz     1004a008
                ↓
                10049eb6 e9XxXxXxXx         jmp     cursor

                10000000 7513               jnz     skip,13
                10000000 8b8c2480070000     mov     ecx,dword ptr [esp+00000780]
                10000000 3b8fc0050000       cmp     ecx,dword ptr [edi+000005c0]
                10000000 0f8fXxXxXxXx       jg      ee+49ebc
                10000000 e9XxXxXxXx         jmp     ee+4a008
            */
            OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x49eb6, 5);
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

    } audio_layer_end;
} // namespace patch
#endif // ifdef PATCH_SWITCH_AUDIO_LAYER_END
