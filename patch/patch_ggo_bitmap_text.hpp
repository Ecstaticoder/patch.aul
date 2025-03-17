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

#ifdef PATCH_SWITCH_GGO_BITMAP_TEXT

#include <exedit.hpp>

#include "global.hpp"
#include "offset_address.hpp"
#include "util.hpp"
#include "global.hpp"
#include "config_rw.hpp"

#include "patch_fast_text_border.hpp"
#include "patch_border_only_text.hpp"

namespace patch {
	inline class ggo_bitmap_text_t {
		bool enabled = true;
		bool enabled_i;

		inline static const char key[] = "ggo_bitmap_text";

		inline static struct _ofs {
			int32_t x5fecd = 0x5fecd;
			int32_t x1b2b0c = 0x1b2b0c;
			int32_t x50c0c = 0x50c0c;
			int32_t x50cf6 = 0x50cf6;
		}ee;
		static void __cdecl asm_func_hp_text();
		static void __cdecl asm_func_hp_setfont();
		static void __cdecl asm_func_ggo_pre();
		static void __cdecl asm_func_ggo();
		// add_base(GLOBAL::exedit_base, &ee, sizeof(ee));

	public:

		inline static const char cb_str[] = "ｱﾝﾁｴｲﾘｱｽ無し";

		void init() {
			enabled_i = enabled;
			if (!enabled_i)return;
			if (!fast::textborder.is_enabled())return;
			if (!borderonly_text.is_enabled())return;

			add_base(GLOBAL::exedit_base, &ee, sizeof(ee));

			{ // ｱﾝﾁｴｲﾘｱｽ無し の場合に高精度モード処理は無効にする
				{ // テキスト
					/*
						1008a87c 0fbe4607           movsx   ax,byte ptr [esi+07]
						1008a880 f7d9               neg     ecx
						↓
						1008a87c 90                 nop
						1008a880 e8XxXxXxXx         call    cursor


						"\x33\xc0"                 // xor     eax,eax
						"\x80\x3e\x07"             // cmp     byte ptr [esi],07
						"\x74\x03"                 // jz      skip,03
						"\x8a\x46\x07"             // mov     al,[esi+07]
						"\xf7\xd9"                 // neg     ecx
						"\xc3"                     // ret
					*/

					OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x08a87c, 6);
					h.store_i16(0, '\x90\xe8');
					h.replaceNearJmp(2, &asm_func_hp_text);
				}
				{ // obj.setfont
					/*
						1005fec7 6a00               push    +00
						1005fec9 6a01               push    +01
						1005fecb 6a00               push    +00
						↓
						1005fec7 90                 nop
						1005fec8 e9XxXxXxXx         jmp     cursor

						cursor00 6a00               push    +00
						cursor02 833dXxXxXxXx07     cmp     dword ptr [ee+1b2b0c],+07
						cursor09 7404               jz      skip,04
						cursor0b 6a01               push    +01
						cursor0d eb02               jmp     skip,02
						cursor0e 6a00               push    +00
						cursor11 6a00               push    +00
						cursor13 e9XxXxXxXx         jmp     ee+5fecd
					*/
					OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x05fec7, 6);
					h.store_i16(0, '\x90\xe9');
					h.replaceNearJmp(2, &asm_func_hp_setfont);
				}
				{ // obj.load
					// patch_lua_loadのバグ修正にて自然と対応される
				}
			}
			
			{ // GGO_BITMAPかGGO_GRAY8かを振り分ける
				{
					/*
						10050c07 52                 push    edx
						10050c08 6a06               push    +06
						10050c0a 57                 push    edi
						10050c0b 50                 push    eax
						↓
						10050c07 e9XxXxXxXx         jmp     cursor

						"\x52"                     // push    edx
						"\x83\xbc\x24"
						"\xb0\x01\x00\x00\x07"     // cmp     dword ptr [esp+000001b0],+07
						"\x74\x04"                 // jz      skip,04
						"\x6a\x06"                 // push    +06
						"\xeb\x02"                 // jmp     skip,02
						"\x6a\x01"                 // push    +01
						"\x57"                     // push    edi
						"\x50"                     // push    eax
						"\xe9"// XXXX              // jmp     ee+50c0c
					*/
					OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x050c07, 5);
					h.store_i8(0, '\xe9');
					h.replaceNearJmp(1, &asm_func_ggo_pre);
				}
				{
					/*
						10050cf1 50                 push    eax
						10050cf2 6a06               push    +06
						10050cf4 57                 push    edi
						10050cf5 51                 push    ecx
						↓
						10050cf1 e9XxXxXxXx         jmp     cursor

						"\x50"                     // push    eax
						"\x83\xbc\x24"
						"\xb0\x01\x00\x00\x07"     // cmp     dword ptr [esp+000001b0],+07
						"\x74\x04"                 // jz      skip,04
						"\x6a\x06"                 // push    +06
						"\xeb\x02"                 // jmp     skip,02
						"\x6a\x01"                 // push    +01
						"\x57"                     // push    edi
						"\x51"                     // push    ecx
						"\xe9"// XXXX              // jmp     ee+50cf6
					*/
					OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x050cf1, 5);
					h.store_i8(0, '\xe9');
					h.replaceNearJmp(1, &asm_func_ggo);
				}
			}
		}

		void switching(bool flag) { enabled = flag; }

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

	} ggo_bitmap_text;

}
#endif // ifdef PATCH_SWITCH_GGO_BITMAP_TEXT
