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

#ifdef PATCH_SWITCH_TRACKBAR

#include "global.hpp"
#include "offset_address.hpp"
#include "util.hpp"

#include "config_rw.hpp"

namespace patch {
	// init at patch load
	// マウスホイールでトラックバーが動くのを修正（実際の値は変わらないので動かないようにするという修正）
	// トラックバーでクリックした位置に直接移動する にて-1への移動が出来ないのを修正
	inline class trackbar_t {

		bool enabled = true;
		bool enabled_i;
		inline static const char key[] = "trackbar";

		static LRESULT __stdcall SendMessageA_TBM_SETPOS_wrap(HWND hwnd, WPARAM wparam, LPARAM lparam);

		inline static struct _ofs {
			int32_t x5d342 = 0x5d342;
			int32_t x5d32a = 0x5d32a;
		} au;
		static void __cdecl asm_func_wheel();
		// add_base(GLOBAL::aviutl_base, &au, sizeof(au));

	public:
		void init() {
			enabled_i = enabled;

			if (!enabled_i)return;

			add_base(GLOBAL::aviutl_base, &au, sizeof(au));

			{ // マウスホイールでトラックバーが動くのを修正

				constexpr int vp_begin = 0x5d1f2;
				OverWriteOnProtectHelper h(GLOBAL::aviutl_base + vp_begin, 0x5d203 - vp_begin);
				h.replaceNearJmp(0x5d1f2 - vp_begin, &asm_func_wheel);
				h.replaceNearJmp(0x5d1ff - vp_begin, &asm_func_wheel);
				/*
					00000000 817d0c0a020000     cmp     dword ptr [ebp+0c],0000020a ; WM_MOUSEWHEEL
					00000000 0f85XxXxXxXx       jnz     au+5d342
					00000000 e9XxXxXxXx         jmp     au+5d32a
				*/
			}

			{ // トラックバーでクリックした位置に直接移動する にて-1への移動が出来ないのを修正
				/*
					0045d2ff 6805040000         push    00000405
					0045d304 53                 push    ebx
					0045d305 ffd6               call    esi
					↓
					0045d2ff 53                 push    ebx
					0045d300 6690               nop
					0045d302 e8XxXxXxXx         call    newfunc
				*/
				OverWriteOnProtectHelper h(GLOBAL::aviutl_base + 0x5d2ff, 8);
				h.store_i32(0, '\x53\x66\x90\xe8');
				h.replaceNearJmp(4, &SendMessageA_TBM_SETPOS_wrap);
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

	} trackbar;
} // namespace patch
#endif // ifdef PATCH_SWITCH_TRACKBAR
