#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include "stack.h"

//assert!!
//no global!!
//functions!!

//typeof???

const int COUNT = 1;

void CapacityTest(stack_t* stk) {

    printf("BIG CAPACITY TEST: \n");

    StackInit(stk);

    for(int i = 0; i < COUNT; i++) {
        if (!(i % 1000))
            printf("%d\n\n", i);
        StackPush(stk, i);
    }

    StackDump(stk);

    for(int i = 0; i < COUNT; i++)
        if (StackPop(stk) != COUNT - 1 - i)
            printf("ERROR");

    StackKill(stk);
}

void CanTest (stack_t* stk) {

    printf("CAN ERROR TEST: \n");

    StackInit(stk);
    *((int*)stk) = 0;

    StackKill(stk);
}

void SizeTest (stack_t* stk) {

    printf("SIZE ERROR TEST: \n");

    StackInit(stk);
    *((int*)stk + 1) = -11111;

    StackKill(stk);
}

void DataTest (stack_t* stk) {

    printf("DATA ERROR TEST: \n");

    StackInit(stk);
    values(0) = -11111;

    StackKill(stk);
}

void ErrcodeTest (stack_t* stk) {

    printf("ERRCODE ERROR TEST: \n");

    StackInit(stk);
    *((int*)stk + 2) = -1223;
    stk->hash = GavHash(stk);

    StackKill(stk);
}

void HashTest (stack_t* stk) {

    printf("HASH ERROR TEST: \n");

    StackInit(stk);
    *((int*)stk + 3) = 0;

    StackKill(stk);
}

void ReallocTest (stack_t* stk) {

    printf("REALLOC TEST: \n");

    StackInit(stk);

    for(int i = 2; i < 10; i++) {

        for(int j = 0; j < 2 << i; j++)
            StackPush(stk, i + 1);

        for(int j = 0; j < 2 << i; j++)
            if (StackPop(stk) != i + 1)
                printf("ERROR");
    }

    StackDump(stk);
    StackKill(stk);
}

int main() {

    stack_t stk = {};

    CapacityTest(&stk);

    CanTest(&stk);

    SizeTest(&stk);

    DataTest(&stk);

    HashTest(&stk);

    ErrcodeTest(&stk);

    ReallocTest(&stk);

    printf("HASH VULNERABILITY: \n");
    StackInit(&stk);
    StackPush(&stk, 2.71828182846);
    *(Elem_t*)(    (*(char**)((char*)&stk + 3 * sizeof(int) + sizeof(long int)))   + sizeof(int)) = 2;
    CheckStack(&stk);
    StackDump(&stk);
    StackKill(&stk);

    return 0;
}
