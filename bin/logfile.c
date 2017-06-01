#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


void log_open()

{
    FILE *logfile;
        logfile = fopen("logfile.txt", "w");
        fprintf(logfile, "========================= Welcome to the logfile! ============================\n\n"); // create and open the logfile to create the heading!
        fclose(logfile);
}

/* used to log anything an everything */
void logger(char *message)
{
    FILE *logfile;
    time_t current_time;

    //Get the current time
    time(&current_time);

   logfile = fopen("logfile.txt", "a");
    //turn it into a string and format to be on one line
    char* timestr = ctime(&current_time);
    timestr[strlen(timestr)-1] = '\0';
    //put it into an event string
    fprintf(logfile, "%s\tEvent:\t%s\n", timestr, message);


    fclose(logfile); // CLOOOOOOOOOSE
}