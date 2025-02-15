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
#ifdef PATCH_SWITCH_SUSIE_LOAD

#include <exedit.hpp>

#include "util.hpp"
#include "global.hpp"
#include "offset_address.hpp"

#include "restorable_patch.hpp"
#include "config_rw.hpp"

namespace patch {
	// init at exedit load
	// Susieのプラグインで正しく対応拡張子情報を取得できないのを修正
    // 画像ファイル選択ダイアログにて拡張子が重複して表示されるのを修正
    // Susieプラグイン情報を追加
	inline class susie_load_t {
		static void __cdecl LoadSpi(LPCSTR dir);
        static void __cdecl FreeSpi();
        static void __cdecl set_susie_extension(char* ptr);
        static BOOL __cdecl get_picture(ExEdit::SpiImageData* spidata, char* path);

        bool enabled = true;
        bool enabled_i;

		inline static const char key[] = "susie_load";

        inline static int spi_count = 0;

	public:

		void init() {
            enabled_i = enabled;

            if (!enabled_i)return;
			

            ReplaceNearJmp(GLOBAL::exedit_base + 0x31544, LoadSpi);
            ReplaceNearJmp(GLOBAL::exedit_base + 0x31807, FreeSpi);
            ReplaceNearJmp(GLOBAL::exedit_base + 0x4cc5c, set_susie_extension);
            ReplaceNearJmp(GLOBAL::exedit_base + 0x4cb15, get_picture);
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


        char* get_info() {
            if (!enabled_i) return (char*)key;

            auto path = *(char**)(GLOBAL::exedit_base + OFS::ExEdit::memory_ptr);
            auto buf = path + _MAX_PATH + 16;
            auto ptr = buf;
            auto loaded_spi_array = reinterpret_cast<ExEdit::structSPI*>(GLOBAL::exedit_base + OFS::ExEdit::loaded_spi_array);
            constexpr int susie_max_n = 32;
            for (int i = 0; i < susie_max_n; i++) {
                auto spi = loaded_spi_array[i];
                if (spi.hmodule != nullptr) {
                    GetModuleFileNameA(spi.hmodule, path, _MAX_PATH);
                    ptr += wsprintfA(ptr, "%s [ %s ] ( %s )\n", spi.information, spi.extension, reinterpret_cast<char* (__fastcall*)(char*)>(GLOBAL::aviutl_base + OFS::AviUtl::get_filename_cursor)(path));
                }
            }

            *ptr = '\0';
            return buf;
        }

	}susie_load;
} // namespace patch
#endif // ifdef PATCH_SWITCH_SUSIE_LOAD
