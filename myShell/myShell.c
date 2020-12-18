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
#include <fcntl.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

#define BUF_SIZE 512
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

char command[BUF_SIZE][BUF_SIZE];
int split_argv(char* argv);
void exit_process();

//***************************************************************
//        THE MAIN FUNCTION OF PROGRAM
//***************************************************************

int main() {
	int argc = -1, amper = -1;
	int fd, pid, status, ch, redi_idx;
	int filedes[2];
	char *file_name, *command1, *command2;
	char argv[BUF_SIZE];
	char user_name[BUF_SIZE], host_name[BUF_SIZE], current_path[BUF_SIZE];
	char *current_dir = getcwd(current_path, BUF_SIZE);
	struct passwd* pwd = getpwuid(getuid());
	strcpy(user_name, pwd->pw_name);
	gethostname(host_name, BUF_SIZE);
	
	while (1) {		
		
		printf("\e[32;1m%s@%s\e[0m:\e[34;1m%s\e[0m$ ", user_name, host_name, current_dir);

		fgets(argv, BUF_SIZE, stdin);
		
		int argv_len = strlen(argv);
		if (argv_len != BUF_SIZE)
			argv[argv_len - 1] = '\0';

		argc = split_argv(argv);
		
		// print
		printf("argc=%d, argv=%s\n", argc, argv);
		for(int i=0;i<3;i++)
			printf("%s\t", command[i]);
		printf("\n");
		
		if (strcmp(command[0], "exit") == 0 || strcmp(command[0], "logout") == 0) {
			printf("exit shell prog\n");
			exit_process();
		}

		if (strcmp(command[argc - 1], "&") == 0) {	// background process
			amper = 1;
		}
		else {	// foreground process
			amper = 0;
		}
		
		pid = fork();
		
		if (pid == 0) {	// child process
			if (strchr(argv, '>') != NULL && strchr(argv, '<') == NULL) {	// redirect output
				for (int i=0; i < argc; i++) {
					if (strcmp(command[i], ">") == 0) {
						redi_idx = i;
						file_name = command[redi_idx + 1];
						break;
					}
				}
				if ((fd = creat(file_name, 0600)) == -1)
					perror("can't create file");
				close(STDOUT_FILENO);
				dup(fd);	// process's stdout is file
				switch (redi_idx) {
					case 1:
						if (execlp(command[0], command[0], NULL) == -1) {
								perror("execlp");
								exit_process();
							}
						break;
					case 2:
						if (execlp(command[0], command[0], command[1], NULL) == -1) {
							perror("execlp");
							exit_process();
						}
						break;
				}
				close(fd);
			}
			else if (strchr(argv, '<') != NULL && strchr(argv, '>') == NULL) {	// redirect input
				for (int i=0; i < argc; i++) {
					if (strcmp(command[i], "<") == 0) {
						redi_idx = i;
						file_name = command[redi_idx + 1];
						break;
					}
				}
				if ((fd = open(file_name, O_RDONLY)) == -1)
					perror("can't read file");
				close(STDIN_FILENO);
				dup(fd);	// process's stdin is file
				switch (redi_idx) {
					case 1:
						if (execlp(command[0], command[0], NULL) == -1) {
								perror("execlp");
								exit_process();
							}
						break;
					case 2:
						if (execlp(command[0], command[0], command[1], NULL) == -1) {
							perror("execlp");
							exit_process();
						}
						break;
				}
				close(fd);
				printf("redirect input\n");
			}
			else {
				if (strchr(argv, '|') != NULL) {	// pipe
					command1 = strtok(argv, " | ");
					command2 = strtok(NULL, " | ");
					pipe(filedes);
					
					if (fork() == 0) {	// grand child process
						close(filedes[0]);
						dup2(filedes[1], 1);
						close(filedes[1]);

						if (execlp(command1, command1, NULL) == -1) {
							perror("pipe");
							exit_process();
						}
					}
					else {	// child process
						close(filedes[1]);
						dup2(filedes[0], 0);
						close(filedes[0]);

						if (execlp(command2, command2, NULL) == -1) {
							perror("pipe");
							exit_process();
						}
					}
				}
				else {
					switch (argc) {
						case 1:
							if (execlp(command[0], command[0], NULL) == -1) {
								perror("execlp");
								exit_process();
							}
							break;
						case 2:
							if (execlp(command[0], command[0], command[1], NULL) == -1) {
								perror("execlp");
								exit_process();
							}
							break;
						case 3:
							if (execlp(command[0], command[0], command[1], command[2], NULL) == -1) {
								perror("execlp");
								exit_process();
							}
							break;
					}
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

void exit_process() {
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
		command[argc][j] = '\0';
		argc++;
	}
	
	return argc;
}
