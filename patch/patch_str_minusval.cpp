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

#include "patch_str_minusval.hpp"

#ifdef PATCH_SWITCH_STR_MINUSVAL
namespace patch {
    __declspec(naked) void __cdecl str_minusval_t::asm_func() {
        __asm {
            test ecx, ecx
            jg skip1
            jl skip2
            push esi
            back1 :
                dec esi
                cmp byte ptr[esi], 0x30
            jz back1
            cmp byte ptr[esi], 0x00
            jnz skip3
            back2 :
                dec esi
                cmp byte ptr[esi], 0x30
            jz back2
            cmp byte ptr[esi], 0x2d
            jnz skip3
            pop esi
        skip2 :
            neg eax
        skip1 :
            ret
        skip3 :
            pop esi
            ret
        }


    }

} // namespace patch
#endif // ifdef PATCH_STR_MINUSVAL