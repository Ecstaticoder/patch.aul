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

#include "patch_obj_scene_audio.hpp"

#ifdef PATCH_SWITCH_OBJ_SCENE_AUDIO
namespace patch {
    __declspec(naked) void __cdecl obj_SceneAudio_t::asm_func() {
        __asm {
            mov eax, dword ptr[esp + 0x34]
            mov eax, dword ptr[eax]
            mov dword ptr[esp + 0x14], eax
            mov eax, dword ptr[ecx + 0x04]
            mov ecx, dword ptr[edx]
            ret
        }
    }

} // namespace patch
#endif // ifdef PATCH_OBJ_SCENE_AUDIO