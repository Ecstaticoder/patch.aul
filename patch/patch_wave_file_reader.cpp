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

#include "patch_wave_file_reader.hpp"


#ifdef PATCH_SWITCH_WAVE_FILE_READER
namespace patch {

    __declspec(naked) void __cdecl wave_file_reader_t::asm_func() {
        enum {
            ng_wave_n = _ng_wave_n,
        };
        __asm {
            mov     ax, word ptr [esi + 0x10]
            mov     edx, offset ng_wave_format
            mov     ecx, ng_wave_n
            cmp     ax, 0xFFFE
            jnz     skip
                mov     ax, word ptr [esi + 0x28]
                mov     word ptr [esi + 0x10], ax
                mov     word ptr [esi + 0x20], 0x0000
            skip:
                cmp     ax, word ptr [edx]
                jz      au_x28d9d
                add     edx, 0x02
            loop    skip
            mov     ecx, dword ptr [esi]
            lea     eax, [ebp - 0x20]
            jmp     dword ptr [au.x28ca8]

            au_x28d9d:
            jmp     dword ptr [au.x28d9d]
        }
    }
} // namespace patch
#endif // ifdef PATCH_SWITCH_WAVE_FILE_READER
