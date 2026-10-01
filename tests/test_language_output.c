#include "../src/runtime_adapter.h"
#include <stdio.h>
#include <string.h>
static char line[1024]; static int sink(const char*s,void*u){(void)u;snprintf(line,sizeof(line),"%s",s);return 1;}
int main(void){irc_output_sink out={sink,NULL};if(forc_runtime_init()!=0)return 1;forc_runtime_set_output_sink(&out);if(!forc_runtime_word("\"#ploos\" \"hello\" SAY",NULL,line,sizeof(line)))return 2;if(strcmp(line,"PRIVMSG #ploos :hello\r\n"))return 3;puts("ForC language output bridge: PASS");forc_runtime_shutdown();return 0;}