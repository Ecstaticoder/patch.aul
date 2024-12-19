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

#include "patch_tra_acc_dec_movement.hpp"

#ifdef PATCH_SWITCH_TRA_ACC_DEC_MOVEMENT
namespace patch {

	void fraction_d2i(double da, double db, int* ra, int* rb) {
		if (da < db) {
			*ra = (int)(da / db * (double)INT_MAX);
			*rb = INT_MAX;
		} else {
			*ra = INT_MAX;
			*rb = (int)(db / da * (double)INT_MAX);
		}
	}
	void __stdcall tra_acc_dec_movement_t::p_wrap_66bac_66c29(int prev_idx, int len, void* esp) {
		double rate = (double)*reinterpret_cast<int*>((int)esp + 0x50) / (double)*reinterpret_cast<int*>((int)esp + 0x2c);
		double len2 = (double)len;
		if (0 <= prev_idx) {
			auto obj = *(ExEdit::Object**)(GLOBAL::exedit_base + OFS::ExEdit::ObjectArrayPointer);
			int len1 = obj[prev_idx].frame_end - obj[prev_idx].frame_begin + 1;
			if (len <= len1) {
				len2 = (double)len1 * (double)len1 / len2;
			}
		}
		double prog = ((double)len + (len2 - (double)len) * rate * 0.5) * rate;
		double total = (double)len + len2;
		fraction_d2i(prog, total, reinterpret_cast<int*>((int)esp + 0x50), reinterpret_cast<int*>((int)esp + 0x60));
	}

	void __stdcall tra_acc_dec_movement_t::n_wrap_66c43_66cb3(int next_idx, int len, void* esp) {
		double rate = (double)*reinterpret_cast<int*>((int)esp + 0x50) / (double)*reinterpret_cast<int*>((int)esp + 0x2c);
		double len2 = (double)len;
		if (0 <= next_idx) {
			auto obj = *(ExEdit::Object**)(GLOBAL::exedit_base + OFS::ExEdit::ObjectArrayPointer);
			int len1 = obj[next_idx].frame_end - obj[next_idx].frame_begin + 1;
			if (len <= len1) {
				len2 = (double)len1 * (double)len1 / len2;
			}
		}
		double total = (double)len + len2;
		double prog = total * 0.5 + (len2 + ((double)len - len2) * rate * 0.5) * rate;
		fraction_d2i(prog, total, reinterpret_cast<int*>((int)esp + 0x50), reinterpret_cast<int*>((int)esp + 0x60));
	}

} // namespace patch
#endif // ifdef PATCH_SWITCH_TRA_ACC_DEC_MOVEMENT