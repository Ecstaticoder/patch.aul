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

#include "patch_theme_cc.hpp"

#ifdef PATCH_SWITCH_THEME_CC
namespace patch {
	
	int __stdcall theme_cc_t::SetBkMode_wrap3760a(void* esp, HDC hdc, int mode) {
		int ret = SetBkMode(hdc, mode);

		RECT* objrect = reinterpret_cast<RECT*>((int)esp + 0x38);
		RECT* drawrect = reinterpret_cast<RECT*>((int)esp + 0x28);
		int wid = min(disp_dialog_wid, (objrect->bottom - objrect->top - 1) >> 1);
		for (int i = 0; i < wid; i++) {
			MoveToEx(hdc, drawrect->left, drawrect->top + i, NULL);
			LineTo(hdc, drawrect->right - 1, drawrect->top + i);
			MoveToEx(hdc, drawrect->right - 1, drawrect->bottom - 1 - i, NULL);
			LineTo(hdc, drawrect->left, drawrect->bottom - 1 - i);
		}

		wid = min(disp_dialog_wid, (objrect->right - objrect->left - 1) >> 1);
		for (int i = 0; i < wid; i++) {
			if (drawrect->left <= objrect->left + i) {
				MoveToEx(hdc, objrect->left + i, objrect->bottom - 1, NULL);
				LineTo(hdc, objrect->left + i, objrect->top);
			}
			if (objrect->right - i <= drawrect->right) {
				MoveToEx(hdc, objrect->right - 1 - i, objrect->top, NULL);
				LineTo(hdc, objrect->right - 1 - i, objrect->bottom - 1);
			}
		}

		return ret;
	}

	int __stdcall theme_cc_t::SetScrollInfo_wrap38dee(HWND hwnd, int nBar, SCROLLINFO* lpsi, BOOL redraw) {
		if ((int)lpsi->nPage < horizontal_thumb_min) {
			lpsi->nMax += horizontal_thumb_min - lpsi->nPage;
			lpsi->nPage = horizontal_thumb_min;
		}
		return SetScrollInfo(hwnd, nBar, lpsi, redraw);
	}

	int __stdcall theme_cc_t::SetScrollInfo_wrap38c65(HWND hwnd, int nBar, SCROLLINFO* lpsi, BOOL redraw) {
		if ((int)lpsi->nPage < vertical_thumb_min) {
			lpsi->nMax += vertical_thumb_min - lpsi->nPage;
			lpsi->nPage = vertical_thumb_min;
		}
		return SetScrollInfo(hwnd, nBar, lpsi, redraw);
	}
	

} // namespace patch
#endif // ifdef PATCH_THEME_CC