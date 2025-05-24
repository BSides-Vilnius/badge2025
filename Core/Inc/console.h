/**
  * Copyright (c) 2025, BSides Vilnius
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  */

#ifndef INC_CONSOLE_H_
#define INC_CONSOLE_H_

#include "stdint.h"

#define CMD_MAXLEN 10
#define CONSOLE_INPUT_MAXLEN 64


// function definition for command handling
typedef void (*cmd_handler_t) (char *, uint32_t);

// function definition for generic commands
// if non-zero value is returned, console prompt is not printed.
typedef int (*cmd_function_t) ();

typedef struct {
	char name[CMD_MAXLEN];
	cmd_function_t func;
} cmd_t;


void console_input_callback(uint8_t *, uint32_t);
void cmd_default_handler(char *, uint32_t);
void console_run_pending();

#endif /* INC_CONSOLE_H_ */
