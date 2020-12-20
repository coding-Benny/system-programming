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
#define READ 0
#define WRITE 1

int fg = 0, bg = 0, num_of_bg_process = 0, stack = 0;
int bg_process_id[BUF_SIZE] = { 0 };
char* bg_process_name[BUF_SIZE];

int split_argv(char* command[], char* argv);
void chldsignal();

//***************************************************************
//        THE MAIN FUNCTION OF PROGRAM
//***************************************************************

int main() {
	int argc = -1, amper = -1;
	int fd, pid, status, redi_idx, pipe_idx;
	int filedes[2];
	char *file_name, *command[BUF_SIZE], *command1[BUF_SIZE], *command2[BUF_SIZE]; 
	char argv[BUF_SIZE], tmp[BUF_SIZE], temp_argv[BUF_SIZE], user_name[BUF_SIZE], host_name[BUF_SIZE], current_path[BUF_SIZE];
	char *current_dir = getcwd(current_path, BUF_SIZE);
	struct passwd* pwd = getpwuid(getuid());
	strcpy(user_name, pwd->pw_name);
	gethostname(host_name, BUF_SIZE);

	signal(SIGCHLD, chldsignal);
	
	while (1) {		
		printf("\e[32;1m%s@%s\e[0m:\e[34;1m%s\e[0m$ ", user_name, host_name, current_dir);

		fgets(argv, sizeof(argv), stdin);
		
		int argv_len = strlen(argv);
		argv[argv_len - 1] = '\0';
			
		strcpy(temp_argv, argv);
		
		if (bg == 0 && strlen(argv))
			strncpy(tmp, argv, strlen(argv) - 2);

		argc = split_argv(command, temp_argv);
		
		if (argc > 0) {
			if (strcmp(command[0], "exit") == 0 || strcmp(command[0], "logout") == 0)
				exit(0);
			
			if (strcmp(command[0], "myjobs") == 0) {
				for(int i=0; i<num_of_bg_process; i++) {
					char stack_sign = (i == num_of_bg_process - 1) ? '+' : '-';
					printf("[%d]%c  Running\t\t%d\t%s\n", i+1, stack_sign, bg_process_id[i], bg_process_name[i]);
				}
				continue;
			}

			if (strcmp(command[argc - 1], "&") == 0) {	// background process
				amper = 1;
				command[argc - 1] = '\0';
				argc--;
			}
			else {	// foreground process
				amper = 0;
			}
			
			pid = fork();
			
			if (pid == 0) {	// child process
				if (strchr(argv, '>') != NULL && strchr(argv, '<') == NULL) {	// redirect output
					for (int i = 0; i < argc; i++) {
						if (strcmp(command[i], ">") == 0) {
							redi_idx = i;
							file_name = command[redi_idx + 1];
							break;
						}
					}
					if ((fd = creat(file_name, 0600)) == -1) {
						perror("can't create file");
						exit(1);
					}
					close(STDOUT_FILENO);
					dup(fd);	// process's stdout is file
					close(fd);
					for (int i = 0; i < redi_idx; i++)
						command1[i] = command[i];
					execvp(command1[0], command1);
					perror("can't redirect output");
					exit(1);
				}
				else if (strchr(argv, '<') != NULL && strchr(argv, '>') == NULL) {	// redirect input
					for (int i = 0; i < argc; i++) {
						if (strcmp(command[i], "<") == 0) {
							redi_idx = i;
							file_name = command[redi_idx + 1];
							break;
						}
					}
					if ((fd = open(file_name, O_RDONLY)) == -1) {
						perror("can't read file");
						exit(1);
					}
					close(STDIN_FILENO);
					dup(fd);	// process's stdin is file
					close(fd);
					for (int i = 0; i < redi_idx; i++)
						command1[i] = command[i];
					execvp(command1[0], command1);
					perror("can't redirect input");
					exit(1);
				}
				else {
					if (strchr(argv, '|') != NULL) {	// pipe
						for(int i = 0; i < argc; i++) {
							if (strcmp(command[i], "|") == 0) {
								pipe_idx = i;
							}
						}
						for (int i = 0; i < pipe_idx; i++)
							command1[i] = command[i];
						for(int i = 0; i < argc - (pipe_idx + 1); i++)
							command2[i] = command[pipe_idx + 1];

						pipe(filedes);
						
						if (fork() == 0) {	// grand child process
							close(filedes[READ]);
							dup2(filedes[WRITE], STDOUT_FILENO);
							close(filedes[WRITE]);
							execvp(command1[0], command1);
							perror("pipe");
							exit(1);
						}
						else {	// child process
							close(filedes[WRITE]);
							dup2(filedes[READ], STDIN_FILENO);
							close(filedes[READ]);
							execvp(command2[0], command2);
							perror("pipe");
							exit(1);
						}
					}
					else {
							execvp(command[0], &command[0]);
							perror("execvp");
							exit(1);
					}
				}
			}
			else {	// parent process
				if (amper == 0) {
					fg = 1;
					while (fg == 1)
						pause();
				}
				else {
					bg_process_name[num_of_bg_process] = malloc(sizeof(char) * BUF_SIZE);
					strcpy(bg_process_name[num_of_bg_process], command[0]);
					bg_process_id[num_of_bg_process] = pid;
					stack++;
					printf("[%d] %d\n", stack, pid);
					num_of_bg_process++;
				}
			}
		}
		
		if (bg == 1) {
			printf("[%d]+  Done\t\t\t%s &\n", stack, tmp);
			stack--;
			bg = 0;
		}
		
		if (argc == 0) continue;
	}
}

//***************************************************************
//        SPLIT COMMAND ARGUMENTS VECTOR
//***************************************************************

int split_argv(char* command[], char* argv) {
	int argc = 0;
	char* args;

	if (argv == NULL)
		return argc;
		
	args = strtok(argv, " \t\n");
	
	while (args != NULL) {
		command[argc] = args;
		args = strtok(NULL, " \t\n");
		argc++;
	}
	command[argc] = '\0';
	
	return argc;
}

//***************************************************************
//        HANDLER
//***************************************************************

void chldsignal() {
	int pid, status;
	pid = waitpid(-1, &status, 0);

	for (int i = 0; i < num_of_bg_process; i++) {
		if (bg_process_id[i] == pid) {
			int cur = i;
			bg = 1;
			if (cur > 0) {
				for(i = cur; i < num_of_bg_process - 1; i++) {
					bg_process_id[i] = bg_process_id[i+1];
					bg_process_name[i] = bg_process_name[i+1];
				}
			}
			bg_process_id[i] = 0;
			bg_process_name[i] = NULL;
			free(bg_process_name[num_of_bg_process-1]);	
			num_of_bg_process--;
			return;
		}
	}
	fg = 0;
}
