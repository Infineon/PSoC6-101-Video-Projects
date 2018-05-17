/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */

    UART_Start();

    /* Disable STDIN input buffering */
    setvbuf ( stdin, NULL, _IONBF, 0 );
    
    char c;
    
    printf("Started UART\r\n");
    
    for(;;)
    {
        /* Ge a character from the UART. If there is a character (i.e. non-zero), echo it back. */
        c = getchar();
        if(c)
        {
            printf("%c",c);
        }
    }
}

/* [] END OF FILE */
