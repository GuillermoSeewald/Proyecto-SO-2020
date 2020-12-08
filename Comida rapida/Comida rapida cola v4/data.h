#define MAX_FOOD_TYPES 2

#define KEY_ID_MAIN_QUEUE 1

#define TYPE_MEAT_QUEUE_EMPTY 1L
#define TYPE_VEG_QUEUE_EMPTY 2L
#define TYPE_MEAT_QUEUE_FULL 3L
#define TYPE_VEG_QUEUE_FULL 4L
#define TYPE_DELIVERED_MEAT_MENU 5L
#define TYPE_DELIVERED_VEG_MENU 6L
#define TYPE_CLEAN_TABLES 7L
#define TYPE_DIRTY_TABLES 8L


#define KEY_ID_ORDERS_QUEUE 2

#define TYPE_ORDER_MEAT_MENU 1L
#define TYPE_ORDER_VEG_MENU 2L

typedef struct message {
    long id;
} msg;

#define PATH "./data.h"
#define SIZE_MSG sizeof(struct message) - sizeof(long)
