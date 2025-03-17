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

#include "patch_obj_imageloop.hpp"

#ifdef PATCH_SWITCH_OBJ_IMAGELOOP
namespace patch {


	__declspec(naked) void __cdecl obj_ImageLoop_t::asm_func_check_size() {
		__asm {
			mov     ecx, dword ptr [esp + 0x08]
			xor     eax, eax
			add     ecx, 0x70
			cmp     dword ptr [ecx + 0x44], eax
			jng     skip
			cmp     dword ptr [ecx + 0x48], eax
			jng     skip
			sub     esp, 0x0000008c
			jmp     dword ptr [ee.x5a576]

    		skip:
			inc     eax
			ret
		}
	}

	// return 0;
	int __stdcall obj_ImageLoop_t::save_current_image(void* esp) {
		auto efp = *reinterpret_cast<ExEdit::Filter**>((int)esp + 0xa0);
		auto efpip = *reinterpret_cast<ExEdit::FilterProcInfo**>((int)esp + 0xa4);

		
		int obj_h = efpip->obj_h;
		int smemline = efpip->obj_w * 8;
		efp->aviutl_exfunc->delete_shared_mem((int)&save_current_image + ExEdit::filter(efp->processing), NULL);
		
		int* smem = (int*)efp->aviutl_exfunc->create_shared_mem((int)&save_current_image + ExEdit::filter(efp->processing), (int)efp->processing, efpip->obj_h * smemline + 16, NULL);
		if (smem == NULL) return 0;

		int editline = efpip->obj_line * 8;
		void* edit = efpip->obj_edit;
		
		smem[0] = obj_h;
		smem[1] = smemline;
		smem = (int*)((int)smem + 16);

		for (int y = 0; y < efpip->obj_h; y++) {
			memcpy(smem, edit, smemline);
			smem = (int*)((int)smem + smemline);
			edit = (int*)((int)edit + editline);
		}

		return 0;
	}


	void __cdecl obj_ImageLoop_t::do_after_filter_effect_wrap(ExEdit::ObjectFilterIndex ofi, ExEdit::FilterProcInfo* efpip, int flag) {
		auto a_exfunc = (AviUtl::ExFunc*)(GLOBAL::aviutl_base + OFS::AviUtl::exfunc);

		int* smem = (int*)a_exfunc->get_shared_mem((int)&save_current_image + ExEdit::filter(ofi), (int)ofi, NULL);
		if (smem == NULL) {
			return;
		}
		int obj_h = smem[0];
		int smemline = smem[1];
		smem = (int*)((int)smem + 16);
		int editline = efpip->obj_line * 8;
		void* edit = efpip->obj_edit;

		for (int y = 0; y < obj_h; y++) {
			memcpy(edit, smem, smemline);
			edit = (void*)((int)edit + editline);
			smem = (int*)((int)smem + smemline);
		}

		reinterpret_cast<void(__cdecl*)(ExEdit::ObjectFilterIndex, ExEdit::FilterProcInfo*, int)>(GLOBAL::exedit_base + OFS::ExEdit::do_after_filter_effect)(ofi, efpip, flag);

		
	}



} // namespace patch
#endif // ifdef PATCH_SWITCH_OBJ_IMAGELOOP
