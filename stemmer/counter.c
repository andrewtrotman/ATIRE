#include <stdio.h>

static int counter = 0;

void inc_counter() {counter++;}
void dec_counter() {counter --;}
void print_counter() {if(counter)printf("C:%d\n", counter);counter = 0;}
