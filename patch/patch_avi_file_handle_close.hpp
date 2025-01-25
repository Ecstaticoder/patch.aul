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

#ifdef PATCH_SWITCH_AVI_FILE_HANDLE_CLOSE

#include <aviutl.hpp>
#include <exedit.hpp>
#include "global.hpp"
#include "util.hpp"
#include "restorable_patch.hpp"

#include "config_rw.hpp"

namespace patch {
    // init at exedit load
    // 動画ファイルオブジェクトを削除してもハンドルを閉じないように変更（閉じるのは最大数を超えた時などに自動で行われる方に任せる）
    // 分割した一部を消す時にハンドルを閉じて重いのが改善される
    // 但し、AviUtlを開いたままファイルを消したり置き換えるためには動画ハンドル開放プラグインなどを使う必要があるようになる

    // 音声ファイルに対しても同じようにすることはできるけど、あまり重くないはずなので変更なし

    inline class avi_file_handle_close_t {

        static void __cdecl avi_handle_close_wrap(ExEdit::Filter*);
        static void __cdecl delete_all_object_wrap();

        bool enabled = true;
        bool enabled_i;

        inline static const char key[] = "avi_file_handle_close";

    public:

        void init() {
            enabled_i = enabled;

            if (!enabled_i)return;
            
            { // movie file
                OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x6858, 5);
                h.store_i32(0, '\x0f\x1f\x44\x00');
                h.store_i8(4, '\x00');
            }
            { // audio file （動画ファイルと連携のみ）
                /*
                    1009023a 57                 push    edi
                    1009023b e8e04bf7ff         call    10004e20
                    ↓
                    1009023a 56                 push    esi
                    1009023b e8XxXxXxXx         call    newfunc
                */
                OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x9023a, 5);
                h.store_i8(0, '\x56');
                h.replaceNearJmp(2, &avi_handle_close_wrap);
            }

            { // WndProc WM_FILTER_FILE_CLOSE にてハンドルを全て解放する
                ReplaceNearJmp(GLOBAL::exedit_base + 0x3ed23, &delete_all_object_wrap);
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

    } avi_file_handle_close;
} // namespace patch
#endif // ifdef PATCH_SWITCH_AVI_FILE_HANDLE_CLOSE
