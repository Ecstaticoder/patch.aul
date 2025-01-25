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

#include "patch_midpt_delete.hpp"


#ifdef PATCH_SWITCH_MIDPT_DELETE
namespace patch {
    void __cdecl midopt_delete_t::delete_object_wrap(int right_idx) {
        auto dialog_idx = *reinterpret_cast<int*>(GLOBAL::exedit_base + OFS::ExEdit::SettingDialog_ObjIdx);
        auto obj = *reinterpret_cast<ExEdit::Object**>(GLOBAL::exedit_base + OFS::ExEdit::ObjectArrayPointer);
        int right_leader_idx = right_idx;
        if (0 <= obj[right_leader_idx].index_midpt_leader) {
            right_leader_idx = obj[right_leader_idx].index_midpt_leader;
        }
        int dialog_leader_idx = dialog_idx;
        if (0 <= dialog_leader_idx && 0 <= obj[dialog_leader_idx].index_midpt_leader) {
            dialog_leader_idx = obj[dialog_leader_idx].index_midpt_leader;
        }

        reinterpret_cast<void(__cdecl*)(int)>(GLOBAL::exedit_base + OFS::ExEdit::delete_object)(right_idx);

        if (right_leader_idx == dialog_leader_idx) {
            // fast_setting_dialogにより変わったアドレスの方を取得して実行
            int update_dialog_by_3957d = GLOBAL::exedit_base + 0x3957d + 4 + *reinterpret_cast<int*>(GLOBAL::exedit_base + 0x3957d);
            reinterpret_cast<BOOL(__cdecl*)(int)>(update_dialog_by_3957d)(dialog_idx);
        }
    }
} // namespace patch
#endif // ifdef PATCH_SWITCH_MIDPT_DELETE
