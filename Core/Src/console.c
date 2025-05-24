/**
  * Copyright (c) 2025, BSides Vilnius
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  */

#include "console.h"
#include "battery.h"
#include "settings.h"
#include "sao.h"
#include <stdio.h>
#include <string.h>

// The active handler for input
cmd_handler_t cmd_handler = &cmd_default_handler;
cmd_function_t console_pending_command = 0;

const char flag_secret[] = "HONEYPOT";

void console_begin() {
	printf("> ");
	fflush(stdout);
}

void flag_check(char * buf, uint32_t buflen) {
	int match = -1;
	// restore default handler
	cmd_handler = &cmd_default_handler;
	int secret_len = strlen(flag_secret);
	if (buflen == secret_len) {
		match = strncmp(buf, flag_secret, strlen(flag_secret));
	}
	if (match == 0) {
		printf(
				"      (   )\\ ) )\\ ) )\\ )        )\\ )       \r\n"
				"    ( )\\ (()/((()/((()/(   (   (()/(       \r\n"
				"    )((_) /(_))/(_))/(_))  )\\   /(_))      \r\n"
				"   ((_)_ (_)) (_)) (_))_  ((_) (_))        \r\n"
				"    | _ )/ __||_ _||   \\ | __|/ __|        \r\n"
				"    | _ \\__ \\  ||  | |) || _| \\__ \\        \r\n"
				"    |___/|__(/|__(||___/ )__(||___/     (  \r\n"
				"         )\\ ) )\\ )  ( /(  )\\ )        )\\ ) \r\n"
				" (   (  (()/((()/(  )\())(()/(    (  (()/(  \r\n"
				" )\\  )\\  /(_))/(_))((_)\\  /(_))   )\\  /(_))\r\n"
				"((_)((_)(_)) (_))   _((_)(_))  _ ((_)(_))  \r\n"
				" \\ \\ / /|_ _|| |   | \\| ||_ _|| | | |/ __| \r\n"
				"  \\ V /  | | | |__ | .` | | | | |_| |\\__ \\ \r\n"
				"   \\_/  |___||____||_|\\_||___| \\___/ |___/ \r\n"
				"           FLAG:BSIDES{M4Dn35S_4ND_0RD3R}\r\n"
		);
	} else {
		printf("Password incorrect.\r\n");
	}
	console_begin();
}

// Command: flag
int cmd_flag() {
	// handler for password
	cmd_handler = &flag_check;
	printf(
			"Vm1wS2IyRkhVa1JSYlhScFpWVktNMWRzWXpWa01rcElWbGRrYTFJeWFIZFpi\r\n"
			"VEY2V2pKSmVWZFhaR3RTTW1oM1dUTnNRMlF5U2toU2JYQmhWa1JuT1E9PQ==\r\n"
			"Password: "
	);
	fflush(stdout);
	return 1;
};

// Command: sao
int cmd_sao() {
	sao_header_t header;
	sao_status_t status = sao_get_header(&header);
	if (status != SAO_OK) {
		if (status == SAO_INVALID) {
			printf("SAO is not valid.\r\n");
		} else {
			printf("SAO not found.\r\n");
		}
		return 0;
	}

	const uint32_t sao_header_size = sizeof(header);
	char badge_name[header.badge_name_len+1];
	badge_name[header.badge_name_len]=0;
	status = sao_read(sao_header_size, badge_name, header.badge_name_len);
	if (status != SAO_OK) {
		printf("SAO badge name error\r\n");
		return 0;
	}
	printf("%s", badge_name);

	char driver_name[header.driver_name_len+1];
	driver_name[header.driver_name_len]=0;
	status = sao_read(sao_header_size+header.badge_name_len, driver_name, header.driver_name_len);
	if (status != SAO_OK) {
		printf("SAO driver name error\r\n");
		return 0;
	}
	printf(":%s\r\n", driver_name);
	return 0;
}

int cmd_settings_erase() {
	settings.raw = 0;
	settings_save();
	printf("Settings erased.\r\n");
	return 0;
}


int cmd_power() {
	print_power();
	return 0;
}

#ifdef __TESTING

int cmd_write_speaker() {
	char data[]={'L', 'I', 'F', 'E', 5, 7, 0, 0, 'B', 'S', 'V', '2', '5', 'S', 'P', 'E', 'A', 'K', 'E', 'R'};
	sao_status_t status = sao_write(0, data, 20);
	printf("SAO write completed (%d).\r\n", status);
	return 0;
}

int cmd_write_sponsor() {
	char data[]={'L', 'I', 'F', 'E', 5, 7, 0, 0, 'B', 'S', 'V', '2', '5', 'S', 'P', 'O', 'N', 'S', 'O', 'R'};
	sao_status_t status = sao_write(0, data, 20);
	printf("SAO write completed (%d).\r\n", status);
	return 0;
}

int cmd_write_crew() {
	char data[]={'L', 'I', 'F', 'E', 5, 4, 0, 0, 'B', 'S', 'V', '2', '5', 'C', 'R', 'E', 'W'};
	sao_status_t status = sao_write(0, data, 17);
	printf("SAO write completed (%d).\r\n", status);
	return 0;
}

int cmd_write_zero() {
	char data[24] = {0};
	sao_status_t status = sao_write(0, data, 24);
	printf("SAO write completed (%d).\r\n", status);
	return 0;
}

int cmd_sao_dump() {
	const uint8_t size = 24;
	char data[size];
	memset(data, 0, size);
	sao_status_t status = sao_read(0, data, size);
	if (status == SAO_OK) {
		for (int i=0; i<size; i++) {
			printf("%02x ", data[i]);
		}
		printf("\r\n");
	} else {

	}
	return 0;
}

int cmd_settings_dump() {
	printf("%02X\r\n", settings.raw);
	return 0;
}

#endif //__TESTING

// Command: help
int cmd_help() {
	printf("Useful commands:\r\n");
	printf("   flag  - badge challenge\r\n");
	printf("   power - get power/battery measurements\r\n");
	printf("   sao   - check Simple Add-On identifier\r\n");
	printf("Dangerous commands:\r\n");
	printf("   s_erase - settings erase\r\n");
	return 0;
};

const cmd_t cmd_list[] = {
		{.name = "help", .func=&cmd_help},
		{.name = "flag", .func=&cmd_flag},
		{.name = "power", .func=&cmd_power},
		{.name = "sao", .func=&cmd_sao},
		{.name = "s_erase", .func=&cmd_settings_erase},
#ifdef __TESTING
		{.name = "sao_dump", .func=&cmd_sao_dump},
		{.name = "w_speaker", .func=&cmd_write_speaker},
		{.name = "w_sponsor", .func=&cmd_write_sponsor},
		{.name = "w_crew", .func=&cmd_write_crew},
		{.name = "w_zero", .func=&cmd_write_zero},
		{.name = "s_dump", .func=&cmd_settings_dump},
#endif
};
const uint8_t cmd_list_len = sizeof(cmd_list)/sizeof(cmd_t);

void console_run_pending() {
	if (console_pending_command != NULL) {
		if ((*console_pending_command)() == 0) {
			console_begin();
		}
		console_pending_command = NULL;
	}
}

// Default command handler
// Finds and executes a matching function
void cmd_default_handler(char *buf, uint32_t buflen) {
	if (buflen == 0) {
		console_begin();
		return;
	}

	int match = 0;
	int cmp_len;

	for (uint8_t cmd_idx=0; cmd_idx<cmd_list_len; cmd_idx++) {
		cmp_len = strlen(cmd_list[cmd_idx].name);
		if (buflen != strlen(cmd_list[cmd_idx].name)) {
			continue;
		}
		match = strncmp(buf, cmd_list[cmd_idx].name, cmp_len);
		if (match == 0) {
			console_pending_command = cmd_list[cmd_idx].func;
			return;
		}
	}
	printf("Unknown command\r\n");
	console_begin();
}


/**************************
 * Console input callback *
 **************************/
const uint8_t console_backspace[] = { 0x08, 0x20, 0x08 };
const uint8_t console_enter[] = { 0x0a, 0x0d };
static char input_buf[CONSOLE_INPUT_MAXLEN];
static uint32_t input_idx = 0;

// Receives raw input and forms a complete string.
// Once enter is pressed, function pointed by cmd_handler is called
void console_input_callback(uint8_t *buf, uint32_t buflen) {
	switch (*buf) {
		case 13: //enter
			fwrite(console_enter, 2, 1, stdout);
			input_buf[input_idx]=0; //string termination
			cmd_handler(input_buf, input_idx);
			input_idx = 0;
			break;
		case 127: //backspace
			if (input_idx > 0) {
				fwrite(console_backspace, 3, 1, stdout);
				input_idx--;
			}
			break;
		case 27: //escape - ignore
			break;
		default:
			uint32_t cmd_len = input_idx + buflen;
			if (cmd_len <= CONSOLE_INPUT_MAXLEN) {
				memcpy(&input_buf[input_idx], buf, buflen);
				fwrite(buf, buflen, 1, stdout);
				input_idx = cmd_len;
			}
			break;
	}
	fflush(stdout);
}
