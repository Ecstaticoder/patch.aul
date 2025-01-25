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

#include "macro.h"

#ifdef PATCH_SWITCH_MIDPT_DELETE
#include <memory>

#include <exedit.hpp>

#include "global.hpp"
#include "util.hpp"
#include "config_rw.hpp"

namespace patch {

    // init at exedit load
    // 中間点を削除にて、設定ダイアログが更新されないのを修正
    inline class midopt_delete_t {
        bool enabled = true;
        bool enabled_i;
        inline static const char key[] = "midpt_delete";

        static void __cdecl delete_object_wrap(int right_idx);

    public:
        void init() {
            enabled_i = enabled;

            if (!enabled_i)return;

            ReplaceNearJmp(GLOBAL::exedit_base + 0x34b0d, &delete_object_wrap);
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
    } midpt_delete;
} // namespace patch

#endif // ifdef PATCH_SWITCH_MIDPT_DELETE
