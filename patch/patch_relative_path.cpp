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

#include "patch_relative_path.hpp"


#ifdef PATCH_SWITCH_RELATIVE_PATH_PATCH
namespace patch {

    __declspec(naked) void __cdecl patch_relative_path_t::asm_func_nulldata() {
        __asm {
            inc     dword ptr [ebp - 0x240]
            cmp     dword ptr [ebp - 0x240], 0x0C
            jl      rp_x191c
            jmp     dword ptr [rp.x1b50]

            rp_x191c:
            jmp     dword ptr [rp.x191c]
        }
    }
    __declspec(naked) void __cdecl patch_relative_path_t::asm_func_preloop() {
        __asm {
            mov     esi, dword ptr [ebp - 0x274]
            mov     dword ptr [filter_idx], 0x00000000
            ret
        }
    }
    __declspec(naked) void __cdecl patch_relative_path_t::asm_func_loopif() {
        __asm {
            inc     dword ptr [filter_idx]
            cmp     dword ptr [filter_idx], 0x0C
            jz      skip
            lea     eax, [esi + 0x0C]
            cmp     ebx, -0x1
            skip:
            ret
        }
    }
} // namespace patch
#endif // ifdef PATCH_SWITCH_RELATIVE_PATH_PATCH
