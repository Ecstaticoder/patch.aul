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

#ifdef PATCH_SWITCH_AUDIO_EFPI_INIT

#include <exedit.hpp>
#include "global.hpp"
#include "util.hpp"
#include "restorable_patch.hpp"

#include "config_rw.hpp"

namespace patch {
    // init at exedit load
    // audio_func_mainでefpiの0埋めを行う
    // ランダム移動でefpip->obj_indexが使われる

    inline class audio_efpi_init_t {

        bool enabled = true;
        bool enabled_i;

        inline static const char key[] = "audio_efpi_init";

        static int __stdcall GetTickCount_wrap(void* esp) {
            auto efpip = reinterpret_cast<ExEdit::FilterProcInfo*>((int)esp + 0x1c);
            memset(reinterpret_cast<void*>((int)efpip + sizeof(AviUtl::FilterProcInfo)), 0, 432 - sizeof(AviUtl::FilterProcInfo)); // sizeof(ExEdit::FilterProcInfo) = 432 true_size
            return GetTickCount();
        }

    public:

        void init() {
            enabled_i = enabled;

            if (!enabled_i)return;

            /*
                10049ce4 ff15a0a10910       call    KERNEL32.DLL::GetTickCount
                ↓
                10049ce4 54                 push    esp
                10049ce5 e8XxXxXxXx         call    newfunc_stdcall
            */
            OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x49ce4, 6);
            h.store_i16(0, '\x54\xe8');
            h.replaceNearJmp(2, &GetTickCount_wrap);

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

    } audio_efpi_init;
} // namespace patch
#endif // ifdef PATCH_SWITCH_AUDIO_EFPI_INIT
