#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "datecommand.c"
#include "lscommand.c"
#include "catcommand.c"
#include "rm.c"
#include "mkdir.c"


#define MAX_LINE 10000


int func_cd(char **args);
int func_exit(char **args);
int func_pwd(char **args);
int func_ls(char **args);
int func_date(char **args);
int func_cat(char **args);
int func_rm(char **args);
int func_mkdir(char **args);
void func_history(char **args);
char *hist[1024];
int run_func;
int val=0;
int num=0;


int func_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("error in executing command cd");
    }
  }
  return 1;
}

int func_echo(char **args)
{
    int i=1;
    if (args[1]==NULL)
    {
        printf("\n");
        return 1;
    }
    else if(!strcmp(args[1], "-n"))
    {
        i=2;
        while(args[i]!=NULL)
        {
            printf ("%s ", args[i]);
            i++;
            fflush(stdout);
        }
    }

    else if(!strcmp(args[1], "-E"))
    {
        i=2;
        while(args[i]!=NULL)
         {
             if(!strcmp(args[i], "\\")==0)
             {
                printf ("%s ", args[i]);
             }
             else
             {
                printf (" ");
             }
             i++;
            fflush(stdout);
         }printf("\n");
    }
    else
    {
        while(args[i]!=NULL)
        {
            printf ("%s ", args[i]);
            i++;
            fflush(stdout);
        }
        printf("\n");
    }


}

int func_exit(char **args)
{
  run_func=0;
  return 0;
}

int func_pwd(char **args)
{
	char cwd[1024];
	if (args[1]!= NULL)
        {
            if (!strcmp(args[1], "-L")|| !strcmp(args[1], "-P"))
            
            {
	            if (getcwd(cwd, sizeof(cwd))==NULL)
				{
					perror("Error: in getting current working directory\n");
				}
				else
				{
					printf("%s\n", cwd);
				}
            }
            else
            {
            	printf("Invalid option \n");
            }
        }
    else
    {
	    if (getcwd(cwd, sizeof(cwd))==NULL)
		{
			printf("Error in getting current working directory\n");
		}
		else
		{
			printf("Current working dir is: %s\n", cwd);
		}
    }

	return 1;
}

void create_history(char *myinput, int val) {
    hist[val] = strdup(myinput);
}

void func_history(char **args) {
    int i = 0;
    int key;
  

    while (1) 
    {
        if (hist[i] == NULL) 
        {
            break;
        }
        
        else if (args[1]!= NULL)
        {
            if (!strcmp(args[1], "-c"))
            
            {
                memset(hist, 0, sizeof hist);
                num=0;
                break;
            }

            else if (!strcmp(args[1], "-n"))
            
            {
                // memset(hist, 0, sizeof hist);
                int temp=num;
                if (hist[temp] == NULL) 
                {
                    break;
                }
                
                printf("%s\n", hist[num]);
                i++;
                temp++;
                num++;

            }
        } 
        
        else 
        {
            printf("%s\n", hist[i]);
            i++;
            num++;
        }
        if (i >= 1024) 
        {
            perror("history limit error");
        }
    }
}


int main()
{
	run_func=1;
    char *newinput;
    char input;
    char str[100];
    while(run_func)
    {
        char cwd[1024];
    	getcwd(cwd, sizeof(cwd));
    	char text[MAX_LINE];
    	char *args[MAX_LINE/2 +1];
    	printf("ruhmamehek@ruhma-desktop:%s/>>>", cwd );

    	fgets(text, MAX_LINE, stdin);
    	text[strlen(text)-1]='\0';
        strcpy(str, text);
        create_history(str, val);
        val++;

    	args[0]=strtok(text, " ");
    	int i=0;
        if (args[0]==NULL)
        {
            printf("no command found\n");
            fflush(stdout);
            continue;
        }
        if (!args[0], NULL) { 
            
            perror("no command");
            continue;
        }
    	while(args[i] != NULL)
        {
            args[++i]=strtok(NULL, " ");
        }
        if(!strcmp(args[0], "echo"))
        {
            func_echo(args);
        }
        if(!strcmp(args[0], "ls"))
        {
            func_ls(args);
        }
        if (!strcmp(args[0], "cd"))
        {
            func_cd(args);
        }
        if (!strcmp(args[0], "pwd"))
        {
            func_pwd(args);
        }
        if (!strcmp(args[0], "exit"))
        {
            func_exit(args);
        }
        if (!strcmp(args[0], "date"))
        {
            func_date(args);
        }
        if (!strcmp(args[0], "cat"))
        {
            func_cat(args);
        }
        if (!strcmp(args[0], "rm"))
        {
            func_rm(args);
        }
        if (!strcmp(args[0], "mkdir"))
        {
            func_mkdir(args);
        }
        if (!strcmp(args[0], "history"))
        {
            func_history(args);
        }

        else
        {
            continue;
        }

    }


}
