#include "../src/runtime_adapter.h"
#include <stdio.h>
static int hits;
static int cb(bot_timer_id id,void *u){(void)id;(void)u;hits++;return 0;}
int main(void){if(forc_runtime_init()!=0)return 1;if(forc_runtime_timer_after(100,cb,0)==0)return 2;if(forc_runtime_timer_every(50,cb,0)==0)return 3;if(forc_runtime_timer_poll(49)!=0)return 4;if(forc_runtime_timer_poll(50)!=1||hits!=1)return 5;if(forc_runtime_timer_poll(100)!=2||hits!=3)return 6;forc_runtime_shutdown();puts("ForC runtime timers: PASS");return 0;}