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

#include "patch_page_size_alloc.hpp"

#ifdef PATCH_SWITCH_PAGE_SIZE_ALLOC
namespace patch {
    
    __declspec(naked) void __cdecl page_size_alloc_t::asm_func() {
        __asm {
            and eax, 0xfffff000
            cmp eax, 0x00040000
            ret
        }
    }
} // namespace patch
#endif // ifdef PATCH_PAGE_SIZE_ALLOC