#include "../src/runtime_adapter.h"
#include <stdio.h>
static int fired;
static int cb(bot_timer_id id,void *u){(void)id;(void)u;fired++;return 0;}
int main(void){irc_event e={0};char reply[128]={0};if(forc_runtime_init()!=0)return 1;if(!forc_runtime_word(": TICK PING ; 20 \"TICK\" EVERY",&e,reply,sizeof(reply)))return 2;if(forc_runtime_timer_poll(19)!=0)return 3;if(forc_runtime_timer_poll(20)!=1)return 4;if(forc_runtime_timer_poll(40)!=1)return 5;bot_timer_id id=forc_runtime_timer_after(100,cb,NULL);if(!id)return 6;if(!forc_runtime_timer_cancel(id))return 7;if(forc_runtime_timer_poll(200)!=0||fired!=0)return 8;forc_runtime_shutdown();puts("ForC Forth timer lifecycle: PASS");return 0;}