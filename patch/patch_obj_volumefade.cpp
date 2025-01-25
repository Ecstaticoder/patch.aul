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

#include "patch_obj_volumefade.hpp"


#ifdef PATCH_SWITCH_OBJ_VOLUMEFADE
namespace patch {
    BOOL __cdecl obj_VolumeFade_t::func_proc(ExEdit::Filter* efp, ExEdit::FilterProcInfo* efpip) {
        if (efp->track[0] <= 0 && efp->track[1] <= 0) {
            return TRUE;
        }
        int milliframe = *reinterpret_cast<int*>((int)efpip + 412);
        int audio_speed = *reinterpret_cast<int*>((int)efpip + 416);
        if (audio_speed == 0) {
            audio_speed = 1000000;
            milliframe = efpip->frame_num * 1000;
        }
        if (milliframe < efp->frame_start_chain * 1000 || efp->frame_end_chain * 1000 < milliframe) {
            memset(efpip->audio_data, 0, efpip->audio_ch * efpip->audio_n * sizeof(*efpip->audio_data));
            return TRUE;
        }
        int track_in = max(0, efp->track[0]);
        int track_out = max(0, efp->track[1]);
        int audio_rate = *reinterpret_cast<int*>((int)efpip + 268);
        int64_t sample_in = (int64_t)audio_rate * track_in * 10000 / audio_speed;
        int64_t sample_out = (int64_t)audio_rate * track_out  * 10000 / audio_speed;
        int64_t sample_cur = (int64_t)efpip->audio_n * (milliframe - efp->frame_start_chain * 1000) * 1000 / audio_speed;
        int64_t sample_n = (int64_t)efpip->audio_n * (efp->frame_end_chain + 1 - efp->frame_start_chain) * 1000000 / audio_speed;
        if (sample_in < sample_cur && sample_cur + efpip->audio_n < sample_n - sample_out) {
            return TRUE;
        }

        int64_t in_e, out_s, out_e;
        if (sample_n < sample_in + sample_out && 0 < sample_in && 0 < sample_out) {
            in_e = sample_n * sample_in / (sample_in + sample_out);
            out_s = in_e;
        } else {
            in_e = sample_in;
            out_s = sample_n - sample_out;
        }
        in_e = min(in_e, sample_cur + efpip->audio_n);
        out_s = max(out_s, sample_cur);
        out_e = min(sample_cur + efpip->audio_n, sample_n);

        if (sample_cur < in_e) {
            double step_d = 16384.0 * 65536.0 / (double)sample_in;
            int step = (int)step_d;
            int rate30 = (int)(step_d * (double)sample_cur);
            short* rate_hi = (short*)&rate30 + 1;

            short* audio_data = efpip->audio_data;
            for (int i = in_e - sample_cur; 0 < i; i--) {
                for (int ch = 0; ch < efpip->audio_ch; ch++) {
                    *audio_data = (short)(*audio_data * *rate_hi >> 14);
                    audio_data++;
                }
                rate30 += step;
            }
        }

        if (out_s < out_e) {
            double step_d = 16384.0 * 65536.0 / (double)sample_out;
            int step = (int)step_d;
            int rate30 = 16384 * 65536 - (int)(step_d * (double)(out_s - sample_n + sample_out));
            short* rate_hi = (short*)&rate30 + 1;
            short* audio_data = efpip->audio_data + efpip->audio_ch * (out_s - sample_cur);
            for (int i = out_e - out_s; 0 < i; i--) {
                for (int ch = 0; ch < efpip->audio_ch; ch++) {
                    *audio_data = (short)(*audio_data * *rate_hi >> 14);
                    audio_data++;
                }
                rate30 -= step;
            }
        }
        if (out_e < sample_cur + efpip->audio_n) {
            memset(efpip->audio_data + efpip->audio_ch * (out_e - sample_cur), 0, efpip->audio_ch * sizeof(*efpip->audio_data) * (sample_cur + efpip->audio_n - out_e));
        }
        return TRUE;
    }
} // namespace patch
#endif // ifdef PATCH_SWITCH_OBJ_VOLUMEFADE
