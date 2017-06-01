#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "files.h"
#include "io.h"
#include "logfile.h"
#include "networking.h"
#define FALSE 0
#define TRUE !FALSE


int menu(void)
    {
        char menu_choice;
        char User[70];
        do { // do the following untill it returns 0 or it hits an error
        printf("------------------ Zoopmydooper File Transfer Program --------------------\n\n");
        printf("[1] - Add an individual file \n");
        printf("[2] - Add file(s) from a directory \n");
        printf("[3] - Remove an individual file \n");
        printf("[4] - Remove file(s) from a directory \n");
        printf("[5] - View Friendslist \n");
        printf("[6] - Add a friend \n");
        printf("[7] - Remove a friend\n");
        printf("[8] - Connect to a friend\n");
        printf("[9] - Exit Program\n\n");

            menu_choice = getchar();
            while(getchar() != '\n');
            switch (menu_choice) { // use a switch case menu to establish a decent menu
                case '1':
                    system("clear"); // ask os to clear screen (asthetics)
                    share_add(); // function use, basically goes untill you quit
                    break;
                case '2':
                    system("clear");
                    share_addDir();
                    break;
                case '3':
                    system("clear");
                    share_rem();
                    break;
                case '4':
                    share_remDir();
                    break;
                case '5':
                    system("clear");
                    share_view();
                    break;
                case '6':
                    system("clear");
                    friend_add();
                    break;
                case '7':
                    system("clear");
                    friend_rem();
                    break;
                case '8':
                    system("clear");
                    printf("Enter a name to connect to:\n");
                    server_handler();
                case '9':
                    printf("Exiting.... Goodbye.\n\n"); // gives the user to quit
                    printf("Created by Gareth Clarson - Unix N C - 10392634");
                    logger("Terminated Program"); // logs it.......
                    return 0;
                default:
                    system("clear");
                    printf("Error! Try again please\n");



            }
        } while(menu_choice !='9');
        return 0;
    }


//Created by Gareth Clarson - Unix N C - 10392634

