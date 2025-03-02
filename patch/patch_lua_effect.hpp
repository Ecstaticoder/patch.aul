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

#ifdef PATCH_SWITCH_LUA_EFFECT
#include <memory>

#include "global.hpp"
#include "offset_address.hpp"
#include "util.hpp"
#include "config_rw.hpp"


namespace patch {

    // init at exedit load
    // obj.effect("filter_name")にてefp->proccesingが変わらないのを修正

    inline class lua_effect_t {
        bool enabled = true;
        bool enabled_i;
        inline static const char key[] = "lua.effect";

    public:
        void init() {
            enabled_i = enabled;

            if (!enabled_i)return;

            {
                /*
                    1005d20e 8983f4000000       mov     dword ptr [ebx+000000f4],eax
                    ↓
                    1005d20e 90                 nop
                    1005d20f e8XxXxXxXx         call    cursor

                    10000000 8b3dXxXxXxXx       mov     edi,dword ptr [ee+1b2b10]
                    10000000 8bbfe4000000       mov     edi,dword ptr [edi+000000e4]
                    10000000 89bbe4000000       mov     dword ptr [ebx+000000e4],edi
                    10000000 8983f4000000       mov     dword ptr [ebx+000000f4],eax
                    10000000 c3                 ret
                */

                auto& cursor = GLOBAL::executable_memory_cursor;

                OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x5d20e, 6);
                h.store_i16(0, '\x90\xe8');
                h.replaceNearJmp(2, cursor);

                static const char code_put[] =
                    "\x8b\x3dXXXX"              // mov     edi,dword ptr [ee+1b2b10]
                    "\x8b\xbf\xe4\x00\x00\x00"  // mov     edi,dword ptr [edi+000000e4]
                    "\x89\xbb\xe4\x00\x00\x00"  // mov     dword ptr [ebx+000000e4],edi
                    "\x89\x83\xf4\x00\x00\x00"  // mov     dword ptr [ebx+000000f4],eax
                    "\xc3"                      // ret
                    ;

                memcpy(cursor, code_put, sizeof(code_put) - 1);
                store_i32(cursor + 2, GLOBAL::exedit_base + OFS::ExEdit::script_efp);

                cursor += sizeof(code_put) - 1;

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
    } lua_effect;
} // namespace patch

#endif // ifdef PATCH_SWITCH_LUA_EFFECT
