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

#include "patch_exa_camera.hpp"

#ifdef PATCH_SWITCH_EXA_CAMERA
namespace patch {
	void __stdcall exa_camera_t::get_obj_camera_flag(void* esp) {
		*reinterpret_cast<int*>((intptr_t)esp + 0x34) = 0;

		int object_idx = *reinterpret_cast<int*>((intptr_t)esp + 0x54);
		int camera;
		if (0 <= object_idx) {
			auto obj = *(ExEdit::Object**)(GLOBAL::exedit_base + OFS::ExEdit::ObjectArrayPointer);
			if (has_flag(obj[object_idx].flag, ExEdit::Object::Flag::Camera)) {
				camera = 1;
			} else {
				camera = 0;
			}
		} else {
			camera = -1;
		}
		*reinterpret_cast<int*>((intptr_t)esp + 0x38) = camera;
	}
} // namespace patch
#endif // ifdef PATCH_SWITCH_EXA_CAMERA
