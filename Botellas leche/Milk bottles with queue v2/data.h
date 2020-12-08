struct message {
    long id;
};

typedef struct message msg;

#define SIZE_MSG sizeof(msg) - sizeof(long)
#define PATH "./data.h"
#define TYPE_HELADERA 1L
#define TYPE_BOTELLAS 2L
#define TYPE_MUTEX_COMPRAR 3L
#define TYPE_COMPRAR 4L
#define CANTIDAD_BOTELLAS 10
