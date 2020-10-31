struct message {
    long id;
};

typedef struct message msg;

#define SIZE_MSG sizeof(msg) - sizeof(long)
#define PATH "./data.h"
#define TYPE_HELADERA_LLENA 1L
#define TYPE_NADIE_COMPRANDO 2L
#define TYPE_HELADERA_CERRADA 3L
#define CANTIDAD_BOTELLAS 10
