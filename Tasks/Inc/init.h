/*
----------------------------------------------------------------------
File    : init.h
Purpose : Implementation for the system initialization task. This task
          is responsible for initializing ALL sensors and peripherals.
          Including configuration parsing.
Revision: $Rev: 2023.49$
----------------------------------------------------------------------
*/

#ifndef INIT_H
#define INIT_H

#include "tx_api.h"

#ifdef DEBUG
#include "SEGGER_RTT.h"
#define INIT_DEBUG(...) SEGGER_RTT_printf(0, __VA_ARGS__)
#else
#define INIT_DEBUG(...)
#endif

void init_thread_entry(ULONG thread_input);

#endif
