/*
 * Watchdog_task.c
 *
 *  Created on:                 Nov 5, 2026
 *      Author:                 sir_l
 *
 *  Last Modified: 				Nov 14, 2026
 *  User that last modified: 	Joey_J
 *
 *  Desc: 
 *      "Much like a small, yappy dog that lives in a celebrity’s purse, watchdog timers are 
 *          often considered unnecessary or excessive. To equate the two, how ever, would do 
 *          a great disservice to the watchdog. Unlike “purse dogs,” watchdogs add critical 
 *          monitoring features that allow you to internally and externally monitor your system 
 *          for failure and take action should that happen."
 * 
 *  Sources Used: 
 *      >   https://www.ti.com/document-viewer/lit/html/SSZTAH7 
 *      >   
 *      >
 */

 // 
 // https://www.ti.com/document-viewer/lit/html/SSZTAH7
 
/**
* Types of watchdogs. 
*   1. Hard watchdog (WDT -> physical countdown timer) 
*       -   Physical countodwn timer that tracks whether individual tasks 
            are actively running, yielding or meeting their expected execution. 
        -   
*   2. Soft watchdog.
*
 */