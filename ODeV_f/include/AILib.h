/**
 * AI header file
 */

/* Includes */
#include <stdint.h>

/* Define */
#define AI_AXIS_NUMBER 3
#define AI_DATA_INPUT_USER 1024

/* Function prototypes */
#ifdef __cplusplus
extern "C" {
#endif

  void AIInit(void);
	uint8_t AILearn(float data_input[]);
	uint8_t AIDetect(float data_input[]);
	void AISetSensitivity(float sensitivity);
	float AIGetSensitivity(void);

#ifdef __cplusplus
}
#endif
