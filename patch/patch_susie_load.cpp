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

#include "patch_susie_load.hpp"
#ifdef PATCH_SWITCH_SUSIE_LOAD

namespace patch {

	struct structSPI_patched {
		HMODULE hmodule;
		char information[252];
		int(PASCAL* IsSupported)(LPSTR filename, DWORD dw);
		char extension[256];
		ExEdit::SpiGetPicture GetPicture;
	};

	void __cdecl susie_load_t::LoadSpi(LPCSTR dir) {

		auto loaded_spi_array = reinterpret_cast<structSPI_patched*>(GLOBAL::exedit_base + OFS::ExEdit::loaded_spi_array);
		ZeroMemory(loaded_spi_array, sizeof(structSPI_patched) * 32);
		reinterpret_cast<BOOL(*)(LPCSTR)>(GLOBAL::exedit_base + OFS::ExEdit::MyFindFirstFile)(dir);

		auto path = *(char**)(GLOBAL::exedit_base + OFS::ExEdit::memory_ptr);
		auto buf = path + _MAX_PATH + 16;

		int i = 0;
		while (i < 32 && reinterpret_cast<BOOL(__cdecl*)(LPSTR)>(GLOBAL::exedit_base + OFS::ExEdit::MyFindNextFile)(path)) {
			auto hMod = LoadLibraryA(path);
			if (hMod == nullptr) continue;

			auto spi_GetPluginInfo = reinterpret_cast<ExEdit::SpiGetPluginInfo>(GetProcAddress(hMod, "GetPluginInfo"));
			if (spi_GetPluginInfo == nullptr) {
				FreeLibrary(hMod);
				continue;
			}

			int format[2];
			spi_GetPluginInfo(0, (LPSTR)format, 8);
			if (format[0] != 'NI00') {
				FreeLibrary(hMod);
				continue;
			}

			loaded_spi_array[i].IsSupported = reinterpret_cast<decltype(structSPI_patched::IsSupported)>(GetProcAddress(hMod, "IsSupported"));
			if (loaded_spi_array[i].IsSupported == nullptr) {
				FreeLibrary(hMod);
				continue;
			}

			loaded_spi_array[i].GetPicture = reinterpret_cast<ExEdit::SpiGetPicture>(GetProcAddress(hMod, "GetPicture"));
			if (loaded_spi_array[i].GetPicture == nullptr) {
				FreeLibrary(hMod);
				continue;
			}
			
			spi_GetPluginInfo(1, loaded_spi_array[i].information, sizeof(loaded_spi_array->information) - 1);

			loaded_spi_array[i].hmodule = hMod;

			auto ptr = buf;
			int j = 2;
			while (spi_GetPluginInfo(j, ptr, sizeof(loaded_spi_array->extension)) != 0) {
				int len = lstrlenA(ptr);
				if (0 < len) {
					ptr += len;
					*ptr = ';';
					ptr++;
					*ptr = '\0';
				}
				j += 2;
			}

			int count = 0;
			j = 0;
			while (ptr = reinterpret_cast<char*(__cdecl*)(char*, int)>(GLOBAL::exedit_base + OFS::ExEdit::get_str_semicolon_index)(buf, j), ptr != nullptr) {
				int len = lstrlenA(ptr);
				if (2 < len && len < 255 && ptr[0] == '*' && ptr[1] == '.') {
					if (255 <= count + len) {
						spi_count = i + 1;
						if (0 < count && loaded_spi_array[i].extension[count - 1] == ';') {
							loaded_spi_array[i].extension[count - 1] = '\0';
						}
						if (31 <= i) return;
						loaded_spi_array[i + 1].hmodule = loaded_spi_array[i].hmodule;
						loaded_spi_array[i + 1].GetPicture = loaded_spi_array[i].GetPicture;
						strcpy_s(loaded_spi_array[i + 1].information, sizeof(loaded_spi_array->information), loaded_spi_array[i].information);
						i++;
						count = 0;
					}
					for (int k = 0; k < len; k++) {
						loaded_spi_array[i].extension[count] = std::tolower(ptr[k]);
						count++;
					}
					loaded_spi_array[i].extension[count] = ';';
					count++;
				}
				j++;
			}
			if (0 < count && loaded_spi_array[i].extension[count - 1] == ';') {
				loaded_spi_array[i].extension[count - 1] = '\0';
			}
			i++;
			spi_count = i;
		}

	}
	void __cdecl susie_load_t::FreeSpi(){
		auto loaded_spi_array = reinterpret_cast<structSPI_patched*>(GLOBAL::exedit_base + OFS::ExEdit::loaded_spi_array);
		for (int i = 0; i < 32; i++) {
			if (loaded_spi_array[i].hmodule != nullptr) {
				FreeLibrary(loaded_spi_array[i].hmodule);
				auto hmodule = loaded_spi_array[i].hmodule;
				for (int j = i; j < 32; j++) {
					if (hmodule == loaded_spi_array[j].hmodule) {
						loaded_spi_array[j].hmodule = nullptr;
					}
				}
			}
		}
	}

	// ファイル選択ダイアログで表示する拡張子を重複しないようにする
	void __cdecl susie_load_t::set_susie_extension(char* ptr) {
		/* ptrバッファは1024byteだけど安全に使えるのは478byteまで
			根拠：
			"ImageFile (*.bmp;*.png;*.jpg;%s)\0*.bmp;*.png;*.jpg;%s\0AllFile (*.*)\0*.*"
			が1024byte以内に収まる必要があり、2カ所ある%sにptr文字列が入る
		*/
		constexpr int maxlen = 478;
		char* ptr_end = ptr + maxlen - 2;

		char* list = ptr + 512;
		const char def[] = "*.bmp\0*.png\0*.jpg";
		memcpy(list, def, sizeof(def));
		int count = 3;

		auto loaded_spi_array = reinterpret_cast<structSPI_patched*>(GLOBAL::exedit_base + OFS::ExEdit::loaded_spi_array);
		for (int i = 0; i < 32; i++) {
			if (loaded_spi_array[i].hmodule != nullptr) {
				int j = 0;
				char* ext;
				while (ext = reinterpret_cast<char*(__cdecl*)(char*, int)>(GLOBAL::exedit_base + OFS::ExEdit::get_str_semicolon_index)(loaded_spi_array[i].extension, j), ext != nullptr) {
					char* listptr = list;
					int k;
					for (k = 0; k < count; k++) {
						if (lstrcmpiA(listptr, ext) == 0) {
							break;
						}
						listptr += lstrlenA(listptr) + 1;
					}
					if (k == count) {
						int len = lstrlenA(ext);
						if (ptr + len < ptr_end) {
							lstrcpyA(ptr, ext);
							lstrcpyA(listptr, ext);
							ptr += len; *ptr = ';'; ptr++;
							count++;
						}
					}
					j++;
				}
			}
		}
		*ptr = '\0';
	}

	BOOL __cdecl susie_load_t::get_picture(ExEdit::SpiImageData* spidata, char* path) {
		if (spi_count <= 0) return FALSE;

		void* ptr = reinterpret_cast<void*>(GLOBAL::exedit_base + 0x14ea18); // exdataの整理のために使われる20,000byteのバッファ。一時領域に使える
		int readsize = reinterpret_cast<DWORD(__cdecl*)(LPCSTR, LPVOID, DWORD)>(GLOBAL::exedit_base + OFS::ExEdit::LoadFile)(path, ptr, 2048);
		if (readsize == 0) return FALSE;
		if (readsize < 2048) { // 2048byteに足りない文は0埋めが推奨されている
			memset((byte*)ptr + readsize, 0, 2048 - readsize);
		}

		auto loaded_spi_array = reinterpret_cast<structSPI_patched*>(GLOBAL::exedit_base + OFS::ExEdit::loaded_spi_array);
		HMODULE prehmod = nullptr;
		for (int i = 0; i < spi_count; i++) {
			if (prehmod != loaded_spi_array[i].hmodule && loaded_spi_array[i].hmodule != nullptr) {
				prehmod = loaded_spi_array[i].hmodule;
				if (loaded_spi_array[i].IsSupported(path, (DWORD)ptr)) {
					if (loaded_spi_array[i].GetPicture(path, 0, 0, &spidata->l_info, &spidata->l_image, 0, 0) == 0) {
						spidata->p_info = (BITMAPINFO*)LocalLock(spidata->l_info);
						spidata->p_image = LocalLock(spidata->l_image);
						return TRUE;
					}
				}
			}
		}
		return FALSE;
	}

} // namespace patch
#endif // ifdef PATCH_SWITCH_SUSIE_LOAD
