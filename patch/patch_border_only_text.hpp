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

#ifdef PATCH_SWITCH_BORDER_ONLY_TEXT

#include <exedit.hpp>

#include "global.hpp"
#include "offset_address.hpp"
#include "util.hpp"
#include "global.hpp"
#include "config_rw.hpp"

#include "patch_fast_text_border.hpp"

namespace patch {
	inline class BorderOnlyText_t {
		static void apend_type();
		bool enabled = true;
		bool enabled_i;

		inline static const char key[] = "borderonly_text";

		inline static const char border_only_str[] = "縁のみ";
		inline static const char border_thin_only_str[] = "縁のみ(細)";

		inline static struct _ofs {
			int32_t xb8f18 = 0xb8f18;
			int32_t x503d7 = 0x503d7;
			int32_t x5035a = 0x5035a;
			int32_t x5039b = 0x5039b;
			int32_t x503cf = 0x503cf;
			int32_t x50872 = 0x50872;
			int32_t x507fb = 0x507fb;
			int32_t x50831 = 0x50831;
			int32_t x5085e = 0x5085e;
			int32_t x50e5b = 0x50e5b;
			int32_t x50f9e = 0x50f9e;
			int32_t x1aee20 = 0x1aee20;
			int32_t x1aee24 = 0x1aee24;
			int32_t x1aebdc = 0x1aebdc;
			int32_t x1a5328 = 0x1a5328;
		}ee;
		static void __cdecl asm_func_apend_type();
		static void __cdecl asm_func_if_type_1();
		static void __cdecl asm_func_if_type_2();
		static void __cdecl asm_func_if_type_3();
		// add_base(GLOBAL::exedit_base, &ee, sizeof(ee));

	public:

		void init() {
			enabled_i = enabled;
			if (!enabled_i)return;
			if (!fast::textborder.is_enabled())return;

			add_base(GLOBAL::exedit_base, &ee, sizeof(ee));

			{ // テキストのコンボボックスに縁のみを追加する
				// 1008c657 bf188f0b10         mov     edi,100b8f18

				OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x08c657, 5);
				h.store_i8(0, '\xe8');
				h.replaceNearJmp(1, &asm_func_apend_type);
				/*
				static const char code_put[] =
					"\xe8XXXX"                     // call    newfunc
					"\xbfXXXX"                     // mov     edi,exedit+b8f18
					"\xc3"                         // ret
					;
				*/

			}
			{ // 縁のみの時の処理を定義する
				{
					/*
						100503ca 83ff02             cmp     edi,+02
						100503cd 7408               jz      exedit+503d7
						↓
						100503ca e9XxXxXxXx         jmp     cursor

					static const char code_put[] =
						"\x83\xff\x02"             // cmp     edi,+02
						"\x0f\x84XXXX"             // jz      exedit+503d7
						"\x83\xff\x05"             // cmp     edi,+05
						"\x0f\x84XXXX"             // jz      exedit+5035a
						"\x83\xff\x06"             // cmp     edi,+06
						"\x0f\x84XXXX"             // jz      exedit+5039b
						"\xe9"// XXXX              // jmp     exedit+503cf
						;
					*/
					OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x0503ca, 5);
					h.store_i16(0, '\xe9');
					h.replaceNearJmp(1, &asm_func_if_type_1);

				}

				{
					/*
						10050859 83f802             cmp     eax,+02
						1005085c 7414               jz      exedit+50872
						↓
						10050859 e9XxXxXxXx         jmp     cursor

					static const char code_put[] =
						"\x83\xf8\x02"             // cmp     eax,+02
						"\x0f\x84XXXX"             // jz      exedit+50872
						"\x83\xf8\x05"             // cmp     eax,+05
						"\x0f\x84XXXX"             // jz      exedit+507fb
						"\x83\xf8\x06"             // cmp     eax,+06
						"\x0f\x84XXXX"             // jz      exedit+50831
						"\xe9"// XXXX              // jmp     exedit+5085e
						;
					*/
					OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x050859, 5);
					h.store_i16(0, '\xe9');
					h.replaceNearJmp(1, &asm_func_if_type_2);

				}

				{
					/*
						10050e55 8d42ff             lea     eax,dword ptr [edx-01]
						10050e58 83f803             cmp     eax,+03
						↓
						10050e55 90                 nop
						10050e56 e9XxXxXxXx         jmp     cursor
					*/
					OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x050e55, 6);
					h.store_i16(0, '\x90\xe9');
					h.replaceNearJmp(2, &asm_func_if_type_3);

					/*
					// type == 5 || type == 6 なら次のcodeへ、 それ以外なら元の処理へ
					static const char code_put1[] =
						"\x83\xfa\x05"             // cmp     edx,+05
						"\x74\x10"                 // jz      skip,+10
						"\x83\xfa\x06"             // cmp     edx,+06
						"\x74\x0b"                 // jz      skip,+0b
						"\x8d\x42\xff"             // lea     eax,dword ptr [edx-01]
						"\x83\xf8\x03"             // cmp     eax,+03
						"\xe9"// XXXX              // jmp     exedit+50e5b
						;
					*/
					/* 以下の部分のコードを再利用
						10050e68 8b442418           mov     eax,dword ptr [esp+18]
						10050e6c 8b9424a0010000     mov     edx,dword ptr [esp+000001a0]
						10050e73 50                 push    eax
						10050e74 8b442468           mov     eax,dword ptr [esp+68]
						10050e78 52                 push    edx
						10050e79 8b1520ee1a10       mov     edx,dword ptr [101aee20]
						10050e7f 51                 push    ecx
						10050e80 8b0d24ee1a10       mov     ecx,dword ptr [101aee24]
						10050e86 50                 push    eax
						10050e87 a1dceb1a10         mov     eax,[101aebdc]
						10050e8c 51                 push    ecx
						10050e8d 8b0d28531a10       mov     ecx,dword ptr [101a5328]
						10050e93 52                 push    edx
						10050e94 8b942490010000     mov     edx,dword ptr [esp+00000190]
						10050e9b 50                 push    eax
						10050e9c 53                 push    ebx
						10050e9d 55                 push    ebp
						10050e9e 51                 push    ecx
						10050e9f 56                 push    esi
						10050ea0 57                 push    edi
						10050ea1 52                 push    edx
						10050ea2 e8                 call    text_border_func
					*/
					/*
					static const char code_put2[] =
						"\x83\xc4\x34"             // add     esp,+34
						"\xe9"                     // jmp     exedit+50f9e
						;
					*/
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

	} borderonly_text;

}
#endif
