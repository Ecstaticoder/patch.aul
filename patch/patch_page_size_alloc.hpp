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

#ifdef PATCH_SWITCH_PAGE_SIZE_ALLOC


#include "global.hpp"
#include "offset_address.hpp"
#include "util.hpp"

#include "config_rw.hpp"

namespace patch {

	// init at exedit load
	// VirtualAllocで確保する部分のサイズをページ単位に広げる

	inline class page_size_alloc_t {

		bool enabled = true;
		bool enabled_i;
		inline static const char key[] = "page_size_alloc";

	public:

		void init() {
			enabled_i = enabled;

			if (!enabled_i)return;

			{ // 画像データのキャッシュ数
				/*
					1000d50f 8d04c510000000     lea     eax,dword ptr [eax*8+00000010]
					1000d516 3d00000400         cmp     eax,00040000
					↓
					1000d50f 8d04c50f100000     lea     eax,dword ptr [eax*8+0000100f]
					1000d516 e8XxXxXxXx         call    cursor

					10000000 2500f0ffff         and     eax,fffff000
					10000000 3d00000400         cmp     eax,00040000
					10000000 c3                 ret
				*/

				auto& cursor = GLOBAL::executable_memory_cursor;

				constexpr int vp_begin = 0xd512;
				OverWriteOnProtectHelper h(GLOBAL::exedit_base + vp_begin, 0xd51b - vp_begin);
				h.store_i16(0xd512 - vp_begin, '\x0f\x10');
				h.store_i8(0xd516 - vp_begin, '\xe8');
				h.replaceNearJmp(0xd517 - vp_begin, cursor);

				store_i32(cursor, '\x25\x00\xf0\xff'); cursor += 4;
				store_i32(cursor, '\xff\x3d\x00\x00'); cursor += 3;
				store_i32(cursor, '\x00\x04\x00\xc3'); cursor += 4;
			}

			{ // exdata buffer
				/*
					1006da0e 81c200710200       add     edx,00027100
					1006dac1 2d00710200         sub     eax,00027100
				*/
				constexpr int vp_begin = 0x6da11;
				OverWriteOnProtectHelper h(GLOBAL::exedit_base + vp_begin, 0x6dac4 - vp_begin);
				h.store_i8(0x6da11 - vp_begin, '\x80');
				h.store_i8(0x6dac3 - vp_begin, '\x80');
			}
			{ // undo buffer
				/*
					1008d18d 6800983a00           push    003a9800
					1008d197 c705184e241000983a00 mov     dword ptr [10244e18],003a9800
				*/
				constexpr int vp_begin = 0x8d18f;
				OverWriteOnProtectHelper h(GLOBAL::exedit_base + vp_begin, 0x8d19f - vp_begin);
				h.store_i8(0x8d18f - vp_begin, '\xa0');
				h.store_i8(0x8d19e - vp_begin, '\xa0');
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


	} page_size_alloc;
} // namespace patch

#endif // ifdef PATCH_SWITCH_PAGE_SIZE_ALLOC