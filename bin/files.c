#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "files.h"
#include "io.h"
#include "logfile.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "networking.h"


//Created by Gareth Clarson - Unix N C - 10392634
int main(void)
{
    system("mkdir bin | mv -v ./* bin");
    int quit = 0;
    int     fd[2], nbytes;
    pid_t   pid;
    char    log_array[1024];
    char    readbuffer[80];
    log_open();
    system("mkdir Downloads");
    FILE *fp;
    config("./settings.conf");
    sharelist_check();
    friendslist_check();

    pipe(fd);

    if((pid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    if(pid == 0)
    {
        // I'm a child process!
        // Close input side of pipe because we are only sending data
        close(fd[0]);


        system("clear");
        //config("./settings.conf");
        menu();

        // Send Log event through the output side of the pipe
        write(fd[1], log_array, (strlen(log_array)+1));
        exit(0);
    }
    else
    {
        // I'm the parent because the PID returned wasn't 0
        // Close output side of pipe because we are only receiving data
        close(fd[1]);

        printf("I am an adult");

        fp=fopen("logfile.txt", "a");
        while(!quit) { // Do this until we are told to quit
            // Read Log events from the input side of the pipe
            nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
            if(nbytes)
                fprintf(fp, "this cunt gets logged: %s", readbuffer);
            nbytes = 0;
        }
    }

    return(0);
}

/* Add an individual file */
void share_add()
{
    sharelist_check();
    FILE *share; //File pointer to the sharinglist.txt
    DIR *dirPntr; // DIR pointer to the DIR structure
    logger("adding to sharelist..."); // log the event of the initialization to add a file

    struct dirent *Ent; // Entity pointer
    struct stat s_data; // Gets filesystem information

    share = fopen("./sharinglist.txt", "a"); //opens the sharinglist using the pointer in append mode
    char input_filepath[251]; //filepath that the user inputs
    char input_filename[31]; // filename that the user inputs
    char input_permission[251]; // Permission type the user inputs
    char full_path[255];  // Fultpath concatenating filepath and filename together
    char permission_char; // used for atoi changing the permission from char to an int

    printf("Please enter the filepath of the file you wish to add:\n");
    fgets(input_filepath, 251, stdin); // ask for filepath fro the keyboard
    input_filepath[strlen(input_filepath)-1] = '\0'; // delete the \n fgets uses

        stat(input_filepath, &s_data); //get the filepath info and store it in s_data

    if(S_ISDIR(s_data.st_mode)){ //uses the s_data to see if the DIR actually exists if it does, process it

        if((dirPntr = opendir(input_filepath))){ //using the DIR pointer to open the directory
            printf("%s is a correct filepath..\n", input_filepath);
            while((Ent = readdir(dirPntr)))
                puts(Ent->d_name); // print the filenames incase the user forgot the filename
            closedir(dirPntr);
        } else { printf("That filepath does not exist %s\n", input_filepath);} // if filepath does not exist output does not exist
    } else if(S_ISREG(s_data.st_mode)){
        printf("this filepath exists..");
    } else{
        printf("%s is invalid, please try again.\n", input_filepath); // reiterate the input is invalid and start again
        printf("Please enter the filepath of the file you wish to add:\n");
        fgets(input_filepath, 251, stdin);
        input_filepath[strlen(input_filepath)-1] = '\0';
        dirPntr = opendir(input_filepath);
        while((Ent = readdir(dirPntr)))

            puts(Ent->d_name);
        closedir(dirPntr);
    }


    printf("Please enter the filename you wish to add:\n");
    fgets(input_filename, 31, stdin); // get the filename from the keyboard
    input_filename[strlen(input_filename)-1] = '\0'; // delete the \n ad the end
    sprintf(full_path, "%s%s%s", input_filepath, "/", input_filename); // concatenate the filepath and the filename to create a full path
    while (access(full_path, F_OK | R_OK) == -1){ // checks to see if fullpath exists and is readable
       perror("file does not exist or cannot be read due to permissions\n"); // return error if any of the above args are FALSE
        printf("Please enter the filename you wish to add:\n"); // reinitiate adding the filename
        fgets(input_filename, 31, stdin);
        input_filename[strlen(input_filename)-1] = '\0';
        sprintf(full_path, "%s%s%s", input_filepath, "/", input_filename);
    } printf("%s exists and can be read!.. processing..\n", input_filename); // if both args are TRUE then output success!


    printf("Please enter the file permission:\n"); // initiating file permissions
    printf("[1] - Private \t [2] - Shared\n ");
    fgets(input_permission, 251, stdin); // asks for either a 1 or 2 string from the keyboard
    input_permission[strlen(input_permission)-1] = '\0'; // delete the \n
    permission_char = input_permission[0]; // assign the char to be the input
    while(permission_char < '1' || permission_char > '2' ) { // if the single char is lower than one or higher than 2
        system("clear");
        printf("%s wasn't a valid choice, please enter again.\n", input_permission); // show invalid output
        printf("Please enter the file permission:\n"); // reinitiate permission
        printf("[1] - Private \t [2] - Shared ");
        fgets(input_permission, 251, stdin);
        input_permission[strlen(input_permission)-1] = '\0';
        permission_char = input_permission[0];
    }

    fprintf(share, "%-15s %-15s %-15s\n", input_filepath, input_filename, input_permission); // put all variables containing the inputted information and format them with 15spaces after each string
    printf("\nhere are the details of the new file:\n\n");
    printf(" filepath: %s\t\n filename: %s\t\n Permission: %s\t\n", input_filepath, input_filename, input_permission); // return what was just added to the sharingslist

    logger("Added new file to sharelist", full_path); // LOG IT!!!!!
    fclose(share); // close the sharinglist
}

/* Add all files within a directory */
void share_addDir()
{
    sharelist_check();
    FILE *share;
    DIR *dirPntr;
    logger("adding dir files to sharelist...");

    struct dirent *Ent;
    struct stat s_data;

    share = fopen("./sharinglist.txt", "a");
    char input_filepath[251];
    char input_filename[31];
    char input_permission[251];
    char permission_char;

    printf("Please enter the file directory you wish to add:\n");
    fgets(input_filepath, 251, stdin);
    input_filepath[strlen(input_filepath)-1] = '\0';

    stat(input_filepath, &s_data);

    if(S_ISDIR(s_data.st_mode)){

        if((dirPntr = opendir(input_filepath))){
            printf("%s is a correct filepath..\n", input_filepath);
            while((Ent = readdir(dirPntr)) != NULL){
                if (!strcmp (Ent->d_name, "."))
                    continue;
                if (!strcmp (Ent->d_name, ".."))
                    continue;
                strncpy(input_filename, Ent->d_name, 31); // basically all same stuff as normal add except copy the string inside filepath, filename, and permissions and store it in the sharinglist
                printf("Please enter the file permission for %s:\n", input_filename); // loop for every file
                printf("[1] - Private \t [2] - Shared\n ");
                fgets(input_permission, 251, stdin);
                input_permission[strlen(input_permission)-1] = '\0';
                permission_char = input_permission[0];
                while(permission_char < '1' || permission_char > '2' ) {
                    printf("%s wasn't a valid choice, please enter again.\n", input_permission);
                    printf("Please enter the file permission for %s:\n", input_filename);
                    printf("[1] - Private \t [2] - Shared ");
                    fgets(input_permission, 251, stdin);
                    input_permission[strlen(input_permission)-1] = '\0';
                    permission_char = input_permission[0];}

                fprintf(share, "%-15s %-15s %-15s\n", input_filepath, input_filename, input_permission);
                printf("\nhere are the details of the new files:\n\n");
                printf(" filepath: %s\t\n filename: %s\t\n Permission: %s\t\n", input_filepath, input_filename, input_permission);


            }
            closedir(dirPntr); // CLOSE THE DIRECTORY FILE!!!
        } else { printf("That filepath does not exist %s\n", input_filepath);}
    } else if(S_ISREG(s_data.st_mode)){
        printf("this filepath exists..");

    } else{
        printf("%s is invalid, please try again.\n", input_filepath);
        printf("Please enter the filepath of the file you wish to add:\n");
        fgets(input_filepath, 251, stdin);
    }

    logger("Added new file to sharelist", input_filename); // LOG IT!!
    fclose(share);
}

/* Remove files within the sharinglist via directory listing */
void share_remDir()
{
    FILE *share, *share_temp;
    logger("Removing files in a dir... Processing..");
    sharelist_check();
    char input_filepath[251]; //input filepath
    char input_dir[251]; // sharinglist.txt
    int path_size = strlen(input_filepath); // get the length of the filepath and put it into an int


    share = fopen("./sharinglist.txt", "r"); // open the sharinglist in read mode
    while (fgets(input_dir, 250, share)){ // get strings from sharinglist and put it all into input_dir

        printf("%s", input_dir); // print it to the screen
    }
    rewind(share); // move the cursor to the begining of the file

    printf("\nEnter the Directory you wish to delete\n");
    fgets(input_filepath, 250, stdin); // ask for the filepath of the directory the user wants to delete from the keyboard
    input_filepath[strlen(input_filepath)-1] = '\0'; // delete that nasty \n

    share_temp = fopen("./sharetemp.txt", "w"); // create and open a temporary file called sharetemp in write mode
    while(fgets(input_dir, 250, share) !=NULL) // continue going through each line untill it hits the end of the file
    {
        if( strncmp(input_dir, input_filepath, path_size) != 0) // compare the input given with the filepath with the first part of the line which is equal to the parth size
            // and ignore it
        {
            fputs(input_dir, share_temp); // put the information that is not ignored into the share temp file
        }
    }
    fclose(share); //close
    fclose(share_temp); // close ...duh

    remove("./sharinglist.txt"); // remove the old sharinglist
    rename("./sharetemp.txt", "./sharinglist.txt"); // rename the temp to the new sharing list! magic!
    share = fopen("./sharinglist.txt", "r"); // open the new sharinglist in read mode
    printf("\n Here is the updated sharinglist:\n"); // show the updated version after the "removal"
    while (fgets(input_dir, 250, share)){

        printf("%s", input_dir);
    }
    rewind(share);

    fclose(share); // CLOSE!
    logger("Completed Removal");
}

/* Remove a specific individual file */
void share_rem()
{
    FILE *share, *share_temp;
    sharelist_check();
    logger("Removing a file... Processing");
    char file_input[251];
    char input_lineno[251];
    int del_line_no;
    int line_no = 0 -1; // line number -1 because I have headers and my first line is used up

    share = fopen("./sharinglist.txt", "r");
    while (fgets(file_input, 250, share)) {

        line_no++;
        printf("%-10d %s", line_no, file_input); // add line numbers to the left of the screen and give 10spaces before the file input
    }
    rewind(share);

    printf("\nEnter the line no. of the file you wish to delete\n"); // ask for the line number and put into the lineno variable
    line_no = 0 -1;
    fgets(input_lineno, 250, stdin);
    input_lineno[strlen(input_lineno)-1] = '\0';
    del_line_no = atoi(input_lineno); // convert the string in lineno into an int


    share_temp = fopen("./sharetemp.txt", "w"); // create and open temp in write mode
    while(fgets(file_input, 250, share) !=NULL) // read sharinglist untill the end of the file
    {

        line_no++;
        if(line_no != del_line_no) // if the line number does not equal the asked input put it into the share temp if it does, ignore it
        {
         fputs(file_input, share_temp);
        }
    }
    fclose(share); // close
    fclose(share_temp); // close

    remove("./sharinglist.txt");
    rename("./sharetemp.txt", "./sharinglist.txt"); // rename and reload the sharinglist
    share = fopen("./sharinglist.txt", "r");
    printf("\n Here is the updates sharinglist:\n"); // show the changes to ensure it has been made
    while (fgets(file_input, 250, share)){

        printf("%s", file_input);
    }
    rewind(share);
    fclose(share);
    logger("Deleted a file"); // logggggg ittt
}

/* View a file within the sharinglist */
void share_view()
{
    FILE *share;
    DIR *dirPntr;
    sharelist_check();
    logger("Viewing file... Processing..");
    share = fopen("./sharinglist.txt", "r");
    char buff[1024];
    char input_filepath[251];
                                    // Basically using the same dirent stuff as above to figure out what the user wants to read, then use the readfile function made to read the file..
    struct dirent *dir_Ent;
    struct stat s_data;

    while (fgets(buff, 1023, share)!=NULL) {
        if (buff[0] != '#')//if there is a '#' in the file it should be ignored
        {
            printf("%s\n", buff);    //print the contents of the buffer/file
        }
    }

    printf("Please enter the filepath and filename of the file you wish to view:\n");
    printf("NOTE: if you want to view a file that has a filepath of './' just the filename is needed.. \n\n");
    fgets(input_filepath, 250, stdin);
    input_filepath[strlen(input_filepath)-1] = '\0';

    stat(input_filepath, &s_data);

    if(S_ISDIR(s_data.st_mode)){

        if((dirPntr = opendir(input_filepath))){
            printf("%s is a correct filepath..\n", input_filepath);
            while((dir_Ent = readdir(dirPntr)))
            closedir(dirPntr);
        } else { printf("That filepath does not exist %s\n", input_filepath);}
    } else if(S_ISREG(s_data.st_mode)){
        printf("this filepath exists..");
    } else{
        printf("%s is invalid, please try again.\n", input_filepath);
        fgets(input_filepath, 251, stdin);
        input_filepath[strlen(input_filepath)-1] = '\0';
    }

    system("clear");
    printf("Reading file....");
    readFile(input_filepath); // this is the readfile funciton

    logger("Read: %s", input_filepath); // LOG IT
    fclose(share);
}

void friend_add()
{
    friendslist_check();
    FILE *friendlst; //File pointer to the sharinglist.txt
    logger("adding friend..."); // log the event of the initialization to add a file

    friendlst = fopen("./friendlist.txt", "a"); //opens the friendslist using the pointer in append mode
    char input_username[251]; //
    char input_ip[16]; //
    char input_hostname[251]; //
    char input_port[255];  //
    char fullprofile[255];
    char choice[3];
    int choose;

    printf("Please enter the username of the friend you wish to add:\n");
    fgets(input_username, 251, stdin); //
    input_username[strlen(input_username)-1] = '\0'; // delete the \n fgets uses

do {

    printf("Would you like to add an [1] IP address or [2] hostname?\n");
    fgets(choice, 3, stdin);
    choice[strlen(choice)-1] = '\0';
    choose = atoi(choice);

        if (choose ==1){
            printf("Please enter the IP address of the friend:\n");
            fgets(input_ip, 16, stdin);
            input_ip[strlen(input_ip)-1] = '\0';

            printf("Please enter the port number to assign to the friend:\n"); // initiating file permissions
            fgets(input_port, 255, stdin);
            input_port[strlen(input_port)-1] = '\0';
            int port = atoi(input_port);


            fprintf(friendlst, "%-15s %-15s %-25i\n", input_username, input_ip, port); // put all variables containing the inputted information and format them with 15spaces after each string
            printf("\nhere are the details of the new friend:\n\n");
            printf(" Username: %s\t\n IP: %s\t\n Port: %i\t\n", input_username, input_ip, port); // return what was just added to the sharingslist
            sprintf(fullprofile, "%-10s %-10s %-10s\n\n", input_username, input_ip, input_port); // concatenate the filepath and the filename to create a full path
            break;
        }
        else if (choose ==2){
            printf("Please enter the hostname of the friend");
            fgets(input_hostname, 251, stdin);
            input_hostname[strlen(input_hostname)-1] = '\0';

            printf("Please enter the port number to assign to the friend:\n"); // initiating file permissions
            fgets(input_port, 255, stdin);
            input_port[strlen(input_port)-1] = '\0';
            int port = atoi(input_port);

            fprintf(friendlst, "%-15s %-15s %-25i\n", input_username, input_hostname, port); // put all variables containing the inputted information and format them with 15spaces after each string
            printf("\nhere are the details of the new friend:\n\n");
            printf(" Username: %s\t\n Hostname: %s\t\n Port: %i\t\n", input_username, input_hostname, port); // return what was just added to the sharingslist
            sprintf(fullprofile, "%-10s %-10s %-10s\n\n", input_username, input_hostname, input_port); // concatenate the filepath and the filename to create a full path
            break;

        }
        else{printf("Incorrect please choose again\n\n");}
    } while (choose != 3);

    logger("Added new friend to friendslist", fullprofile); // LOG IT!!!!!
    fclose(friendlst); // close the sharinglist

    // Use this to fix the int problem http://stackoverflow.com/questions/32573382/how-to-take-ints-from-user-using-fgets-in-c
}

void friend_rem()
{
    FILE *friendlist, *friendlist_temp;
    friendslist_check();
    logger("Removing a friend... Processing");
    char file_input[251];
    char input_lineno[251];
    int del_line_no;
    int line_no = 0 -1; // line number -1 because I have headers and my first line is used up

    friendlist = fopen("./friendlist.txt", "r");
    while (fgets(file_input, 250, friendlist)) {

        line_no++;
        printf("%-10d %s", line_no, file_input); // add line numbers to the left of the screen and give 10spaces before the file input
    }
    rewind(friendlist);

    printf("\nEnter the line no. of the friend you wish to delete\n"); // ask for the line number and put into the lineno variable
    line_no = 0 -1;
    fgets(input_lineno, 250, stdin);
    input_lineno[strlen(input_lineno)-1] = '\0';
    del_line_no = atoi(input_lineno); // convert the string in lineno into an int


    friendlist_temp = fopen("./friend_temp.txt", "w"); // create and open temp in write mode
    while(fgets(file_input, 250, friendlist) !=NULL) // read sharinglist untill the end of the file
    {

        line_no++;
        if(line_no != del_line_no) // if the line number does not equal the asked input put it into the share temp if it does, ignore it
        {
            fputs(file_input, friendlist_temp);
        }
    }

    fclose(friendlist); // close
    fclose(friendlist_temp); // close

    remove("./friendlist.txt");
    rename("./friend_temp.txt", "./friendlist.txt"); // rename and reload the sharinglist
    friendlist = fopen("./friendlist.txt", "r");

    fgets(file_input, 250, friendlist);

    printf("Congratulations, here is the updated friendslist!: \n%s", file_input);
    fclose(friendlist);
    logger("Deleted a file"); // logggggg ittt
}

int find_usr(char *usrName)
{
    FILE *friend;
    char fileFriend [64];
    char buffer[255];
    const char s[3] = " :";
    char* token;
    int line = 1-1;

    if (access("./friendsList.txt", F_OK | W_OK) !=-1)
    {
        friend = fopen("./friendsList.txt", "r");//open in read mode

        while (fgets(buffer, 254, friend) != NULL)
        {
           // buffer[strlen(buffer)-1] = '\0';
            line++;
            token = strtok(buffer, s);
            //        printf("token 1 is: %s\n", token);
            token = strtok(NULL, s);
//            printf("token 2 is: %s\n", token);//username!
//            inputHelper(token);
            //        printf("Token is still the user name?? >>> %s\n\n", token);
            strcpy(fileFriend, token);
//            inputHelper(fileFriend); // null vyte removal
            printf("Friend %d)  %s\n",line, fileFriend);
            if (strcmp(usrName, fileFriend) ==0)//Match found
            {
                printf("Match found!\n Processing.....\n");
                break;//match found, break out of while loop
            }
        }
        if (strcmp(fileFriend, usrName)!=0)//No match found - Add fName to file
        {
            printf("That friend does not exist please try again.\n");
            return -1;
        }
    }
    fclose(friend);
    return 0;

}


/* readfile funciton that works with the share_view funciton */
int readFile(char* filename)
{
    FILE *filePntr;
    char buff[1024];
    //char log[255];


    filePntr = fopen(filename, "r");//open the file name passed to the function in read mode
        //sprintf(log, "File %s Printed To Screen\n", filename);
        //logger(log);//log file printed to screen
        //printf("\nReading contents of %s\n\n\n", filename);
        while (fgets(buff, 1023, filePntr)!=NULL)
        {
            if (buff[0] != '#')//if there is a '#' in the file it should be ignored
            {
                printf("%s\n", buff);    //print the contents of the buffer/file
            }
        }
    fclose(filePntr);//close the file!
    return 0;
}

/* check to see if the sharinglist actually exists! */
int sharelist_check()
{
    FILE *share;
    if (access("./sharinglist.txt", F_OK | R_OK) !=-1) // Does it exist, and is it readable?
    {
        share = fopen("./sharinglist.txt", "r");    // If it is open it in read mode
        printf("sharinglist exists... \nReading contents....\n");
        logger("Reading contents of sharinglist");

    }
    else
    {
        share = fopen("./sharinglist.txt", "w"); // if not create it with the filepath file name and permission headers! make it look pertty
        fprintf(share, "%-15s %-15s %-15s\n", "[Filepath]", "[Filename]", "[Permission]");
        printf("sharinglist.txt created..\n");
        logger("sharinglist created"); // LOOOOGGG IT
    }
    fclose(share);
    return 0;
}

/*check to see if the friendslist actually exists */
int friendslist_check()
{
    FILE *share;
    if (access("./friendlist.txt", F_OK | R_OK) !=-1) // Does it exist, and is it readable?
    {
        share = fopen("./friendlist.txt", "r");    // If it is open it in read mode
        printf("friendlist exists... \nReading contents....\n");
        logger("Reading contents of friendslist");

    }
    else
    {
        share = fopen("./friendlist.txt", "w"); // if not create it with the filepath file name and permission headers! make it look pertty
        fprintf(share, "%-15s %-15s %-15s\n", "[Username]", "[IP Address/Hostname]", "[Port]");
        printf("Friendlist.txt created..\n");
        logger("friendlist created"); // LOOOOGGG IT
    }
    fclose(share);
    return 0;
}

/* Configuration settings */
void config(const char *filename)
{
    FILE *set_conf;
    char input_str[251], *label, *value; // create the input string variable and create a label and value pointer to point to it

    config_st settings = {"vetrix\t", "10.1.1.8\t", 9001, "\t./logfile.txt"}; // establish default settings along with the struct used in .h
    if (access("./settings.conf", F_OK | R_OK) !=-1) { // check to see if file actually exists and is readable
        logger("Reading existing settings.conf..");
        if((set_conf=fopen("./settings.conf", "r"))) // if it does read it,
            while(fgets(input_str, 250, set_conf))
            {
                if(input_str[0] != '#') // ignore anything with a # in it
                {
                    label = input_str; // split the string into two!
                    value = input_str;
                    while(*value != ':' && *value != '\0') // continue incrementing value until it hits :
                        value++;
                    if(*value != '\0')
                    {
                        *value = '\0';
                        value ++;
                        //get length of the value
                        int length = strlen(value);
                        //delete the newline at the end
                        value[length-1] = '\0';
                        save_setting(label, value, &settings); // save settings to the struct
                    }
                }
            }

        printf("Settings File already exists...\n\n");
        printf("here are the current settings:\n\n");
        printf(" User: %s\t\n IP: %s\t\n Port: %d\t\n Logfile: %s\n", settings.User, settings.IPAddress, settings.Port, settings.Logfile);
        logger("Completed!");
        fclose(set_conf);

    } else {

        logger("Creating settings.conf..."); // if it does not exist, create it with the config_st settings as a default then open it
        printf("Settings.conf does not exist..\n\n");
        printf("Creating settings.conf...\n\n");

            set_conf = fopen("./settings.conf", "w+");
            fputs(settings.User, set_conf);
            fputs(settings.IPAddress, set_conf);
            fprintf(set_conf, "%d", settings.Port);
            fputs(settings.Logfile, set_conf);
            printf("here are the current settings:\n\n");
            printf(" User: %s\t\n IP: %s\t\n Port: %d\t\n Logfile: %s\n", settings.User, settings.IPAddress, settings.Port, settings.Logfile);
            logger("Completed!");
            fclose(set_conf);

    }
}

/* works in unison with config funciton to save the settings to the config struct */
int save_setting(char* setting_name,char* setting_data, config_st* config)
{

    if(strcmp(setting_name, "Name") == 0)
    {
        strcpy(config->User, setting_data);
    }
    else if (strcmp(setting_name, "IP") == 0)
    {
        strcpy(config->IPAddress, setting_data);
    }
    else if (strcmp(setting_name, "Port") == 0)
    {
        config->Port = atoi(setting_data);
    }
    else if (strcmp(setting_name, "Log") == 0)
    {
        strcpy(config->Logfile, setting_data);
    }

    return 0;
}

//Created by Gareth Clarson - Unix N C - 10392634