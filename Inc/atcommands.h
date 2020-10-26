
typedef struct {
    char cmd[100];
    //void (*func)();
    const char *ret;
    int timeout;
} atcmd_t;

#define ATCMD_FLOW_UNIT(cmd, ret, timeout) {#cmd, #ret, timeout}

void atcommandSendList();
