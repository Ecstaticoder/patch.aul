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

#include "patch_yc_rgb_conv.hpp"
#include "simd.hpp"

#ifdef PATCH_SWITCH_YC_RGB_CONV
namespace patch {

    void __cdecl yc_rgb_conv_t::do_multi_thread_func_wrap(AviUtl::MultiThreadFunc func,BOOL flag) {
        if (*reinterpret_cast<int*>(GLOBAL::exedit_base + OFS::ExEdit::yc_conv_w_loop_count) <= 0) {
            return;
        }
        reinterpret_cast<void(__cdecl*)(AviUtl::MultiThreadFunc, BOOL)>(GLOBAL::exedit_base + OFS::ExEdit::do_multi_thread_func)(func, flag);
    }

    // 処理時間が元から短いのでAVX2等の実装は容量の無駄
    void __cdecl yc_rgb_conv_t::bgra2yca_mt(int thread_id, int thread_num, void* n1, void* n2) {
        int w = *reinterpret_cast<int*>(GLOBAL::exedit_base + 0x1e4300);
        int h = *reinterpret_cast<int*>(GLOBAL::exedit_base + 0x1e4328);
        auto dst0 = *reinterpret_cast<ExEdit::PixelYCA**>(GLOBAL::exedit_base + 0x1e42f4);
        auto src0 = *reinterpret_cast<ExEdit::PixelBGRA**>(GLOBAL::exedit_base + 0x1e42d0);
        int dst_linesize = *reinterpret_cast<int*>(GLOBAL::exedit_base + 0x1e4330);
        int src_linesize = *reinterpret_cast<int*>(GLOBAL::exedit_base + 0x1e4304);

        __m128i add_bgra128 = _mm_setr_epi16(18, 18, 18, 1, 18, 18, 18, 1);
        __m128i b2yc128 = _mm_setr_epi16(1875, 8224, -1337, 0, 1875, 8224, -1337, 0);
        __m128i g2yc128 = _mm_setr_epi16(9655, -5449, -6887, 0, 9655, -5449, -6887, 0);
        __m128i r2yc128 = _mm_setr_epi16(4918, -2775, 8224, 16448, 4918, -2775, 8224, 16448);
        __m128i add_yca128 = _mm_setr_epi16(-3, 1, 1, 0, -3, 1, 1, 0);
        __m128i zero128 = _mm_setzero_si128();
        __m128i bgra128;
        int y = h * thread_id / thread_num;
        dst0 = reinterpret_cast<decltype(dst0)>((int)dst0 + dst_linesize * y);
        src0 = reinterpret_cast<decltype(src0)>((int)src0 + src_linesize * y);
        for (y = h * (thread_id + 1) / thread_num - y; 0 < y; y--) {
            dst0 = reinterpret_cast<decltype(dst0)>((int)dst0 + dst_linesize);
            auto dst = dst0;
            auto src = src0;

            for (int x = w >> 1; 0 < x; x--) {
                bgra128.m128i_i32[0] = *(int32_t*)src;
                src++;
                bgra128.m128i_i32[1] = *(int32_t*)src;
                src++;
                bgra128 = _mm_unpacklo_epi8(bgra128, zero128);
                bgra128 = _mm_adds_epi16(_mm_slli_epi16(bgra128, 6), add_bgra128);
                __m128i b128 = _mm_shuffle_epi16(bgra128, 0);
                __m128i g128 = _mm_shuffle_epi16(bgra128, 0b01010101);
                __m128i ra128 = _mm_shuffle_epi16(bgra128, 0b11101010);

                b128 = _mm_mulhi_epi16(b128, b2yc128);
                g128 = _mm_mulhi_epi16(g128, g2yc128);
                ra128 = _mm_mulhi_epi16(ra128, r2yc128);
                __m128i yca128 = _mm_adds_epi16(_mm_adds_epi16(b128, g128), _mm_adds_epi16(ra128, add_yca128));
                _mm_store_si128((__m128i*)dst, yca128);
                dst += 2;
            }
            if (w & 1) {
                dst->y = (src->b * 1875 + src->g * 9655 + src->r * 4918 + 1554) >> 10;
                dst->cb = ((src->b * 8224 + src->g * -5449 + src->r * -2775) >> 10) + 1;
                dst->cr = ((src->b * -1337 + src->g * -6887 + src->r * 8224) >> 10) + 1;
                dst->a = (src->a * 16448 + 257) >> 10;
            }
            src0 = reinterpret_cast<decltype(src0)>((int)src0 + src_linesize);
        }
    }
    void __cdecl yc_rgb_conv_t::bgra2yca_u_mt(int thread_id, int thread_num, void* n1, void* n2) {
        int w = *reinterpret_cast<int*>(GLOBAL::exedit_base + 0x1e4300);
        int h = *reinterpret_cast<int*>(GLOBAL::exedit_base + 0x1e4328);
        auto dst0 = *reinterpret_cast<ExEdit::PixelYCA**>(GLOBAL::exedit_base + 0x1e42f4);
        auto src0 = *reinterpret_cast<ExEdit::PixelBGRA**>(GLOBAL::exedit_base + 0x1e42d0);
        int dst_linesize = *reinterpret_cast<int*>(GLOBAL::exedit_base + 0x1e4330);
        int src_linesize = *reinterpret_cast<int*>(GLOBAL::exedit_base + 0x1e4304);

        __m128i add_bgra128 = _mm_setr_epi16(18, 18, 18, 1, 18, 18, 18, 1);
        __m128i b2yc128 = _mm_setr_epi16(1875, 8224, -1337, 0, 1875, 8224, -1337, 0);
        __m128i g2yc128 = _mm_setr_epi16(9655, -5449, -6887, 0, 9655, -5449, -6887, 0);
        __m128i r2yc128 = _mm_setr_epi16(4918, -2775, 8224, 16448, 4918, -2775, 8224, 16448);
        __m128i add_yca128 = _mm_setr_epi16(-3, 1, 1, 0, -3, 1, 1, 0);
        __m128i zero128 = _mm_setzero_si128();
        __m128i bgra128;
        int y = h * thread_id / thread_num;
        dst0 = reinterpret_cast<decltype(dst0)>((int)dst0 + dst_linesize * y);
        src0 = reinterpret_cast<decltype(src0)>((int)src0 + src_linesize * y);
        for (y = h * (thread_id + 1) / thread_num - y; 0 < y; y--) {
            dst0 = reinterpret_cast<decltype(dst0)>((int)dst0 + dst_linesize);
            auto dst = dst0;
            auto src = src0;

            for (int x = w >> 1; 0 < x; x--) {
                bgra128.m128i_i32[0] = *(int32_t*)src;
                src++;
                bgra128.m128i_i32[1] = *(int32_t*)src;
                src++;
                bgra128 = _mm_unpacklo_epi8(bgra128, zero128);
                bgra128 = _mm_adds_epi16(_mm_slli_epi16(bgra128, 6), add_bgra128);
                __m128i b128 = _mm_shuffle_epi16(bgra128, 0);
                __m128i g128 = _mm_shuffle_epi16(bgra128, 0b01010101);
                __m128i ra128 = _mm_shuffle_epi16(bgra128, 0b11101010);

                b128 = _mm_mulhi_epi16(b128, b2yc128);
                g128 = _mm_mulhi_epi16(g128, g2yc128);
                ra128 = _mm_mulhi_epi16(ra128, r2yc128);
                __m128i yca128 = _mm_adds_epi16(_mm_adds_epi16(b128, g128), _mm_adds_epi16(ra128, add_yca128));
                _mm_storeu_si128((__m128i*)dst, yca128);
                dst += 2;
            }
            if (w & 1) {
                dst->y = (src->b * 1875 + src->g * 9655 + src->r * 4918 + 1554) >> 10;
                dst->cb = ((src->b * 8224 + src->g * -5449 + src->r * -2775) >> 10) + 1;
                dst->cr = ((src->b * -1337 + src->g * -6887 + src->r * 8224) >> 10) + 1;
                dst->a = (src->a * 16448 + 257) >> 10;
            }
            src0 = reinterpret_cast<decltype(src0)>((int)src0 + src_linesize);
        }
    }
} // namespace patch
#endif // ifdef PATCH_SWITCH_YC_RGB_CONV
