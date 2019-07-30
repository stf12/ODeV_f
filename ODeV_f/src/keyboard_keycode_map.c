/**
 ******************************************************************************
 * @file    usb_keyboard_keycode_map.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Dec 21, 2016
 * @brief   Keyboard key codes map definition
 *
 * This file defines the key codes map for a specific keyboard.
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

#include "KeyboardKeyCodesMap.h"

/**
 * This map defines the keyboard layout. It is application specific and it is used by the key scanner algorithm.
 */
const uint8_t g_ppn_keyboard_kc_map[8][KM_COLUMN_NUMBERS] = {
//   --00--  --01--  --02--  --03--  --04--  --05--  --06--  --07--  --08--  --09--  --10--  --11--  --12--  --13--
    {KC_NUL, KC_022, KC_016, KC_030, KC_021, KC_NUL, KC_027, KC_028, KC_NUL, KC_NUL, KC_NUL, KC_NUL, KC_015, KC_044 },  // --00--
    {KC_NUL, KC_008, KC_002, KC_003, KC_005, KC_010, KC_011, KC_009, KC_004, KC_NUL, KC_NUL, KC_NUL, KC_NUL, KC_NUL },  // --01--
    {KC_NUL, KC_007, KC_001, KC_NUL, KC_006, KC_NUL, KC_012, KC_013, KC_NUL, KC_NUL, KC_076, KC_NUL, KC_NUL, KC_NUL },  // --02--
    {KC_NUL, KC_052, KC_046, KC_047, KC_049, KC_054, KC_129, KC_053, KC_048, KC_NUL, KC_NUL, KC_NUL, KC_043, KC_NUL },  // --03--
    {KC_062, KC_051, KC_NUL, KC_NUL, KC_050, KC_NUL, KC_055, KC_NUL, KC_NUL, KC_089, KC_084, KC_079, KC_NUL, KC_NUL },  // --04--
    {KC_NUL, KC_037, KC_031, KC_032, KC_034, KC_039, KC_040, KC_038, KC_033, KC_NUL, KC_NUL, KC_NUL, KC_029, KC_057 },  // --05--
    {KC_060, KC_036, KC_110, KC_029, KC_035, KC_NUL, KC_041, KC_130, KC_NUL, KC_NUL, KC_061, KC_083, KC_NUL, KC_NUL },  // --06--
    {KC_NUL, KC_023, KC_017, KC_018, KC_020, KC_025, KC_026, KC_024, KC_019, KC_NUL, KC_NUL, KC_NUL, KC_NUL, KC_NUL },  // --07--
};

// Private member function declaration
// ***********************************



// Public API definition
// *********************

