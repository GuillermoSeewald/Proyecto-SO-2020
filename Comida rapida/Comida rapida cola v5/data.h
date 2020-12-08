#define TYPE_QUEUE_EMPTY 1L
#define TYPE_MEAT_QUEUE_FULL 2L
#define TYPE_VEGE_QUEUE_FULL 3L
#define TYPE_ORDER_MENU 4L
#define TYPE_DELIVERED_MEAT_MENU 5L
#define TYPE_DELIVERED_VEG_MENU 6L
#define TYPE_CLEAN_TABLES 7L
#define TYPE_DIRTY_TABLES 8L

#define MENU_MEAT 1
#define MENU_VEGE 2

typedef struct message {
    long id;
    int menu;
} msg;

#define PATH "./data.h"
#define SIZE_MSG sizeof(struct message) - sizeof(long)
