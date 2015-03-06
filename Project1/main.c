#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h> 
#include <pwd.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#define clean() printf("\033[H\033[J")
#define ARRSIZE 1024

int num_children;
int jobs[ARRSIZE];
char* commands_list[ARRSIZE];

void exit_handle(int sig)
{
	int pid;
	int j;
	int jobid;
	pid = wait(NULL);
	for (j = 0; j < num_children; j++)
	{
		if (jobs[j] == pid)
		{
			jobid = j;
			break;
		}
	}
	printf("\n[%d] %d finished %s\n",jobid, pid, commands_list[jobid]);
}


int main (int argc, char **argv, char **envp) 
{
	signal(SIGCHLD, exit_handle);
	char cmd[ARRSIZE];
	char cmd2[ARRSIZE]; // just to keep the original value of cmd intact.
	char* home_dir;
	char* pch;
	char* commands[ARRSIZE];
	int i;
	char cwd[ARRSIZE];
	char charTotext[ARRSIZE];
	int endID, status;
	extern char** environ;
	//char** environ;
		num_children=0;
	//printf("%s", argv[1]);
	//if(strcmp(argv[1], "<") == 0) printf("%s", argv[2]);
	
	
	clean();
	
	while (true)
	{
		endID = waitpid(-1, &status, WNOHANG);	
		if(endID > 0)
		{
			printf("Process %d ended\n", endID);
		}
		printf("> ");
		
		fgets(cmd, sizeof cmd, stdin);
		cmd[strlen(cmd)-1] = '\0'; 	// make the last character '\0'
		
		strcpy(cmd2, cmd); // Keep the original value of cmd in cmd2 before the mutations
		
		pch = strtok(cmd, " ");
		i=0;
		while (pch != NULL)
  		{
		commands[i] = pch;
    		if(i > 1 && strcmp(commands[0], "cd") ==0)
    		{
				strcpy(charTotext, pch);
    			commands[1] = strcat(commands[1], " ");
    			commands[1] = strcat(commands[1], charTotext);
    		}
    		pch = strtok (NULL, " ");
    		
  		i++;	
  		}
		if(strcmp(commands[0], "fork") == 0)
		{
			system("./a.out");
		}
		/* Command : where - Print the current location. */
		else if ((strcmp(commands[0], "where") == 0))
		{
			if (getcwd(cwd, sizeof(cwd)) != NULL) printf("%s\n\n", cwd);
		}
		else if ((strcmp(commands[0], "exit") == 0) || (strcmp(commands[0], "quit") == 0))
		{
			clean();
			break;
		}
		else if (strcmp(commands[0], "set")==0)
		{
			if(i == 2)
			{
				int ret;
				char* setHelper;
				char variable_name[ARRSIZE];
				char variable_value[ARRSIZE];
				char setter[ARRSIZE];
				int j=0;				
				setHelper = strtok (commands[1], "=");

				while (setHelper != NULL)
		  		{
    				if(j==0)
    				{
	    				strcpy(variable_name, setHelper);
    				}
    				if( j==1)
    				{
		    			strcpy(variable_value, setHelper);
    				}
    				if(j > 1 )
    				{
    					printf("Error in the input. \n\n");
		    		}
    		
			    	setHelper = strtok (NULL, "=");
  					j++;	
  				}
  				
				ret = setenv(variable_name, variable_value, 1);
				if(ret == 0)
				{
					 printf("New %s has been set to %s \n\n", variable_name, variable_value);		
				}else
				{
					 printf("Encountered error while setting %s value.\n\n", variable_name);		
				}
				
			}
			else
			{
				printf("ERROR:\tInvalid number of arguments provided.\n");
				printf("\tLegal use: set VARIABLE=VALUE  \n\n");
			}
		}
		else if (strcmp(commands[0], "cd") == 0)
		{	
			// Only one word is present i.e. 'cd'
			if(i==1) 
			{
				int checker = chdir(getenv("HOME"));
				if(checker == 0 ) 
				{	
					printf("Arrived at : HOME.\n\n");
					char *cwd;
					cwd = getcwd (0, 0);

  					/*int len;
			   		len = strlen(cwd);*/
			   		
				}
				else 
				{
					printf("Problem loading the home directory path!\n\n");
				}
				
			}
			if(i>1)
			{
				
				if(strncmp(commands[1], "/", 1)==0)
				{
					chdir(commands[1]); 
					printf("Arrived at : ");
					if (getcwd(cwd, sizeof(cwd)) != NULL) printf("%s\n\n", cwd);
				}
				else if (strcmp(commands[1], "..")==0)
				{
					chdir("..");
					printf("Arrived at : ");
					if (getcwd(cwd, sizeof(cwd)) != NULL) printf("%s\n\n", cwd);
				}
				else
				{
					char* changeTo;
					if (getcwd(cwd, sizeof(cwd)) != NULL);
					changeTo = strcat(cwd, "/");
					changeTo = strcat(changeTo, commands[1]);
					chdir(changeTo); 
					
					printf("Arrived at : ");
					if (getcwd(cwd, sizeof(cwd)) != NULL) printf("%s\n\n", cwd);
				
				}
				
			}
			
		}
		else if ((strcmp(cmd2, "get paths") == 0))
		{
			printf("PATH : %s\n", getenv("PATH"));
			printf("HOME : %s\n", getenv("HOME"));
			printf("ROOT : %s\n\n", getenv("ROOT"));
			
		}
		else if(strcmp(commands[0], "ls" )== 0 || strcmp(commands[0], "dir" )== 0)
		{
			DIR	*d;
			struct dirent *dir;
			d = opendir(".");
			if(d)
			{
				int count;
				while ((dir = readdir(d)) != NULL)
				{
						printf("%s\n", dir->d_name);

				}
				printf("\n");
				closedir(d);
                
			}
		
		}
		else
		{
		
		
		int pid;
		int status;
		pid = fork();
		
		 // Parent
		 if(pid > 0)
		 {
		 	jobs[num_children] = pid;
			commands_list[num_children] = cmd2;
		 	num_children++;
			if (strcmp(commands[i-1], "&") == 0)
			{
				printf("[%d] %d running in background",num_children - 1, jobs[num_children-1]);
		 	}
		 	else
		 	{
				printf("Parent waits. \n");
				printf("pid is : %d\n", jobs[num_children-1]);
		 		endID = waitpid(pid, &status, 0);
		 		printf("Process %d ended\n", endID);
		 		printf("Parent continues.\n");	
		 	}
		 }
		 // Child
		 else
		 {
			printf("in child");
			if (strcmp(commands[i - 1], "&") == 0)
			{
				setpgid(0, 0);
				int exists;
				commands[i-1] = NULL;
				exists= execvpe(commands[0], commands, environ); // Linux
				//exists= execvp(commands[0], commands);	// OS X
				if(exists == -1)
				{
					printf("ERROR:\tcommand not recognized. \n\n");
					printf("errno: %s\n", strerror(errno));
				}
				
		 	}
		 	else
		 	{
				int exists;
				commands[i] = NULL;
				exists= execvpe(commands[0], commands, environ); // Linux
				// exists= execvp(commands[0], commands); // OS X
				if(exists == -1)
				{
					printf("ERROR:\tcommand not recognized. \n\n");
					printf("errno: %s\n", strerror(errno));
				}			
			}
		 	
		 	return 0; // Necessary
		 
		 }
		
		
		
			//printf("ERROR:\tcommand not recognized. \n\n");
		}
		
	}

return 0;
}
