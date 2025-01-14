/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

#include "quantum.h"

// トラックボールを移動するとマウスレイヤーに自動遷移する機能ここから
// https://zenn.dev/takashicompany/articles/69b87160cda4b9
// ここを参考にさせていただいている
// 参考元との違いは
// 1. 時間経過によるレイヤーの無効化をしない
// 2. スクロールモードへの自動移行は行わない

enum click_state {
    NONE = 0,
    WAITING,    // マウスレイヤーが有効になるのを待つ。 Wait for mouse layer to activate.
    MOUSELAYER
};


enum click_state state;     // 現在のクリック入力受付の状態 Current click input reception status
uint16_t click_timer;       // タイマー。状態に応じて時間で判定する。 Timer. Time to determine the state of the system.
int16_t to_clickable_movement = 50;

const uint16_t mouse_layer = 2;   // マウス入力が可能になった際に有効になるレイヤー。Layers enabled when mouse input is enabled

int16_t mouse_movement;

// クリック用のレイヤーを有効にする。　Enable layers for clicks
void enable_mouse_layer(void) {
    layer_on(mouse_layer);
    click_timer = timer_read();
    state = MOUSELAYER;
}

// クリック用のレイヤーを無効にする。 Disable layers for clicks.
void disable_mouse_layer(void) {
    state = NONE;
    layer_off(mouse_layer);
}

// 自前の絶対数を返す関数。 Functions that return absolute numbers.
int16_t my_abs(int16_t num) {
    if (num < 0) {
        num = -num;
    }

    return num;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    switch (keycode) {
        case KC_BTN1:
        case KC_BTN2:
        case KC_BTN3:
        case KC_LEFT:
        case KC_RIGHT:
        case KC_UP:
        case KC_DOWN:
        case S(KC_LEFT):
        case S(KC_RIGHT):
        case S(KC_UP):
        case S(KC_DOWN):
        case G(KC_LEFT):
        case G(KC_RIGHT):
        case G(KC_UP):
        case G(KC_DOWN):
        case A(KC_LEFT):
        case A(KC_RIGHT):
        case A(KC_UP):
        case A(KC_DOWN):
        case C(KC_LEFT):
        case C(KC_RIGHT):
        case C(KC_UP):
        case C(KC_DOWN):
        case SCRL_MO:
        case SCRL_TO:
        {
            // マウスボタン及びLayer2に居るボタンを押された場合には何もしない
            return true;
        }
         default:
          // Layer2以外のボタン（KC_TRANS等）が押された場合にはマウスモードを終わる
            if  (record->event.pressed) {
                disable_mouse_layer();
            }
    }
    return true;
}


report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    int16_t current_x = mouse_report.x;
    int16_t current_y = mouse_report.y;

    if (current_x != 0 || current_y != 0) {
        if (state == WAITING) {
            mouse_movement += my_abs(current_x) + my_abs(current_y);
            if (mouse_movement >= to_clickable_movement)
            {
                mouse_movement = 0;
                enable_mouse_layer();
            }
        } else if (state == NONE){
            click_timer = timer_read();
            state = WAITING;
            mouse_movement = 0;
        }
    }
    else
    {
        if (state == WAITING) {
            if (timer_elapsed(click_timer) > 50) {
                mouse_movement = 0;
                state = NONE;
            }
        } else {
            mouse_movement = 0;
        }
    }

    return mouse_report;
}

// トラックボールを移動するとマウスレイヤーに自動遷移する機能ここまで

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_universal(
    KC_ESC   , KC_1     , KC_2     , KC_3     , KC_4     , KC_5     ,                                  KC_6     , KC_7     , KC_8     , KC_9     , KC_0     , KC_MINS  ,
    KC_DEL   , KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                                  KC_Y     , KC_U     , KC_I     , KC_O     , KC_P     , KC_INT3  ,
    KC_TAB   , KC_A     , KC_S     , KC_D     , KC_F     , KC_G     ,                                  KC_H     , KC_J     , KC_K     , KC_L     , KC_SCLN  , S(KC_7)  ,
    MO(1)    , KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     , KC_RBRC  ,              KC_NUHS, KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  , KC_RSFT  ,
    _______  , KC_LCTL  , KC_LALT  , KC_LGUI,LT(1,KC_LANG2),LT(2,KC_SPC),LT(3,KC_LANG1),    KC_BSPC,LT(2,KC_ENT),LT(1,KC_LANG2),KC_RGUI, _______ , KC_RALT  , KC_PSCR
  ),

  [1] = LAYOUT_universal(
    S(KC_ESC), S(KC_1)  , KC_LBRC  , S(KC_3)  , S(KC_4)  , S(KC_5)  ,                                  KC_EQL   , S(KC_6)  ,S(KC_QUOT), S(KC_8)  , S(KC_9)  ,S(KC_INT1),
    S(KC_DEL), S(KC_Q)  , S(KC_W)  , S(KC_E)  , S(KC_R)  , S(KC_T)  ,                                  S(KC_Y)  , S(KC_U)  , S(KC_I)  , S(KC_O)  , S(KC_P)  ,S(KC_INT3),
    S(KC_TAB), S(KC_A)  , S(KC_S)  , S(KC_D)  , S(KC_F)  , S(KC_G)  ,                                  S(KC_H)  , S(KC_J)  , S(KC_K)  , S(KC_L)  , KC_QUOT  , S(KC_2)  ,
    _______  , S(KC_Z)  , S(KC_X)  , S(KC_C)  , S(KC_V)  , S(KC_B)  ,S(KC_RBRC),           S(KC_NUHS), S(KC_N)  , S(KC_M)  ,S(KC_COMM), S(KC_DOT),S(KC_SLSH),S(KC_RSFT),
    _______  ,S(KC_LCTL),S(KC_LALT),S(KC_LGUI), _______  , _______  , _______  ,            _______  , _______  , _______  ,S(KC_RGUI), _______  , S(KC_RALT), _______
  ),

  [2] = LAYOUT_universal(
    _______  , KC_F1    , KC_F2    , KC_F3    , KC_F4    , KC_F5    ,                                  KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   , KC_F11   ,
    _______  , _______  , KC_7     , KC_8     , KC_9     , _______  ,                                  _______  , KC_LEFT  , KC_UP    , KC_RGHT  , _______  , KC_F12   ,
    _______  , _______  , KC_4     , KC_5     , KC_6     ,S(KC_SCLN),                                  KC_PGUP  , KC_BTN1  , KC_DOWN  , KC_BTN2  , KC_BTN3  , _______  ,
    _______  , _______  , KC_1     , KC_2     , KC_3     ,S(KC_MINS), S(KC_8)  ,            S(KC_9)  , KC_PGDN  , _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , KC_0     , KC_DOT   , _______  , _______  , _______  ,             KC_DEL  , _______  , _______  , _______  , _______  , _______  , _______
  ),

  [3] = LAYOUT_universal(
    RGB_TOG  , _______  , _______  , _______  , _______  , _______  ,                                  RGB_M_P  , RGB_M_B  , RGB_M_R  , RGB_M_SW , RGB_M_SN , RGB_M_K  ,
    RGB_MOD  , RGB_HUI  , RGB_SAI  , RGB_VAI  , _______  , _______  ,                                  RGB_M_X  , RGB_M_G  , RGB_M_T  , RGB_M_TW , _______  , _______  ,
    RGB_RMOD , RGB_HUD  , RGB_SAD  , RGB_VAD  , _______  , _______  ,                                  CPI_D1K  , CPI_D100 , CPI_I100 , CPI_I1K  , KBC_SAVE , KBC_RST  ,
    _______  , _______  , SCRL_DVD , SCRL_DVI , SCRL_MO  , SCRL_TO  , EEP_RST  ,            EEP_RST  , KC_HOME  , KC_PGDN  , KC_PGUP  , KC_END   , _______  , _______  ,
    RESET    , _______  , KC_LEFT  , KC_DOWN  , KC_UP    , KC_RGHT  , _______  ,            _______  , KC_BSPC  , _______  , _______  , _______  , _______  , RESET
  ),
};
// clang-format on

const rgblight_segment_t PROGMEM my_layer0_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 74, HSV_GREEN}
);
const rgblight_segment_t PROGMEM my_layer1_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 74, HSV_PURPLE}
);
const rgblight_segment_t PROGMEM my_layer2_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 74, HSV_RED}
);
const rgblight_segment_t PROGMEM my_layer3_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 74, HSV_CYAN}
);

const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    my_layer0_layer,
    my_layer1_layer,
    my_layer2_layer,
    my_layer3_layer
);

layer_state_t layer_state_set_user(layer_state_t state) {
    rgblight_set_layer_state(0, get_highest_layer(state) == 0);
    rgblight_set_layer_state(1, get_highest_layer(state) == 1);
    rgblight_set_layer_state(2, get_highest_layer(state) == 2);
    rgblight_set_layer_state(3, get_highest_layer(state) == 3);
    return state;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
    rgblight_set_layer_state(0, get_highest_layer(state) == 0);
    return state;
}

void keyboard_post_init_user(void) {
    rgblight_layers = my_rgb_layers;
    rgblight_set_layer_state(0, true);
}

#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
}
#endif
