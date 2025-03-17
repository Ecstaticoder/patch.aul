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

#ifdef PATCH_SWITCH_EXA_CAMERA
#include <memory>

#include <exedit.hpp>

#include "global.hpp"
#include "util.hpp"
#include "config_rw.hpp"

namespace patch {

    // init at exedit load
    // exaでエフェクトを追加するときにカメラ制御の対象が外れるのを修正
    inline class exa_camera_t {
        bool enabled = true;
        bool enabled_i;
        inline static const char key[] = "exa_camera";
        static void __stdcall get_obj_camera_flag(void* esp);

    public:
        void init() {
            enabled_i = enabled;

            if (!enabled_i)return;


            { // exo_write
                OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x29e05, 6);
                h.store_i32(0, '\x66\x90\x54\xe8');
                h.replaceNearJmp(4, &get_obj_camera_flag);
                /*
                    10029e05 897c2438           mov     dword ptr [esp+38], edi ; =-1
                    10029e09 895c2434           mov     dword ptr [esp+34], ebx ; =0
                    ↓
                    10029e05 6690               nop
                    10029e07 54                 push    esp
                    10029e08 e8XxXxXxXx         call    stdcall_func
                */
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
    } exa_camera;
} // namespace patch

#endif // ifdef PATCH_SWITCH_EXA_CAMERA
