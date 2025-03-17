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

#include "patch_tra_specified_speed.hpp"

#ifdef PATCH_SWITCH_TRA_SPECIFIED_SPEED
namespace patch {
	__declspec(naked) int __cdecl tra_specified_speed_t::asm_func() {
		__asm {
			imul eax, dword ptr[esp + 0x5c]
			mov ecx, dword ptr[ebp + 0x10]
			test ecx, ecx
			jz skip
			mov ecx, 0x64
			cdq
			idiv ecx
			skip :
			ret
		}
	}

} // namespace patch
#endif // ifdef PATCH_SWITCH_TRA_SPECIFIED_SPEED