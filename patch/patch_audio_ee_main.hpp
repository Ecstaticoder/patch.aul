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

#ifdef PATCH_SWITCH_AUDIO_EE_MAIN

#include <aviutl.hpp>
#include <exedit.hpp>

#include "global.hpp"
#include "util.hpp"
#include "restorable_patch.hpp"

#include "config_rw.hpp"

#include "update_count.hpp"

namespace patch {
    // init at exedit load
    // 主に音声波形表示（編集全体の音声を元にする）で音がおかしくなるやつの対策のためにキャッシュを取る

    inline class audio_ee_main_t {

        bool enabled = true;
        bool enabled_i;

        inline static const char key[] = "audio_ee_main";
        static BOOL __cdecl audio_func_main_wrap(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, int end_layer, int add_frame, int audio_speed, int milliframe, int scene_idx, ExEdit::ObjectFilterIndex ofi);

    public:

        void init() {
            enabled_i = enabled;

            if (!enabled_i)return;

            if (!update_count.is_enabled_i())return;
            /*
                10049c88 e813000000         call    10049ca0
                ↓
                10049c88 e8XxXxXxXx         call    newfunc
            */
            ReplaceNearJmp(GLOBAL::exedit_base + 0x49c89, &audio_func_main_wrap);
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

    } audio_ee_main;
} // namespace patch
#endif // ifdef PATCH_SWITCH_AUDIO_EE_MAIN
