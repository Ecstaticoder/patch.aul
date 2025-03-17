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

#include "patch_exo_midpt_and_tra.hpp"


#ifdef PATCH_SWITCH_EXO_MIDPT_AND_TRA
namespace patch {

    __declspec(naked) void __cdecl exo_midpt_and_tra_t::asm_func() {
        __asm {
            mov     eax, dword ptr [esp + 0x140]
            test    eax, eax
            jz      jump_ee_x349b0
            ret

            jump_ee_x349b0:
            jmp     dword ptr [ee.x349b0]
        }
    }
} // namespace patch
#endif // ifdef PATCH_SWITCH_EXO_MIDPT_AND_TRA
