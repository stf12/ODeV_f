/**
 ******************************************************************************
 * @file    KeyboardKeyCodesMap.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Dec 22, 2016
 *
 * @brief Keys code definition.
 *
 * This file define the keys code and a set of macro to access the keys map.
 * The key map is declared here as extern and defined by the user application.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#ifndef USB_KEYBOARD_KEYCODE_MAP_H_
#define USB_KEYBOARD_KEYCODE_MAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

// Key Code to ASCII code definition
#define KC_001 0x35 ///< '`'              110101
#define KC_002 0x1E ///< '1'              11110
#define KC_003 0x1F ///< '2'              11111
#define KC_004 0x20 ///< '3'              100000
#define KC_005 0x21 ///< '4'              100001
#define KC_006 0x22 ///< '5'              100010
#define KC_007 0x23 ///< '6'              100011
#define KC_008 0x24 ///< '7'              100100
#define KC_009 0x25 ///< '8'              100101
#define KC_010 0x26 ///< '9'              100110
#define KC_011 0x27 ///< '0'              100111
#define KC_012 0x2D ///< '-'              101101
#define KC_013 0x2E ///< '='              101110
#define KC_015 0x2A ///< backspace        101010
#define KC_016 0x2B ///< tab              101011
#define KC_017 0x14 ///< 'q'              10100
#define KC_018 0x1A ///< 'w'              11010
#define KC_019 0x08 ///< 'e'              1000
#define KC_020 0x15 ///< 'r'              10101
#define KC_021 0x17 ///< 't'              10111
#define KC_022 0x1C ///< 'y'              11100
#define KC_023 0x18 ///< 'u'              11000
#define KC_024 0x0C ///< 'i'              1100
#define KC_025 0x12 ///< 'o'              10010
#define KC_026 0x13 ///< 'p'              10011
#define KC_027 0x2F ///< '['              101111
#define KC_028 0x30 ///< ']'              110000
#define KC_029 0x31 ///< '\'              110001
#define KC_030 0x39 ///< caps lock        111001
#define KC_031 0x04 ///< 'a'              100
#define KC_032 0x16 ///< 's'              10110
#define KC_033 0x07 ///< 'd'              111
#define KC_034 0x09 ///< 'f'              1001
#define KC_035 0x0A ///< 'g'              1010
#define KC_036 0x0B ///< 'h'              1011
#define KC_037 0x0D ///< 'j'              1101
#define KC_038 0x0E ///< 'k'              1110
#define KC_039 0x0F ///< 'l'              1111
#define KC_040 0x33 ///< ';'              110011
#define KC_041 0x34 ///< '''              110100
#define KC_043 0x28 ///< enter            101000
#define KC_044 0xE1 ///< left shift       11100001
#define KC_046 0x1D ///< 'z'              11101
#define KC_047 0x1B ///< 'x'              11011
#define KC_048 0x06 ///< 'c'              110
#define KC_049 0x19 ///< 'v'              11001
#define KC_050 0x05 ///< 'b'              101
#define KC_051 0x11 ///< 'n'              10001
#define KC_052 0x10 ///< 'm'              10000
#define KC_053 0x36 ///< ','              110110
#define KC_054 0x37 ///< '.'              110111
#define KC_055 0x38 ///< '/'              111000
#define KC_057 0xE5 ///< right shift      11100101
#define KC_058 0xE0 ///< left control     11100000
#define KC_059 0xED ///< fn - ?           11111111
#define KC_060 0xE2 ///< left alt         11100010
#define KC_061 0x2C ///< space            101100
#define KC_062 0xE6 ///< right alt        11100110
#define KC_064 0xE4 ///< right control    11100100
#define KC_070 0x46 ///< Print Screen
#define KC_072 0x48 ///< Pause (F14)
#define KC_075 0x49 ///< insert           1001001
#define KC_076 0x4C ///< delete           1001100
#define KC_079 0x50 ///< left arrow       1010000
#define KC_083 0x52 ///< up arrow         1010010
#define KC_084 0x51 ///< down arrow       1010001
#define KC_089 0x4F ///< right arrow      1001111
#define KC_110 0x29 ///< esc              101001
#define KC_112 0x3A ///< f1               111010
#define KC_113 0x3B ///< f2               111011
#define KC_114 0x3C ///< f3               111100
#define KC_115 0x3D ///< f4               111101
#define KC_116 0x3E ///< f5               111110
#define KC_117 0x3F ///< f6               111111
#define KC_118 0x40 ///< f7               1000000
#define KC_119 0x41 ///< f8               1000001
#define KC_120 0x42 ///< f9               1000010
#define KC_121 0x43 ///< f10              1000011
#define KC_122 0x44 ///< f11              1000100
#define KC_123 0x45 ///< f12              1000101
#define KC_126 0xEE ///< fn lock - ?      11111111
#define KC_127 0xE3 ///< left gui         11100011
#define KC_128 0x35 ///< Grave accent or Tilde

#define KC_NUL 0xFF

// definition of the the special key
#define KC_LEFT_CTRL      KC_058
#define KC_RIGHT_CTRL     KC_064
#define KC_LEFT_SHIFT     KC_044
#define KC_RIGHT_SHIFT    KC_057
#define KC_LEFT_ALT       KC_060
#define KC_RIGHT_ALT      KC_062
#define KC_LEFT_GUI       KC_127
#define KC_RIGHT_GUI      KC_NUL
#define KC_FN_LOCK        KC_126
#define KC_FN             KC_059
#define KC_FN1            KC_112
#define KC_FN2            KC_113
#define KC_FN3            KC_114
#define KC_FN4            KC_115
#define KC_FN5            KC_116
#define KC_FN6            KC_117
#define KC_FN7            KC_118
#define KC_FN8            KC_119
#define KC_FN9            KC_120
#define KC_FN10           KC_121
#define KC_FN11           KC_122
#define KC_FN12           KC_123

#define KC_F24            0x73
#define KC_MENU           0x65
#define KC_HOME           0x4A
#define KC_END            0x4D
#define KC_PAGEDOWN       0x4E
#define KC_PAGEUP         0x4B
#define KC_PRINTSCREEN    0x46

#define KM_COLUMN_NUMBERS 14

/**
 * Specifies the keyboard layout. The map is defined by the application. Each not used key position is mapped to KC_NULL.
 */
extern const uint8_t g_ppn_keyboard_kc_map[8][KM_COLUMN_NUMBERS];


// Public API declaration
//***********************
/**
 * Get a pointer to the keys map.
 */
#define KEYBOARD_GET_KEY_MAP()                (g_ppn_keyboard_kc_map)

/**
 * Get the key code at position (row, column)
 */
#define KEYBOARD_GET_KEY_CODE(row, column)    g_ppn_keyboard_kc_map[(row)][(column)]

/**
 * Check if a key code key code correspond to a special not alphanumeric key.
 */
#define KEYBOARD_IS_SPECIAL_KEY(keycode)      (((keycode) & 0xF0) == 0xE0)

/**
 * Check if a key code key code correspond to a function key F1..F12
 */
#define KEYBOARD_IS_FUNCTION_KEY(keycode)     ((keycode>=KC_112) && (keycode<=KC_123))



// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif


#endif /* USB_KEYBOARD_KEYCODE_MAP_H_ */
