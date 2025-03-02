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

#ifdef PATCH_SWITCH_RELATIVE_PATH_PATCH
#include <Windows.h>

#include <exedit.hpp>

#include "cryptostring.hpp"
#include "util_others.hpp"
#include "util_resource.hpp"
#include "config_rw.hpp"

namespace patch {
    /* 相対パス保存プラグインのバグ修正 (対応バージョン：相対パスv0.9b by rikky)
        バグの概要：
            フィルタ効果を最大まで付けたオブジェクトで問題が起こる
        
        原因：
            ループ条件が正しくない
            while( obj->filter_param[i].filter_id != -1 )
        
        修正：
            ループ条件にi < 12を追加する
    */

    inline class patch_relative_path_t {

        bool enabled = true;
        bool enabled_i;

        inline static const char key[] = "patch_relative_path";

        inline static int filter_idx = 0;
        
    public:
        void init(HMODULE hmod) {
            enabled_i = enabled;
            if (!enabled_i)return;

            uint32_t& base = (uint32_t&)hmod;
            auto& cursor = GLOBAL::executable_memory_cursor;
            { // project_saveの data!=NULL
                /*
                    64081915 8385c0fdffff01     add     dword ptr [ebp+fffffdc0],+01
                    ↓
                    64081915 e9XxXxXxXx         jmp     cursor

                    cursor00 ff85c0fdffff       inc     dword ptr [ebp+fffffdc0]
                    cursor06 83bdc0fdffff0c     cmp     dword ptr [ebp+fffffdc0],+0c
                    cursor0d 0f8cXxXxXxXx       jl      relative_path+0x191c
                    cursor13 e9XxXxXxXx         jmp     relative_path+0x1b50
                */
                OverWriteOnProtectHelper h(base + 0x1915, 5);
                h.store_i8(0, '\xe9');
                h.replaceNearJmp(1, cursor);

                store_i32(cursor, '\xff\x85\xc0\xfd'); cursor += 4;
                store_i32(cursor, '\xff\xff\x83\xbd'); cursor += 4;
                store_i32(cursor, '\xc0\xfd\xff\xff'); cursor += 4;
                store_i32(cursor, '\x0c\x0f\x8c\x00'); cursor += 3;
                store_i32(cursor, base + 0x191c - (int)cursor - 4); cursor += 4;
                store_i8(cursor, '\xe9'); cursor++;
                store_i32(cursor, base + 0x1b50 - (int)cursor - 4); cursor += 4;
            }
            { //  // project_saveの data==NULL
                { // ループ前にfilter_idx=0
                    /*
                        64081c2f 8bb58cfdffff       mov     esi,dword ptr [ebp+fffffd8c]
                        ↓
                        64081c2f 90                 nop
                        64081c30 e8XxXxXxXx         call    cursor

                        cursor00 8bb58cfdffff       mov     esi,dword ptr [ebp+fffffd8c]
                        cursor06 c705XxXxXxXx00000000 mov     dword ptr [filter_idx],00000000
                        cursor10 c3                 ret
                    */
                    OverWriteOnProtectHelper h(base + 0x1c2f, 6);
                    h.store_i16(0, '\x90\xe8');
                    h.replaceNearJmp(2, cursor);

                    store_i32(cursor, '\x8b\xb5\x8c\xfd'); cursor += 4;
                    store_i32(cursor, '\xff\xff\xc7\x05'); cursor += 4;
                    store_i32(cursor, &filter_idx); cursor += 4;
                    store_i32(cursor, '\x00\x00\x00\x00'); cursor += 4;
                    store_i8(cursor, '\xc3'); cursor++;
                }
                { // ループ内にfilter_idx++; if(filter_idx==12)break;
                    /*
                        64081cfa 8d460c             lea     eax,dword ptr [esi+0c]
                        64081cfd 83fbff             cmp     ebx,-01
                        ↓
                        64081cfa 90                 nop
                        64081cfb e8XxXxXxXx         call    cursor

                        64081d62 83fbff             cmp     ebx,-01
                        64081d65 8d460c             lea     eax,dword ptr [esi+0c]
                        ↓
                        64081d62 90                 nop
                        64081d63 e8XxXxXxXx         call    cursor


                        cursor00 ff05XxXxXxXx       inc     dword ptr [filter_idx]
                        cursor00 833dXxXxXxXx0c     cmp     dword ptr [filter_idx],+0c
                        cursor00 7406               jz      skip,06 ; そのままretに行けばjnzループを抜ける
                        cursor00 8d460c             lea     eax,dword ptr [esi+0c]
                        cursor00 83fbff             cmp     ebx,-01
                        cursor00 c3                 ret
                    */

                    constexpr int vp_begin = 0x1cfa;
                    OverWriteOnProtectHelper h(base + vp_begin, 0x1d68 - vp_begin);
                    h.store_i16(0x1cfa - vp_begin, '\x90\xe8');
                    h.replaceNearJmp(0x1cfc - vp_begin, cursor);
                    h.store_i16(0x1d62 - vp_begin, '\x90\xe8');
                    h.replaceNearJmp(0x1d64 - vp_begin, cursor);

                    store_i16(cursor, '\xff\x05'); cursor += 2;
                    store_i32(cursor, &filter_idx); cursor += 4;
                    store_i16(cursor, '\x83\x3d'); cursor += 2;
                    store_i32(cursor, &filter_idx); cursor += 4;
                    store_i32(cursor, '\x0c\x74\x06\x8d'); cursor += 4;
                    store_i32(cursor, '\x46\x0c\x83\xfb'); cursor += 4;
                    store_i16(cursor, '\xff\xc3'); cursor += 2;
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
    } patch_relative_path;
}
#endif
