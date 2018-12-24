/*
 * main.cpp
 *
 *  Created on: Dec 7, 2018
 *      Author: oliveris
 */




#include <stdio.h>
#include "sysinit.h"
#include "task.h"


int main()
{
  // System initialization.
  SysInit(FALSE);

  vTaskStartScheduler();

  while (1);
}
