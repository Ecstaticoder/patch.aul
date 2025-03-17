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

#include "patch_lua_effect.hpp"
#ifdef PATCH_SWITCH_LUA_EFFECT

namespace patch {
    __declspec(naked) void __cdecl lua_effect_t::asm_func() {
        __asm {
            mov     edi, dword ptr[ee.script_efp_ptr]
            mov     edi, dword ptr[edi]                 // *ee.script_efp_ptr
            mov     edi, dword ptr[edi + 0x000000e4]
            mov     dword ptr[ebx + 0x000000e4], edi
            mov     dword ptr[ebx + 0x000000f4], eax
            ret
        }
    }
} // namespace patch
#endif // ifdef PATCH_SWITCH_LUA_EFFECT
