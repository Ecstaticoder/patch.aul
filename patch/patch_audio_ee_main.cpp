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

#include "patch_audio_ee_main.hpp"

#ifdef PATCH_SWITCH_AUDIO_EE_MAIN
namespace patch {

	int last_disp_scene_idx = -1;
	int last_undo_id_cur = -1;
	int last_undo_id_max = -1;
	int last_tick_start_playing = -1;
	int last_tick_start_output = -1;
	int last_update_count = -1;
	BOOL exists_smem = FALSE;

	BOOL __cdecl audio_ee_main_t::audio_func_main_wrap(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, int end_layer, int add_frame, int audio_speed, int milliframe, int scene_idx, ExEdit::ObjectFilterIndex ofi) {
		int disp_scene_idx = *reinterpret_cast<int*>(GLOBAL::exedit_base + OFS::ExEdit::SceneDisplaying);
		int undo_id_cur = *reinterpret_cast<int*>(GLOBAL::exedit_base + OFS::ExEdit::undo_id_current);
		int undo_id_max = *reinterpret_cast<int*>(GLOBAL::exedit_base + OFS::ExEdit::undo_id_max);
		int tick_start_playing = *reinterpret_cast<int*>(GLOBAL::aviutl_base + OFS::AviUtl::tick_start_playing);
		int tick_start_output = *reinterpret_cast<int*>(GLOBAL::aviutl_base + OFS::AviUtl::tick_start_output);
		int update_count_val = update_count.get();

		int buf_size = fpip->audio_n * fpip->audio_ch * sizeof(*fpip->audiop);
		if (last_disp_scene_idx == disp_scene_idx && last_undo_id_cur == undo_id_cur && last_undo_id_max == undo_id_max &&
			last_update_count == update_count_val && last_tick_start_playing == tick_start_playing && last_tick_start_output == tick_start_output) {

			void* ptr = fp->exfunc->get_shared_mem((int)&audio_func_main_wrap, fpip->frame, NULL);
			if (ptr != nullptr) {
				memcpy(fpip->audiop, ptr, buf_size);
				return TRUE;
			}
		} else if (exists_smem) {
			fp->exfunc->delete_shared_mem((int)&audio_func_main_wrap, NULL);
			last_tick_start_output = -1;
			exists_smem = FALSE;
		}
		BOOL ret = reinterpret_cast<BOOL(__cdecl*)(AviUtl::FilterPlugin*, AviUtl::FilterProcInfo*, int, int, int, int, int, ExEdit::ObjectFilterIndex)>(GLOBAL::exedit_base + OFS::ExEdit::audio_func_main)(fp, fpip, end_layer, add_frame, audio_speed, milliframe, scene_idx, ofi);
		if (ret) {
			void* ptr = fp->exfunc->create_shared_mem((int)&audio_func_main_wrap, fpip->frame, buf_size, NULL);
			if (ptr != nullptr) {
				memcpy(ptr, fpip->audiop, buf_size);
				last_disp_scene_idx = disp_scene_idx;
				last_undo_id_cur = undo_id_cur;
				last_undo_id_max = undo_id_max;
				last_tick_start_playing = tick_start_playing;
				last_tick_start_output = tick_start_output;
				last_update_count = update_count_val;
				exists_smem = TRUE;
			}
		}
		return ret;
	}

} // namespace patch
#endif // ifdef PATCH_SWITCH_AUDIO_EE_MAIN
