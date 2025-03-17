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

#include "patch_audio_layer_end.hpp"


#ifdef PATCH_SWITCH_AUDIO_LAYER_END
namespace patch {

    __declspec(naked) void __cdecl audio_layer_end_t::asm_func() {
        __asm {
            jnz     jmp_ee_4a008
            mov     ecx, dword ptr [esp + 0x00000780]
            cmp     ecx, dword ptr [edi + 0x000005c0]
            jng     jmp_ee_4a008
            jmp     dword ptr [ee.x49ebc]

            jmp_ee_4a008:
            jmp     dword ptr [ee.x4a008]
        }
    }

} // namespace patch
#endif // ifdef PATCH_SWITCH_AUDIO_LAYER_END
