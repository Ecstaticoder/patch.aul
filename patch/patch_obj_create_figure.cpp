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

#include "patch_obj_create_figure.hpp"

#ifdef PATCH_SWITCH_OBJ_CREATE_FIGURE
namespace patch {
    __declspec(naked) int __cdecl obj_CreateFigure_t::asm_aspect_func() {
        __asm {
            test ecx, ecx
            jnl skip
                xor ecx, ecx
            skip :
            mov eax, 0x10624dd3
            ret
        }
    }
    __declspec(naked) void __cdecl obj_CreateFigure_t::asm_maskblur_func() {
        __asm {
            dec edx
            mov dword ptr[esp + 0x48], edx
            inc edx
            lea ebx, [edx + ecx * 2]
            dec ebp
            ret
        }
    }
} // namespace patch
#endif // ifdef PATCH_OBJ_CREATE_FIGURE