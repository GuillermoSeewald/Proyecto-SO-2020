#define TYPE_MEAT_QUEUE_EMPTY 1L
#define TYPE_VEG_QUEUE_EMPTY 2L
#define TYPE_MEAT_QUEUE_FULL 5L
#define TYPE_VEG_QUEUE_FULL 6L

#define MAX_FOOD_TYPES 2
#define TYPE_ORDER_MEAT_MENU 4L
#define TYPE_ORDER_VEG_MENU 3L
#define TYPE_DELIVERED_MEAT_MENU 7L
#define TYPE_DELIVERED_VEG_MENU 8L

#define TYPE_CLEAN_TABLES 9L
#define TYPE_DIRTY_TABLES 10L

typedef struct message {
    long id;
} msg;

#define PATH "./data.h"
#define SIZE_MSG sizeof(struct message) - sizeof(long)
