/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "../inc/tx_api.h"
#include "../inc/tx_thread.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_stack_error_handler                      PORTABLE C      */
/*                                                           6.0          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes stack errors detected during run-time. The  */
/*    processing currently consists of a spin loop.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                            Thread control block pointer  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ThreadX internal code                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*                                                                        */
/**************************************************************************/
VOID  _tx_thread_stack_error_handler(TX_THREAD *thread_ptr)
{

TX_INTERRUPT_SAVE_AREA

#ifdef TX_ENABLE_STACK_CHECKING

    /* Disable interrupts.  */
    TX_DISABLE

    /* Determine if the application has registered an error handler.  */
    if (_tx_thread_application_stack_error_handler != TX_NULL)
    {

        /* Yes, an error handler is present, simply call the application error handler.  */
        (_tx_thread_application_stack_error_handler)(thread_ptr);
    }

    /* Restore interrupts.  */
    TX_RESTORE

#else

    /* Access input argument just for the sake of lint, MISRA, etc.  */
    if (thread_ptr != TX_NULL)
    {

        /* Disable interrupts.  */
        TX_DISABLE
        
        /* Restore interrupts.  */
        TX_RESTORE
    }
#endif
}

