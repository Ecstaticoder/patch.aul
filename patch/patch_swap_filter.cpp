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

#include "patch_swap_filter.hpp"

#ifdef PATCH_SWITCH_SWAP_FILTER
namespace patch {

    void __stdcall swap_filter_t::prev_swap(int object_idx, int filter_idx, int filter_ofs) {
        int filter_idx2 = filter_idx + filter_ofs;
        if (filter_idx > filter_idx2) {
            std::swap(filter_idx, filter_idx2);
        }
        if (filter_idx != 0) {
            return;
        }
        auto obj = *reinterpret_cast<ExEdit::Object**>(GLOBAL::exedit_base + OFS::ExEdit::ObjectArrayPointer);
        obj += object_idx;
        auto LoadedFilterTable = reinterpret_cast<ExEdit::Filter**>(GLOBAL::exedit_base + OFS::ExEdit::LoadedFilterTable);
        auto efAnimationEffect = reinterpret_cast<ExEdit::Filter*>(GLOBAL::exedit_base + OFS::ExEdit::efAnimationEffect_ptr);
        if (LoadedFilterTable[obj->filter_param[filter_idx2].id] == efAnimationEffect) {
            efAnimationEffect->exdata_ptr = reinterpret_cast<void*(__cdecl*)(ExEdit::ObjectFilterIndex)>(GLOBAL::exedit_base + OFS::ExEdit::get_exdata_ptr)(static_cast<ExEdit::ObjectFilterIndex>((filter_idx2 << 16) | (object_idx + 1)));
            efAnimationEffect->processing = static_cast<ExEdit::ObjectFilterIndex>(object_idx + 1); // filter_idx部分は0でなければ表示名が書き換えられない
            reinterpret_cast<void(__cdecl*)(ExEdit::Filter*, void*)>(GLOBAL::exedit_base + 0x4150)(efAnimationEffect, efAnimationEffect->exdata_ptr);
        } else if (LoadedFilterTable[obj->filter_param[filter_idx].id] == efAnimationEffect) {
            efAnimationEffect->exfunc->rename_object(static_cast<ExEdit::ObjectFilterIndex>(object_idx + 1), const_cast<char*>("\0"));
        }
    }

} // namespace patch
#endif // ifdef PATCH_SWAP_FILTER