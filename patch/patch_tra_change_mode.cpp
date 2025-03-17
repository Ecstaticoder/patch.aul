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

#include "patch_tra_change_mode.hpp"

#ifdef PATCH_SWITCH_TRA_CHANGE_MODE
namespace patch {
	__declspec(naked) void __cdecl tra_change_mode_t::asm_func() {
		__asm {
			cmp     ebx, 0x05
			jz      jump_ee_x2c9a0
			test    edi, edi
			jz      jump_ee_x2c9c4
			pop     edi
			pop     esi
			pop     ebx
			ret

			jump_ee_x2c9a0:
			jmp     dword ptr [ee.x2c9a0]

			jump_ee_x2c9c4:
			jmp     dword ptr [ee.x2c9c4]
		}
	}

} // namespace patch
#endif // ifdef PATCH_SWITCH_TRA_CHANGE_MODE