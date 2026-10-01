#include "../src/runtime_adapter.h"
#include <stdio.h>
#include <string.h>
int main(void){irc_event e={0};char reply[128]={0};if(forc_runtime_init()!=0)return 1;if(!forc_runtime_word(": HEARTBEAT PING ; 10 \"HEARTBEAT\" AFTER HELLO",&e,reply,sizeof(reply)))return 2;if(strcmp(reply,"Hello from ForC")!=0)return 3;if(forc_runtime_timer_poll(9)!=0)return 4;if(forc_runtime_timer_poll(10)!=1)return 5;forc_runtime_shutdown();puts("ForC Forth named timer: PASS");return 0;}
