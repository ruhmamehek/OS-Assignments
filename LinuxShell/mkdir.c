#include <sys/wait.h>
#include <sys/types.h>
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

int func_mkdir(char **args)
{
    int status;
    int ret;
    int pid;           
	pid = fork();     

    if ( pid < 0 )
	{                                 
	   perror("\nFork failed\n");
	   exit (-1);
	}
    if ( pid == 0 )
    {
        ret = execvp( args[0], args );
        if (ret<0)
        {
            perror("Error in execution\n");
        }
        if(ret==0)
        {
            execvp(args[0],args);
        }
        else
        {
            perror("Error\n");
        }
    }

    else
    {
        wait( &status ); 
    } 

    return 1;
}