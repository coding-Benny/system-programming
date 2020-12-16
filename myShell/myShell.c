//***************************************************************
//                   HEADER FILE USED IN PROJECT
//***************************************************************

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#include <errno.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

#define BUF_SIZE 512

char command[BUF_SIZE][BUF_SIZE];
char user_name[BUF_SIZE];
char host_name[BUF_SIZE];
char current_path[BUF_SIZE];

int split_argv(char* argv);
void exit_shell();

//***************************************************************
//        THE MAIN FUNCTION OF PROGRAM
//***************************************************************

int main() {
	int argc = -1;
	int amper = -1;
	int fd, pid, status;
	char argv[BUF_SIZE];
	int filedes[2];
	char *command1, *command2;
	
	char *current_dir = getcwd(current_path, BUF_SIZE);
	struct passwd* pwd = getpwuid(getuid());
	strcpy(user_name, pwd->pw_name);
	gethostname(host_name, BUF_SIZE);
	
	while (1) {
		printf("\e[32;1m%s@%s\e[0m:\e[34;1m%s\e[0m$ ", user_name, host_name, current_dir);
		
		fgets(argv, sizeof(argv), stdin);
		argv[strlen(argv) - 1] = '\0';

		argc = split_argv(argv);
		
		if (strcmp(command[0], "exit") == 0 || strcmp(command[0], "logout") == 0) {
			exit_shell();
		}

		if (strcmp(command[argc-1],"&") == 0) {	// background process
			amper = 1;
		}
		else {	// foreground process
			amper = 0;
		}
		
		pid = fork();
		
		if (pid == 0) {	// child process
			if (strchr(argv, '>') != NULL && strchr(argv, '<') == NULL) {	// redirect output
				if ((fd = creat("newfile.txt", 0600)) == -1)
					perror("newfile");
				close(stdout);
				dup(fd);	// process's stdout is file
				close(fd);
			}
			else if (strchr(argv, '<') != NULL && strchr(argv, '>') == NULL) {	// redirect input
				printf("redirect input\n");
			}
			else {
				if (strchr(argv, '|') != NULL) {	// pipe
					command1 = strtok(argv, "| ");
					command2 = strtok(NULL, "| ");
					pipe(filedes);
					
					if (fork() == 0) {	// grand child process
						close(stdout);
						dup(filedes[1]);
						close(filedes[1]);
						close(filedes[0]);
						if (execlp(command1, command1, NULL) == -1) {
							perror("pipe");
							exit_shell();
						}
					}
					else {	// child process
						close(stdin);
						dup(filedes[0]);
						close(filedes[0]);
						close(filedes[1]);
						if (execlp(command2, command2, NULL) == -1) {
							perror("pipe");
							exit_shell();
						}
					}
				}
				else {
					switch(argc) {
						case 1:
							if (execlp(command[0], command[0], NULL) == -1)
								fprintf(stderr, "error: %s\n", strerror(errno));
							break;
						case 2:
							if (execlp(command[0], command[0], command[1], NULL) == -1)
								fprintf(stderr, "error: %s\n", strerror(errno));
							break;
						case 3:
							if (execlp(command[0], command[0], command[1], command[2], NULL) == -1)
								fprintf(stderr, "error: %s\n", strerror(errno));
							break;
					}
					perror("execlp");
				}
			}
		}
		else {	// parent process
			if (amper == 0) {
				wait(&status);
			}
		}	
	}
}

//***************************************************************
//        EXIT SHELL PROGRAM
//***************************************************************

void exit_shell() {
	pid_t pid = getpid();
	if (kill(pid, SIGTERM) == -1)
		exit(-1);
	else
		exit(0);
}

//***************************************************************
//        SPLIT COMMAND ARGUMENTS VECTOR
//***************************************************************

int split_argv(char* argv) {
	int i, j;
	int argc = 0;
	int argv_len = strlen(argv);
	
	for(i = 0, j = 0; i < argv_len; i++) {
		if (argv[i] != ' ') {
			command[argc][j] = argv[i];
			j++;
		}
		else {
			if (j != 0) {
				command[argc][j] = '\0';
				j = 0;
				argc++;
			}
		}
	}
	if (j != 0) {
		command[i][j] = '\0';
		argc++;
	}
	
	return argc;
}
