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

#pragma once
#include "macro.h"

#ifdef PATCH_SWITCH_PLAYBACK_SPEED

#include <exedit.hpp>
#include "config_rw.hpp"
#include "util.hpp"

#include "global.hpp"

namespace patch {

    // init at exedit load
    // n番目の中間点で再生速度を変化させるとnフレーム遅れて反映されるのを修正
    // 中間点の途中で再生速度トラックバーを動かした時にオブジェクトの長さがおかしくなるのを修正
    // 中間点を動かした後に再生速度トラックバーを動かした時にオブジェクトの長さがおかしくなることがあるのを修正
    // オブジェクトの長さを変えた後に元に戻すをして再生速度トラックバーを動かした時にオブジェクトの長さがおかしくなることがあるのを修正
    inline class playback_speed_t {
        bool enabled = true;
        bool enabled_i;
        inline static const char key[] = "playback_speed";


        inline static struct _ofs {
            int32_t x177a10 = 0x177a10; // SettingDialog_ObjIdx
            int32_t xd7368 = 0xd7368;
            int32_t x24de58 = 0x24de58;
            int32_t x230980 = 0x230980;
            int32_t x2309e0 = 0x2309e0;
            int32_t x6900 = 0x6900;
            int32_t x618c = 0x618c;
            int32_t x902d0 = 0x902d0;
            int32_t x8fbf8 = 0x8fbf8;
            int32_t x83770 = 0x83770;
            int32_t x83cc0 = 0x83cc0;
            int32_t x83797 = 0x83797;
            int32_t x84332 = 0x84332;
            int32_t x848d0 = 0x848d0;
            int32_t x84359 = 0x84359;
        }ee;
        static void __cdecl asm_func();
        // add_base(GLOBAL::exedit_base, &ee, sizeof(ee));
        static void __cdecl asm_func_calc_length_if();
        static void __cdecl asm_func_calc_length_movie_file();
        static void __cdecl asm_func_calc_length_audio_file();
        static void __cdecl asm_func_calc_length_scene();
        static void __cdecl asm_func_calc_length_scene_audio();
        static void __cdecl asm_func_wndproc_undo_movie_file();
        static void __cdecl asm_func_wndproc_undo_audio_file();
        static void __cdecl asm_func_wndproc_undo_scene();
        static void __cdecl asm_func_wndproc_undo_scene_audio();

    public:
        void init() {
            enabled_i = enabled;

            if (!enabled_i)return;

            add_base(GLOBAL::exedit_base, &ee, sizeof(ee));

            { // n番目の中間点で再生速度を変化させるとnフレーム遅れて反映されるのを修正
                { // movie_file
                    OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x005fd9, 1);
                    h.store_i8(0, '\x90');
                }
                { // audio_file
                    OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x08faab, 1);
                    h.store_i8(0, '\x90');
                }

                { // scene
                    OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x0836b3, 1);
                    h.store_i8(0, '\x90');
                }
                { // scene_audio
                    OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x084297, 1);
                    h.store_i8(0, '\x90');
                }
            }

            { // 中間点の途中で再生速度トラックバーを動かした時にオブジェクトの長さがおかしくなるのを修正
                /*
                    length = efp->frame_start - efp->frame_start_chain - objinfo.frame_begin + objinfo.frame_end;
                    ↓
                    length = 0 - objinfo.frame_begin + objinfo.frame_end;
                */
                { // audio_file
                    OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x006973, 2);
                    h.store_i16(0, '\x33\xc0');
                }
                { // audio_file
                    OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x090343, 2);
                    h.store_i16(0, '\x33\xc0');
                }

                { // scene
                    OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x083d11, 2);
                    h.store_i16(0, '\x33\xc0');
                }
                { // scene_audio
                    OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x084921, 2);
                    h.store_i16(0, '\x33\xc0');
                }
            }
            { // 中間点を動かした後に再生速度トラックバーを動かした時にオブジェクトの長さがおかしくなるのを修正
                int addr[4] = { 0x06900, 0x902d0, 0x83cc0, 0x848d0 };
                void* asm_func_addr[4] = { &asm_func_calc_length_movie_file, &asm_func_calc_length_audio_file, &asm_func_calc_length_scene, &asm_func_calc_length_scene_audio };
                byte espsub[4] = { 0x6c, 0x6c, 0x30, 0x30 };

                for (int i = 0; i < 4; i++) {
                    OverWriteOnProtectHelper h(GLOBAL::exedit_base + addr[i], 13);
                    h.store_i8(0, '\xe8');
                    h.replaceNearJmp(1, &asm_func_calc_length_if);
                    h.store_i8(5, '\xe8');
                    h.replaceNearJmp(6, asm_func_addr[i]);
                    h.store_i16(10, '\x83\xec');
                    h.store_i8(12, espsub[i]);
                }
                { // movie_file
                    /*
                        10006900 83ec6c               sub     esp,+6c
                        10006903 c70568730d1000000000 mov     dword ptr [100d7368],00000000
                        ↓
                        10006900 e8XxXxXxXx           call    asm_func_calc_length_if
                        10006905 e8XxXxXxXx           call    asm_func_calc_length
                        1000690a 83ec6c               sub     esp,+6c

                        10006900:
                        if(LOWORD(efp->processing) - 1 != *reinterpret_cast<int*>(GLOBAL::exedit_base + OFS::ExEdit::SettingDialog_ObjIdx)){
                            return;
                        }
                        1000690a:
                    */
                }
                { // audio_file
                    /*
                        100902d0 83ec6c               sub     esp,+6c
                        100902d3 c70558de241000000000 mov     dword ptr [1024de58],00000000
                        ↓
                        100902d0 e8XxXxXxXx           call    asm_func_calc_length_if
                        100902d5 e8XxXxXxXx           call    asm_func_calc_length
                        100902da 83ec6c               sub     esp,+6c
                    */
                }
                { // scene
                    /*
                        10083cc0 83ec30               sub     esp,+30
                        10083cc3 c7058009231000000000 mov     dword ptr [10230980],00000000
                        ↓
                        10083cc0 e8XxXxXxXx           call    asm_func_calc_length_if
                        10083cc5 e8XxXxXxXx           call    asm_func_calc_length
                        10083cca 83ec30               sub     esp,+30
                    */
                }
                { // scene_audio
                    /*
                        100848d0 83ec30               sub     esp,+30
                        100848d3 c705e009231000000000 mov     dword ptr [102309e0],00000000
                        ↓
                        100848d0 e8XxXxXxXx           call    asm_func_calc_length_if
                        100848d5 e8XxXxXxXx           call    asm_func_calc_length
                        100848da 83ec30               sub     esp,+30
                    */
                }
            }
            { // オブジェクトの長さを変えて元に戻して再生速度を変えるとオブジェクトの長さがおかしくなるのを修正

                /* movie_file
                    100060d3 0f87b3000000       ja      1000618c
                    ↓
                    100060d3 0f87XxXxXxXx       ja      cursor

                    00000000 83f812             cmp     eax,+12
                    00000000 750a               jnz     skip,+0a
                    00000000 57                 push    edi
                    00000000 56                 push    esi
                    00000000 e8XxXxXxXx         call    ee+06900
                    00000000 83c408             add     esp,+08
                    00000000 e9XxXxXxXx         jmp     ee+0618c
                */
                // movie_file
                ReplaceNearJmp(GLOBAL::exedit_base + 0x060d5, &asm_func_wndproc_undo_movie_file);

                // movie_file
                ReplaceNearJmp(GLOBAL::exedit_base + 0x8fb3b, &asm_func_wndproc_undo_audio_file);

                { // scene
                    /*
                        1008376b 83f80f             cmp     eax,+0f
                        1008376e 7727               ja      10083797
                        ↓
                        1008376b e9XxXxXxXx         jmp     cursor

                        00000000 83f80f             cmp     eax,+0f
                        00000000 0f86XxXxXxXx       jna     ee+83770
                        00000000 83f812             cmp     eax,+12
                        00000000 750a               jnz     skip,+09
                        00000000 56                 push    esi
                        00000000 e8XxXxXxXx         call    ee+83cc0
                        00000000 83c404             add     esp,+04
                        00000000 e9XxXxXxXx         jmp     ee+83797
                    */
                    OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x8376b, 5);
                    h.store_i8(0, '\xe9');
                    h.replaceNearJmp(1, &asm_func_wndproc_undo_scene);
                }
                { // scene_audio
                    /*
                        1008432d 83ff0f             cmp     edi,+0f
                        10084330 7727               ja      10084359
                        ↓
                        1008432d e9XxXxXxXx         jmp     cursor

                        00000000 83ff0f             cmp     edi,+0f
                        00000000 0f86XxXxXxXx       jna     ee+84332
                        00000000 83ff12             cmp     edi,+12
                        00000000 750a               jnz     skip,+09
                        00000000 56                 push    esi
                        00000000 e8XxXxXxXx         call    ee+848d0
                        00000000 83c404             add     esp,+04
                        00000000 e9XxXxXxXx         jmp     ee+84359
                    */
                    OverWriteOnProtectHelper h(GLOBAL::exedit_base + 0x8432d, 5);
                    h.store_i8(0, '\xe9');
                    h.replaceNearJmp(1, &asm_func_wndproc_undo_scene_audio);
                }
            }
        }

        void switching(bool flag) {
            enabled = flag;
        }

        bool is_enabled() { return enabled; }
        bool is_enabled_i() { return enabled_i; }

        void switch_load(ConfigReader& cr) {
            cr.regist(key, [this](json_value_s* value) {
                ConfigReader::load_variable(value, enabled);
                });
        }

        void switch_store(ConfigWriter& cw) {
            cw.append(key, enabled);
        }
    } playback_speed;
} // namespace patch

#endif // ifdef PATCH_SWITCH_PLAYBACK_SPEED
