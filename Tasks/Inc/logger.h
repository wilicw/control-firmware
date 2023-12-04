/*
----------------------------------------------------------------------
File    : logger.h
Purpose : Implementation for the data logger task. This task is
          responsible for logging data to the microSD card or other
          storage media.
Revision: $Rev: 2023.49$
----------------------------------------------------------------------
*/

#ifndef LOGGER_H
#define LOGGER_H

#include "tx_api.h"

#ifdef DEBUG
#include "SEGGER_RTT.h"
#define LOGGER_DEBUG(...) SEGGER_RTT_printf(0, __VA_ARGS__)
#else
#define LOGGER_DEBUG(...)
#endif

void logger_thread_entry(ULONG thread_input);

#endif
