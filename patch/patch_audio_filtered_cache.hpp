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

#ifdef PATCH_SWITCH_AUDIO_FILTERED_CACHE

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

    inline class audio_filtered_cache_t {

        bool enabled = true;
        bool enabled_i;

        inline static const char key[] = "audio_filtered_cache";
        static BOOL __fastcall audio_auf_func_proc_wrap(AviUtl::FilterProcInfo* fpip, char* profile, AviUtl::FilterPlugin* exit_fp);

    public:

        void init() {
            enabled_i = enabled;

            if (!enabled_i)return;


            /*
                0041d686 e8b5320100         call    00430940
                0041d68b 83c404             add     esp,+04
                ↓
                0041d686 e8XxXxXxXx         call    audio_auf_func_proc_wrap
                0041d68b 0f1f00             nop
            */

            OverWriteOnProtectHelper h(GLOBAL::aviutl_base + 0x1d687, 7);
            h.store_i32(3, '\x00\x0f\x1f\x00');
            /* AviUtl1.10のfastcallは呼び出し側によるスタック掃除となっており、通常のfastcallに置き換える際にはそれを削除する必要がある */
            h.replaceNearJmp(0, &audio_auf_func_proc_wrap);
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

    } audio_filtered_cache;
} // namespace patch
#endif // ifdef PATCH_SWITCH_AUDIO_FILTERED_CACHE
