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

#ifdef PATCH_SWITCH_TRA_ACC_DEC_MOVEMENT

#include <memory>
#include <exedit.hpp>

#include "global.hpp"
#include "offset_address.hpp"
#include "util.hpp"
#include "restorable_patch.hpp"

#include "config_rw.hpp"

namespace patch {
	// init at exedit load
	// トラック変化方式の「加減速移動」で長いオブジェクトに中間点がある場合に正常ではなくなるのを修正
	inline class tra_acc_dec_movement_t {

		bool enabled = true;
		bool enabled_i;

		inline static const char key[] = "tra_acc_dec_movement";

		static void __stdcall p_wrap_66bac_66c29(int prev_idx, int len, void* esp);
		static void __stdcall n_wrap_66c43_66cb3(int next_idx, int len, void* esp);

	public:

		void init() {
			enabled_i = enabled;

			if (!enabled_i)return;

			constexpr int vp_begin = 0x66bac;
			OverWriteOnProtectHelper h(GLOBAL::exedit_base + vp_begin, 0x66ccf - vp_begin);
			h.store_i32(0x66bac - vp_begin, '\x54\x53\x50\xe8'); // push esp,ebx,eax  call
			h.replaceNearJmp(0x66bb0 - vp_begin, &p_wrap_66bac_66c29);
			h.store_i32(0x66bb4 - vp_begin, '\xe9\xfb\x00\x00'); // jmp ee+66cb4
			h.store_i8(0x66bb8 - vp_begin, '\x00');

			h.store_i32(0x66c43 - vp_begin, '\x54\x51\x50\xe8'); // push esp,ecx,eax  call
			h.replaceNearJmp(0x66c47 - vp_begin, &n_wrap_66c43_66cb3);
			h.store_i16(0x66c4b - vp_begin, '\xeb\x67'); // jmp ee+66cb4

			h.store_i16(0x66cb7 - vp_begin, '\x66\x90');
			h.store_i16(0x66cc4 - vp_begin, '\x0f\x1f');
			h.store_i8(0x66cc6 - vp_begin, '\x00');
			h.store_i16(0x66cc9 - vp_begin, '\x66\x0f');
			h.store_i32(0x66ccb - vp_begin, '\x1f\x44\x00\x00');

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

	} tra_acc_dec_movement;
} // namespace patch
#endif // ifdef PATCH_SWITCH_TRA_ACC_DEC_MOVEMENT
