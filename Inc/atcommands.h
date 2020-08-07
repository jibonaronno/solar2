
typedef struct {
    const char* cmd;
    //void (*func)();
    const char* ret;
    int timeout;
} atcmd_t;

#define ATCMD_FLOW_UNIT(cmd, ret, timeout) {#cmd, #ret, timeout}
