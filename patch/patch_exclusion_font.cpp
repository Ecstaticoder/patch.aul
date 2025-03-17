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

#include "patch_exclusion_font.hpp"

#ifdef PATCH_SWITCH_EXCLUSION_FONT
namespace patch {
    __declspec(naked) void __cdecl exclusion_font_t::asm_func() {
        __asm {
            call    lstrlenA
            cmp     eax, 0x20
            jl      jump_ee_x8cc8e
            pop     ecx
            jmp     dword ptr [ee.x8ccc2]

            jump_ee_x8cc8e:
            jmp     dword ptr [ee.x8cc8e]
        }
    }
} // namespace patch
#endif // ifdef PATCH_SWITCH_EXCLUSION_FONT