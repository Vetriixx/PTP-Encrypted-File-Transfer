#include <stdio.h>
#include <string.h>
#include "files.h"
#include "io.h"

int main()
{
	config_t settings = {"noob", "192.168.0.1", 1337, "./copier_log.txt"};
	FILE *fp;
	char input_string[251], *label, *value;

	printf("defaults:\n");
	printf("%s\t%s\t%d\t%s\n", settings.User, settings.IPAddress, settings.Port, settings.Logfile);

	if((fp=fopen(CONFIGFILE, "r")))
	{
		while(fgets(input_string, 250, fp))
		{
			if(input_string[0] != '#')
			{	
				label = input_string;
				value = input_string;
				while(*value != ':' && *value != '\0')
					value++;
				if(*value != '\0')
				{
					*value = '\0';
					value ++;	
					//get length of the value
					int length = strlen(value);
					//delete the newline at the end
					value[length-1] = '\0';
					save_setting(label, value, &settings);
				}			
			}
		}
	}
	else
	{
		printf("The file %s cannot be opened!\n",CONFIGFILE);
	}
	
	print_settings(settings);
	return 0;
}

void print_settings(config_t data)
{
	printf("here we are.. the values:\n");
	printf("%s\t%s\t%d\t%s\n", data.User, data.IPAddress, data.Port, data.Logfile);

}
