#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

typedef double Elem_t;
#define ELEM_T "%lg"

int CAPACITY = 4;
const Elem_t POISON = - 1073676287;
const int CAN_V = 0xBEDA;
enum errcode_t {OK = 0, CAN_ERROR = 1, SIZE_ERROR = 2, DATA_ERROR = 3, HASH_ERROR = 4, ERRCODE_ERROR = 5};

#define CheckStack(stk); if(StackOk(stk))StackDump(stk);
#define can1 *(int*)stk->data
#define can2 *(int*)(stk->data + sizeof(stk->can0) + CAPACITY * sizeof(Elem_t))
#define values(i)  (*(Elem_t*)(stk->data + sizeof(stk->can0) + (i) * sizeof(Elem_t)) )

struct stack_t {
    int can0;
    int size;
    errcode_t errcode;
    long int hash;
    char* data;
};

long int GavHash(stack_t* stk) {
    
    assert(stk != NULL);

    const int code = 0xBEDAB0DA;
    long int hash = 0;

    hash += (long int)stk->can0 * 1 * code + stk->size * 2 * code + stk->errcode * 3 * code;
    hash += (long int)can1 * 4 * code + (long int)can2 * (CAPACITY + 5);

    for(int i = 0; i < CAPACITY; i++)
        hash += (long int) values(i) * (i + 5) * code;

    return hash;
}

int StackOk(stack_t* stk) {

    assert(stk != NULL);

    if (stk->can0 != CAN_V || can1 != CAN_V || can2 != CAN_V) {
        stk->errcode = CAN_ERROR;
        return CAN_ERROR;
    }

    if (stk->size < 0 || stk->size > CAPACITY) {
        stk->errcode = SIZE_ERROR;
        return SIZE_ERROR;
    }

    for(int i = stk->size; i < CAPACITY; i++) {
        if (values(i) != POISON) {
            stk->errcode = DATA_ERROR;
            return DATA_ERROR;
        }
    }

    if (stk->errcode < 0 || stk->errcode > 4)
        return ERRCODE_ERROR;

    if (stk->hash != GavHash(stk)) {
        stk->errcode = HASH_ERROR;
        return HASH_ERROR;
    }

    return 0;
}

bool ErrorPrint(errcode_t errcode) {

    switch (errcode) {

        case CAN_ERROR:
            printf("(CANARY_ERROR)");
            break;

        case SIZE_ERROR:
            printf("(SIZE_ERROR)");
            break;

        case DATA_ERROR:
            printf("(DATA_ERROR)");
            break;

        case HASH_ERROR:
            printf("(HASH_ERROR)");
            break;

        case 0:
            break;

        default:
            printf ("(ERRCODE_ERROR)");
            break;
    }

    return 0;
}

bool DataStkPrint(const stack_t* stk) {

    if (stk->size >= 0 && stk->size <= CAPACITY) {
        for(int i = 0; i < stk->size; i++)
            printf("\t\t *[%d] = "ELEM_T"\n", i + 1, values(i));

        for(int i = stk->size; i < CAPACITY; i++) {

            if (values(i) == POISON)
                printf("\t\t  [%d] = "ELEM_T" (POISON?) \n", i + 1, values(i));
            else
                printf("\t\t  [%d] = "ELEM_T"\n", i + 1, values(i));
        }
    }

    else {
        for(int i = 0; i < CAPACITY; i++)
            printf("\t\t  [%d] = "ELEM_T"\n", i + 1, values(i));
    }

    return 0;
}

bool StackDump(const stack_t* stk) {

    assert(stk != NULL);

    printf("\n stack_t [%p] ", stk);
    (stk->errcode == 0) ? printf("(ok)\n") : printf("(ERROR)\n");

    printf("\t {\n"
           "\t can0 = %d\n"
           "\t size = %d\n"
           "\t errcode = %d ", stk->can0, stk->size, stk->errcode);

    ErrorPrint(stk->errcode);

    printf("\n\t hash = %ld\n"
           "\t can1 = %d\n"
           "\t data[%d] [%p]\n"
           "\t\t {\n", stk->hash, can1, CAPACITY, stk->data + sizeof(stk->can0));

    DataStkPrint(stk);

    printf("\t\t }\n"
           "\t can2 = %d\n"
           "\t }\n\n", can2);

    //abort();
    return 0;
}

bool StackInit(stack_t* stk) {

    assert(stk != NULL);

    stk->data = (char*)calloc(CAPACITY * sizeof(Elem_t) + 2 * sizeof(stk->can0), 1);
    assert(stk->data != NULL);

    can1 = CAN_V;
    can2 = CAN_V;

    for(int i = 0; i < CAPACITY; i++)
         values(i) = POISON;

    stk->size = 0;
    stk->can0 = CAN_V;
    stk->errcode = OK;
    stk->hash = GavHash(stk);

    CheckStack(stk);
    return 0;
}

bool IncreaseData(stack_t* stk) {

    assert(stk != NULL);
    CheckStack(stk);

    char* reservedata = (char*) realloc(stk->data, sizeof(Elem_t) * (CAPACITY *= 2) + 2 * sizeof(stk->can0));
    stk->data = (reservedata != NULL) ? reservedata : stk->data;

    if (reservedata == NULL) {
        CAPACITY /= 2;
        printf("Not able to increase data");
        return 1;
    }

    for(int i = stk->size; i < CAPACITY; i++)
        values(i) = POISON;

    can2 = CAN_V;
    stk->hash = GavHash(stk);

    CheckStack(stk);
    return 0;
}

bool StackPush(stack_t* stk, Elem_t elem) {

    assert(stk!= NULL);
    CheckStack(stk);
    //CHECKSTACK
    if (stk->size == CAPACITY && IncreaseData(stk) == 1) {
        return 1;
    }

    values(stk->size++) = elem;
    //VALUES
    stk->hash = GavHash(stk);

    CheckStack(stk);
    return 0;
}

bool DecreaseData(stack_t* stk) {

    assert(stk != NULL);
    CheckStack(stk);

    char* reservedata = (char*) realloc(stk->data, sizeof(Elem_t) * (CAPACITY /= 2) + 2 * sizeof(stk->can0));
    stk->data = (reservedata != NULL) ? reservedata : stk->data;

    if (reservedata == NULL) {
        CAPACITY *= 2;
        return 1;
    }

    can2 = CAN_V;
    stk->hash = GavHash(stk);

    CheckStack(stk);
    return 0;

}

Elem_t StackPop(stack_t* stk) {

    assert(stk!= NULL);
    CheckStack(stk);

    if (stk->size < 1) {
        printf("No more data");
        return 0;
    }

    Elem_t value = values(--stk->size);
    values(stk->size) = POISON;
    stk->hash = GavHash(stk);

    if (stk->size == CAPACITY / 2 - 4)
        DecreaseData(stk);

    CheckStack(stk);
    return value;
}

bool StackKill(stack_t* stk) {

    assert(stk != NULL);
    CheckStack(stk);

    free(stk->data);
    stk->can0 = 0;
    stk->size = 0;
    stk->errcode = OK;
    stk->hash = 0;
    stk->data = NULL;

    return 0;
}
