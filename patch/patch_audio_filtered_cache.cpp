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

#include "patch_audio_filtered_cache.hpp"


#ifdef PATCH_SWITCH_AUDIO_FILTERED_CACHE
namespace patch {

	struct cache_info {
		int audio_n_in;
		int audio_ch_in;
		int audio_n_out;
		int audio_ch_out;
		byte data[1];
	};


	struct {
		int last_disp_scene_idx = -1;
		int last_undo_id_cur = -1;
		int last_undo_id_max = -1;
	}ee;

	int last_update_count = -1;

	int last_undo_frame_len = -1;
	int last_undo_frame_n = -1;
	int last_tick_start_playing = -1;
	int last_tick_start_output = -1;
	AviUtl::FilterPlugin* last_filter_order[128];

	BOOL exists_smem = FALSE;

	BOOL __fastcall audio_filtered_cache_t::audio_auf_func_proc_wrap(AviUtl::FilterProcInfo* fpip, char* profile, AviUtl::FilterPlugin* exit_fp) {

		if (exit_fp != nullptr && has_flag(exit_fp->flag, AviUtl::FilterPlugin::Flag::AudioFilter)) {
			return fastcall_caller((void*)(GLOBAL::aviutl_base + OFS::AviUtl::audio_auf_func_proc), 3, (intptr_t)fpip, (intptr_t)profile, (intptr_t)exit_fp);
		}

		int disp_scene_idx, undo_id_cur, undo_id_max;
		if (GLOBAL::exedit_base != NULL) {
			disp_scene_idx = *reinterpret_cast<int*>(GLOBAL::exedit_base + OFS::ExEdit::SceneDisplaying);
			undo_id_cur = *reinterpret_cast<int*>(GLOBAL::exedit_base + OFS::ExEdit::undo_id_current);
			undo_id_max = *reinterpret_cast<int*>(GLOBAL::exedit_base + OFS::ExEdit::undo_id_max);
		} else {
			disp_scene_idx = undo_id_cur = undo_id_max = 0;
		}
		int update_count_val = update_count.get();

		int undo_frame_len = *reinterpret_cast<int*>(GLOBAL::aviutl_base + OFS::AviUtl::undo_frame_len);
		int undo_frame_n = *reinterpret_cast<int*>(GLOBAL::aviutl_base + OFS::AviUtl::undo_frame_n);
		int tick_start_playing = *reinterpret_cast<int*>(GLOBAL::aviutl_base + OFS::AviUtl::tick_start_playing);
		int tick_start_output = *reinterpret_cast<int*>(GLOBAL::aviutl_base + OFS::AviUtl::tick_start_output);

		auto filter_order = reinterpret_cast<AviUtl::FilterPlugin*>(GLOBAL::aviutl_base + OFS::AviUtl::SortedFilterTable);

		int audio_n_in = fpip->audio_n;
		int audio_ch_in = fpip->audio_ch;

		auto a_exfunc = (AviUtl::ExFunc*)(GLOBAL::aviutl_base + OFS::AviUtl::exfunc);
		if (exists_smem) {
			if (ee.last_disp_scene_idx == disp_scene_idx && ee.last_undo_id_cur == undo_id_cur && ee.last_undo_id_max == undo_id_max &&
				last_undo_frame_len == undo_frame_len && last_undo_frame_n == undo_frame_n &&
				last_update_count == update_count_val && last_tick_start_playing == tick_start_playing && last_tick_start_output == tick_start_output &&
				memcmp(last_filter_order, filter_order, sizeof(last_filter_order)) == 0) {

				cache_info* ptr = (cache_info*)a_exfunc->get_shared_mem((int)&audio_auf_func_proc_wrap, fpip->frame, NULL);
				if (ptr != nullptr) {
					if (ptr->audio_n_in == audio_n_in && ptr->audio_ch_in == audio_ch_in) {
						fpip->audio_n = ptr->audio_n_out;
						fpip->audio_ch = ptr->audio_ch_out;
						int buf_size = fpip->audio_n * fpip->audio_ch * sizeof(*fpip->audiop);
						memcpy(fpip->audiop, ptr->data, buf_size);
						return TRUE;
					}
					a_exfunc->delete_shared_mem((int)&audio_auf_func_proc_wrap, NULL);
					last_tick_start_output = -1;
					exists_smem = FALSE;
				}
			} else {
				a_exfunc->delete_shared_mem((int)&audio_auf_func_proc_wrap, NULL);
				last_tick_start_output = -1;
				exists_smem = FALSE;
			}
		}
		
		BOOL ret = fastcall_caller((void*)(GLOBAL::aviutl_base + OFS::AviUtl::audio_auf_func_proc), 3, (intptr_t)fpip, (intptr_t)profile, (intptr_t)exit_fp);

		if (ret) {
			int buf_size = sizeof(cache_info) + fpip->audio_n * fpip->audio_ch * sizeof(*fpip->audiop) + 16;
			cache_info* ptr = (cache_info*)a_exfunc->create_shared_mem((int)&audio_auf_func_proc_wrap, fpip->frame, buf_size, NULL);
			if (ptr != nullptr) {
				ptr->audio_n_in = audio_n_in;
				ptr->audio_ch_in = audio_ch_in;
				ptr->audio_n_out = fpip->audio_n;
				ptr->audio_ch_out = fpip->audio_ch;
				memcpy(ptr->data, fpip->audiop, buf_size);
				ee.last_disp_scene_idx = disp_scene_idx;
				ee.last_undo_id_cur = undo_id_cur;
				ee.last_undo_id_max = undo_id_max;
				last_update_count = update_count_val;
				last_undo_frame_len = undo_frame_len;
				last_undo_frame_n = undo_frame_n;
				last_tick_start_playing = tick_start_playing;
				last_tick_start_output = tick_start_output;
				memcpy(last_filter_order, filter_order, sizeof(last_filter_order));
				exists_smem = TRUE;
			}
		}
		return ret;
	}

} // namespace patch
#endif // ifdef PATCH_SWITCH_AUDIO_FILTERED_CACHE
