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

#include "patch_tra_aviutlfilter.hpp"

#ifdef PATCH_SWITCH_TRA_AVIUTL_FILTER
namespace patch {
    __declspec(naked) void __cdecl tra_aviutlfilter_t::asm_func() {
        __asm {
            add     eax, ebx
            push    eax
            mov     dl, [ecx + 0x03]
            test    dl, 0x04
            jz      skip
                mov     edx, dword ptr [ecx + 0x000000d0]
                mov     ecx, dword ptr [ecx + 0x000000cc]
                test    ecx, ecx
                jnz     ee_x65789
            skip:
            xor     edx, edx
            push    edx
            inc     edx
            jmp     dword ptr [ee.x6578d]

            ee_x65789:
            jmp     dword ptr [ee.x65789]
        }
    }

} // namespace patch
#endif // ifdef PATCH_TRA_AVIUTL_FILTER