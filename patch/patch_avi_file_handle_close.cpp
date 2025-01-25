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

#include "patch_avi_file_handle_close.hpp"

#ifdef PATCH_SWITCH_AVI_FILE_HANDLE_CLOSE
namespace patch {
	void __cdecl avi_file_handle_close_t::avi_handle_close_wrap(ExEdit::Filter* efp) {
		if (!efp->check[1]) {
			reinterpret_cast<void(__cdecl*)(void*)>(GLOBAL::exedit_base + OFS::ExEdit::avi_handle_close)(efp->exdata_ptr);
		}
	}

	void __cdecl avi_file_handle_close_t::delete_all_object_wrap() {
		reinterpret_cast<void(__cdecl*)()>(GLOBAL::exedit_base + OFS::ExEdit::delete_all_object)();
		reinterpret_cast<void(__cdecl*)()>(GLOBAL::exedit_base + OFS::ExEdit::avi_handle_free)();
	}

} // namespace patch
#endif // ifdef PATCH_SWITCH_AVI_FILE_HANDLE_CLOSE

