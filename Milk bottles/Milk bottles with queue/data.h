struct message {
    long id;
};

typedef struct message msg;

#define SIZE_MSG sizeof(msg) - sizeof(long)
#define PATH "./data.h"
#define TYPE_FULL_FRIDGE 1L
#define TYPE_NOBODY_AT_STORE 2L
#define TYPE_CLOSED_FRIDGE 3L
#define BOTTLES 10
