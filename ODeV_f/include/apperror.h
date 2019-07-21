/**
 ******************************************************************************
 * @file    apperror.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 14, 2017
 *
 * @brief Application specific error code
 *
 * Application defines its own error code in this file starting form the
 * constant APP_BASE_ERROR_CODE.
 * It is recommended to group the error code in the following groups:
 * - Low Level API error code
 * - Service Level error code
 * - Task Level error code
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
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
#ifndef APPERROR_H_
#define APPERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

// Low Level API error code
// ************************

// IP error
//#define SYS_BASE_XX_ERROR_CODE                               APP_BASE_ERROR_CODE

// KeyMatrix Driver error
#define SYS_BASE_KMDRV_ERROR_CODE                             APP_BASE_ERROR_CODE
#define SYS_KMDRV_INVALID_CONFIG_ERROR_CODE                   SYS_BASE_KMDRV_ERROR_CODE + 1



// Service Level error code
// ************************

// KeyScanner error
#define SYS_BASE_KS_ERROR_CODE                                SYS_BASE_KMDRV_ERROR_CODE + SYS_GROUP_ERROR_COUNT
#define SYS_KS_TIMEOUT_ERROR_CODE                             SYS_BASE_KS_ERROR_CODE + 1


// Task Level error code
// *********************

#define SYS_BASE_HCC_BASE_ERROR_CODE                          SYS_BASE_KS_ERROR_CODE + SYS_GROUP_ERROR_COUNT
#define SYS_HCC_TASK_REPORT_LOST_ERROR_CODE                   SYS_BASE_HCC_BASE_ERROR_CODE + 1


#ifdef __cplusplus
}
#endif

#endif /* APPERROR_H_ */
