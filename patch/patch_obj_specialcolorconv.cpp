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

#include "patch_obj_specialcolorconv.hpp"

#ifdef PATCH_SWITCH_OBJ_SPECIALCOLORCONV
namespace patch {
    __declspec(naked) void __cdecl obj_specialcolorconv_t::asm_func1() {
        __asm {
            movsx esi, word ptr[ecx]
            movsx ebx, word ptr[ebp]
            test esi, esi
            jg skip
                xor esi, esi
            skip :
            ret
        }
    }
    __declspec(naked) void __cdecl obj_specialcolorconv_t::asm_func2() {
        __asm {
            movsx ebx, word ptr[edi]
            movsx ebp, word ptr[esi]
            test ebx, ebx
            jg skip
                xor ebx, ebx
            skip :
            ret
        }
    }
} // namespace patch
#endif // ifdef PATCH_OBJ_SPECIALCOLORCONV