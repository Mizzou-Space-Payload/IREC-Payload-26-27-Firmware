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
 *      >   https://www.youtube.com/watch?v=C2xF3O6qkbg 
 *      >   https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/system/wdts.html#_CPPv417esp_task_wdt_initPK21esp_task_wdt_config_t
 */
 
/**
* Types of watchdogs.
*
*   1. Hard watchdog (WDT -> physical countdown timer)
*       -   Hardware countdown timer that resets the chip if it isn't
*               petted before it hits zero.
*       -   Works even when the CPU is stuck looping non-stop / fully hung.
*       -   Doesn't know which task failed, only that nobody petted it.
*
*       PET:  the soft watchdog pets it (resets the countdown) when
*                 every task is healthy -> "good boy".
*       BARK: countdown hits zero because nobody petted it in time
*                 -> hardware resets the whole chip.
*
*   2. Soft watchdog (SWDT)
*       -   Task that tracks "heartbeats" (check-ins) from other tasks.
*       -   Knows exactly which task is healthy and which one died.
*
*       PET:  each monitored task pets the soft watchdog by checking in
*                 (heartbeat) before its timeout.
*       BARK: a task misses its check-in -> soft watchdog logs which task
*                 died, flags it in system_status, and restarts it.
*                 If it can't recover, it stops petting the hard watchdog
*                 -> hard watchdog barks and resets the chip.
*
*   // pet watchdog when it is being a "good boy" (often called as kicking)
*   // barks when system is intrrupted (fails)
*
*   // Starved => low priority task that wants to run but there exists a higher 
*       priority task or multiple tasks that exist that are full hardware resources
* 
*   Flow: 
*       tasks -> heartbeat -> soft watchdog -> healthy? -> pet hard watchdog -> not healthy -> BARK
*
*/

#include "watchdog_task.h"

/* ------------------
* config 
*/ ------------------

// TODO: task name, stack size, priority, period 

/* ------------------
* private state  
*/ ------------------

// TODO: task handle, system_status pointer 

/* ------------------
* Task 
*/ ------------------

static void watchdog_task(void * arg) {

    // TODO: setup

    while(1) {

        // TODO: check that every task's heartbeat 
        // TODO: pet the cute dog 
        // TODO: bark at person if task is missed 
    }
}

/* ------------------
* public API  
*/ ------------------
esp_err_t watchdog_task_start(
    system_status_t *system_status) {

        // TODO: validate args 
        // TODO: create task 


        // from esp_err.h 
        // #define ESP_OK          0       /*!< esp_err_t value indicating success (no error) */
        // #define ESP_FAIL        -1      /*!< Generic esp_err_t code indicating failure */
        return ESP_OK; 
    }