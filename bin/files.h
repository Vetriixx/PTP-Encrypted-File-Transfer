typedef struct Config config_st;

struct Config
{
    char User[31];
    char IPAddress[16];
    int Port;
    char Logfile[251];
};

typedef struct Sharelist share_t;

struct Sharelist
{
    char Filepath[251];
    char Filename[31];
    char Permission[251];
};


typedef struct Friendslist friends_st;

struct Friendslist
{
    char Username[31];
    char IPAddress[16];
    int Port;
    char hostname[251];
};


void print_settings(config_st data);
void config();
int sharelist_check();
int friendslist_check();
void friend_add();
void friend_rem();
void share_view();
int find_usr(char *usrName);
void share_add();
void share_addDir();
int readFile();
void share_rem();
void share_remDir();




