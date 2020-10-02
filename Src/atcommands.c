#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "atcommands.h"

/*
atcmd_t atcmdtable[] = {
	{"AT\r\n", "OK", 5},
	{"ATE0\r\n", "OK", 5},
	{"AT\r\n", "OK", 5},
	{"AT+CIPSTATUS\r\n", "IP INITIAL", 5},
	{"AT+CSTT=\"gpinternet\",\"\",\"\"\r\n", "OK", 5},
	{"AT+CIPSTATUS\r\n", "IP START", 5},
	{"AT+CIICR\r\n", "OK", 5},
	{"AT+CIPSTATUS\r\n", "IP GPRSACT", 5},
	{"AT+CIFSR\r\n", "__EGO", 5},
	{"AT+CIPSTATUS\r\n","IP STATUS",5},
	{"AT+CIPSTART=\"TCP\",\"rms.stealthshop.xyz\",\"80\"\r\n", "CONNECT OK", 5},
	{"AT+CIPSEND\r", ">", 5},
	{"GET /gate/pinlog.php\r\n", "^Z", 20},
	{"^Z", "SEND OK", 20},
	{"", "CLOSED", 20}
};
*/


atcmd_t atcmdtable[] = {
	{"AT\r\n", "OK", 5},
	{"ATE0\r\n", "OK", 5},
	{"AT\r\n", "OK", 5},
	{"AT+CIPSTATUS\r\n", "IP INITIAL", 5},
	{"AT+CSTT=\"gpinternet\",\"\",\"\"\r\n", "OK", 5},
	{"AT+CIPSTATUS\r\n", "IP START", 5},
	{"AT+CIICR\r\n", "OK", 5},
	{"AT+CIPSTATUS\r\n", "IP GPRSACT", 5},
	{"AT+CIFSR\r\n", "__EGO", 5},
	{"AT+CIPSTATUS\r\n","IP STATUS",5},
	{"AT+CIPSTART=\"TCP\",\"103.110.113.54\",\"8090\"\r\n", "CONNECT OK", 5},
	//{"AT+CIPSTART=\"TCP\",\"144.91.99.51\",\"80\"\r\n", "CONNECT OK", 5},
	{"AT+CIPSEND\r", ">", 5},
	{"GET /gateway/pinlog.php\r\n", "^Z", 20},
	{"^Z", "SEND OK", 20},
	{"", "CLOSED", 20}
};
