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

#include "patch_exo_fold_gui.hpp"


#ifdef PATCH_SWITCH_EXO_FOLD_GUI
namespace patch {

	__declspec(naked) void __cdecl exo_fold_gui_t::asm_func_write() {
        __asm {
            push    eax
            and     eax, 0x07
            shr     eax, 0x01
            xor     eax, 0x02
            jz      skip
                push    eax
                push    offset fold_gui_write
                push    ebp
                call    ebx
                add     esp, 0x0c
                add     ebp, eax
            skip:
            pop     eax
            and     al, 0x01
            jnz     jnz_ee_x288c6
                jmp     dword ptr [ee.x288b7]
            jnz_ee_x288c6:
            jmp     dword ptr [ee.x288c6]
        }
	}
	__declspec(naked) void __cdecl exo_fold_gui_t::asm_func_read() {
        __asm {
            push    dword ptr [esp+0x14]
            push    offset fold_gui_read
            call    esi
            test    eax, eax
            jnz     skip
                push    ebx
                call    dword ptr [ee.x918ab]
                add     esp, 0x04
                test    eax, eax
                jz      skip
                and     eax, 0x03
                shl     eax, 0x01
                mov     ecx, dword ptr [esp+0x48]
                add     ecx, ebp
                add     ecx, 0x72
                xor     byte ptr [ecx+0x72], al
            skip:
            push    dword ptr [esp+0x14]
            jmp     dword ptr [ee.x29b26]
        }
	}
} // namespace patch
#endif // ifdef PATCH_SWITCH_EXO_FOLD_GUI
