#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//Type definitions
typedef union {
    char  *s;
    char   c;
    float  f;
} arg_t;

typedef struct {
    const char* name;
    void (*func)(arg_t*);
    const char* args;
    const char* doc;
} cmd_t;

#define CMDS		8
#define MK_CMD(x) void cmd_ ## x (arg_t*)
#define CMD(func, params, help) {#func, cmd_ ## func, params, help}

MK_CMD(GET_CGDCONT);

arg_t *args_parse(const char *s);

cmd_t dsp_table[CMDS] ={
    CMD(GET_CGDCONT,"","Get CGDCONT")
};


void cmd_GET_CGDCONT(arg_t *args)
{
	
}
