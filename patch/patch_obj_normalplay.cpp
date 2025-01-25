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

#include "patch_obj_normalplay.hpp"


#ifdef PATCH_SWITCH_OBJ_NORMALPLAY
namespace patch {
    // スクリプトのような仕組みで呼ばれた場合、Object*にデータが存在しないことになり、右トラックが存在しない
    // その状態でcalc_trackbarを呼び出すと変な値を拾ってきたりエラーを出す恐れがある
    // efp->processingから取得したFilter*がefpと異なる場合、そのようなものの可能性が高い
    BOOL check_efp_processing(ExEdit::Filter* efp) {
        if ((int)efp->processing == 0) return FALSE;
        if (*reinterpret_cast<int*>((int)efp->exedit_fp->dll_hinst + OFS::ExEdit::ObjectAllocNum) < LOWORD(efp->processing)) return FALSE;
        if (12 <= HIWORD(efp->processing)) return FALSE;
        return efp == reinterpret_cast<ExEdit::Filter*(__cdecl*)(ExEdit::ObjectFilterIndex)>((int)efp->exedit_fp->dll_hinst + OFS::ExEdit::get_filterp)(efp->processing);
    }

    BOOL __cdecl obj_NormalPlay_t::func_proc(ExEdit::Filter* efp, ExEdit::FilterProcInfo* efpip) {
        int next_track_values[2];
        int* next_track = next_track_values;
        if (check_efp_processing(efp)) {
            int audio_speed = *reinterpret_cast<int*>((int)efpip + 416);
            int tl_nextframe, tl_nextsubframe;
            if (audio_speed == 0) {
                tl_nextframe = min(efpip->frame_num + 1, efp->frame_end_chain);
                tl_nextsubframe = 0;
            } else {
                int tl_milliframe = *reinterpret_cast<int*>((int)efpip + 412);
                int tl_nextmilli = min(tl_milliframe + audio_speed / 1000, efp->frame_end_chain * 1000);
                tl_nextframe = tl_nextmilli / 1000;
                tl_nextsubframe = (tl_nextmilli % 1000) / 10;
            }
            efp->exfunc->calc_trackbar(efp->processing, tl_nextframe, tl_nextsubframe, next_track_values, nullptr);
        } else {
            next_track = efp->track;
        }
        double volume = (double)max(0, efp->track[0]) * 4.096;
        double d_right = volume;
        double d_left = volume;
        if (efpip->audio_ch == 2) {
            int lr = std::clamp(efp->track[1], -1000, 1000); // 左右
            if (0 < lr) {
                d_right *= (double)(1000 - lr) * 0.001;
            } else if (lr < 0) {
                d_left *= (double)(lr + 1000) * 0.001;
            }
        }
        double next_volume = (double)max(0, next_track[0]) * 4.096;
        double next_right = next_volume;
        double next_left = next_volume;
        if (efpip->audio_ch == 2) {
            int lr = std::clamp(next_track[1], -1000, 1000); // 左右
            if (0 < lr) {
                next_right *= (double)(1000 - lr) * 0.001;
            } else if (lr < 0) {
                next_left *= (double)(lr + 1000) * 0.001;
            }
        }

        double step = 65536.0 / (double)efpip->audio_n;
        int step_l = (int)((next_left - d_left) * step);
        int step_r = (int)((next_right - d_right) * step);

        int right = (int)d_right;
        int left = (int)d_left;

        int volume_fr = (int)((d_right - (double)right) * 65536.0);
        int volume_fl = (int)((d_left - (double)left) * 65536.0);

        short* volume_fhr = (short*)&volume_fr + 1;
        short* volume_fhl = (short*)&volume_fl + 1;

        short* audiop = efpip->audio_p;
        short* audio_data = efpip->audio_data;
        

        if (right == 0x1000 && left == 0x1000 && 0 <= volume_fr + step_r * efpip->audio_n && volume_fr + step_r * efpip->audio_n < 0x10000 && 0 <= volume_fl + step_l * efpip->audio_n && volume_fl + step_l * efpip->audio_n < 0x10000) { // 音量100 && 左右0
            for (int i = efpip->audio_n * efpip->audio_ch; 0 < i; i--) {
                *audiop = (short)std::clamp((int)*audiop + *audio_data, SHRT_MIN, SHRT_MAX);

                audiop++;
                audio_data++;
            }
        } else if (efpip->audio_ch == 2) { // ステレオ音声
            for (int i = efpip->audio_n; 0 < i; i--) {
                *audiop = (short)std::clamp(((int)*audio_data * (right + *volume_fhr) >> 12) + *audiop, SHRT_MIN, SHRT_MAX);
                audiop++;
                audio_data++;
                volume_fr += step_r;

                *audiop = (short)std::clamp(((int)*audio_data * (left + *volume_fhl) >> 12) + *audiop, SHRT_MIN, SHRT_MAX);
                audiop++;
                audio_data++;
                volume_fl += step_l;
            }
        } else {
            for (int i = efpip->audio_n; 0 < i; i--) {
                int volume = right + *volume_fhr;
                volume_fr += step_r;
                for (int ch = efpip->audio_ch; 0 < ch; ch--) {
                    *audiop = (short)std::clamp(((int)*audio_data * volume >> 12) + *audiop, SHRT_MIN, SHRT_MAX);
                    audiop++;
                    audio_data++;
                }
            }
        }
        return TRUE;
    }
} // namespace patch
#endif // ifdef PATCH_SWITCH_OBJ_NORMALPLAY
