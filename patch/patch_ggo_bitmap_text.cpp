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

#include "patch_ggo_bitmap_text.hpp"

#ifdef PATCH_SWITCH_GGO_BITMAP_TEXT
namespace patch {
    __declspec(naked) void __cdecl ggo_bitmap_text_t::asm_func_hp_text() {
        __asm {
            xor eax, eax
            cmp byte ptr[esi], 0x07
            jz skip
                mov al, [esi + 0x07]
            skip :
            neg ecx
            ret
        }
    }
    __declspec(naked) void __cdecl ggo_bitmap_text_t::asm_func_hp_setfont() {
        __asm {
            push    0x00
            cmp     dword ptr [ee.x1b2b0c], 0x07
            jz      push0case
                push    0x01
                jmp     push1case
            push0case :
                push    0x00
            push1case :
            push    0x00
            jmp     dword ptr [ee.x5fecd]
        }
    }

    __declspec(naked) void __cdecl ggo_bitmap_text_t::asm_func_ggo_pre() {
        __asm {
            push    edx
            cmp     dword ptr[esp + 0x000001b0], 0x07
            jz      push01case
                push    0x06
                jmp     push06case
            push01case :
                push    0x01
            push06case :
            push    edi
            push    eax
            jmp     dword ptr[ee.x50c0c]
        }
    }

    __declspec(naked) void __cdecl ggo_bitmap_text_t::asm_func_ggo() {
        __asm {
            push    eax
            cmp     dword ptr[esp + 0x000001b0], 0x07
            jz      push01case
                push    0x06
                jmp     push06case
            push01case :
                push    0x01
            push06case :
            push    edi
            push    ecx
            jmp     dword ptr[ee.x50cf6]
        }
    }
} // namespace patch
#endif // ifdef PATCH_GGO_BITMAP_TEXT