//***************************************************************
//                   HEADER FILE USED IN PROJECT
//***************************************************************

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

char type(mode_t mode);
char* perm(mode_t mode);
void printStat(char *pathname, char *file, struct stat *st);

//***************************************************************
//                  THE MAIN FUNCTION OF PROGRAM
//***************************************************************

int main(int argc, char *argv[]) {
	DIR *dp;
	char *dir;
	char *opt;
	char *filename;
	struct stat st;
	struct stat tmp;
	struct dirent *d;
	char path[BUFSIZ + 1];
	
	if (argc == 1) {	/* only ls command */
		dir = ".";
	}
	else if (argc == 2) {	/* ls + option || ls + file/directory */		
		if (argv[1][0] == '-') {	// option
			opt = argv[1];
			dir = ".";
		}
		else {
			if (lstat(argv[1], &tmp) < 0)
				perror(argv[1]);
			
			if (S_ISDIR(tmp.st_mode)) {	// directory
				dir = argv[1];
				opt = NULL;
				filename = NULL;
			}
			else if (S_ISREG(tmp.st_mode)) {	// file
				opt = NULL;
				
				if (strchr(argv[1], (int)'/') != NULL) {
					filename = basename(argv[1]);
					dir = dirname(argv[1]);
				}
				else {
					dir = ".";
					filename = argv[1];
				}
			}
		}
	}
	else if (argc == 3) {	/* ls + option + file/directory */
		opt = argv[1];
		
		if (lstat(argv[2], &tmp) < 0)
			perror(argv[2]);
			
		/*if (argv[2][0] == '.' || strchr(argv[2], (int)'/') != NULL) {
			dir = argv[2];
		}
		else {
			filename = argv[2];
			dir = ".";
		}*/
		
		if (S_ISDIR(tmp.st_mode)) {	// directory
			dir = argv[2];
			filename = NULL;
		}
		else if (S_ISREG(tmp.st_mode)) {	// file			
			if (strchr(argv[2], (int)'/') != NULL) {
				filename = basename(argv[2]);
				dir = dirname(argv[2]);
			}
			else {
				dir = ".";
				filename = argv[2];
			}
		}
	}

	if ((dp = opendir(dir)) == NULL)	// open directory
		perror(dir);
		
	while ((d = readdir(dp)) != NULL) {	// create a file path name for each directory entry
		sprintf(path, "%s/%s", dir, d->d_name);

		if (lstat(path, &st) < 0)   // get file status information
			perror(path);
		
		if (argc == 1) {    /* only ls command */
			if (d->d_name[0] == '.')
				continue;
			printf("%s\n", d->d_name);
		}
		else if (argc == 2) {   /* ls + option || ls + file/directory */
			if (opt == NULL) {
				if (filename == NULL && dir != ".") {	// ls dir
					if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
						continue;
					printf("%s\n", d->d_name);
				}
				else if (strcmp(filename, d->d_name) == 0 && dir == ".") {	// ls filename
					printf("%s\n", d->d_name);
					break;
				}
				else if (dir != "." && strcmp(filename, d->d_name) == 0) {	// ls dir/filename
					printf("%s\n", path);
					break;
				}
			}
			else if (strcmp(opt, "-a") == 0) {	// ls -a
				printf("%s\n", d->d_name);
			}
			else if (strcmp(opt, "-l") == 0) {	// ls -l
				if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
					continue;
				printStat(path, d->d_name, &st);
				putchar('\n');
			}
			else if (strcmp(opt, "-al") == 0 || strcmp(opt, "-la") == 0) {	// ls -al/-la
				printStat(path, d->d_name, &st);
				putchar('\n');
			}
		}
		else if (argc == 3) {   /* ls + option + file/directory */
			if (strcmp(opt, "-a") == 0) {
				if (strcmp(argv[2], d->d_name) == 0 && dir == ".") {	// ls -a filename
					printf("%s\n", d->d_name);
					break;
				}
				else if (dir != "." && filename == NULL) {	// ls -a dir
					printf("%s\n", d->d_name);
				}
				else if (dir != "." && strcmp(filename, d->d_name) == 0) {	// ls -a dir/filename
					printf("%s\n", path);
					break;
				}
			}
			else if (strcmp(opt, "-l") == 0) {
				if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
					continue;
					
				if (strcmp(argv[2], d->d_name) == 0 && dir == ".") {	// ls -l filename
					printStat(path, d->d_name, &st);
					putchar('\n');
					break;
				}
				else if (dir != "." && filename == NULL) {	// ls -l dir
					printStat(path, d->d_name, &st);
					putchar('\n');
				}
				else if (dir != "." && strcmp(filename, d->d_name) == 0) {	// ls -l dir/filename
					printStat(path, path, &st);
					putchar('\n');
					break;
				}
			}
			else if (strcmp(opt, "-al") == 0 || strcmp(opt, "-la") == 0) {
				if (strcmp(argv[2], d->d_name) == 0 && dir == ".") {	// ls -al/-la filename
					printStat(path, d->d_name, &st);
					putchar('\n');
					break;
				}
				else if (dir != "." && filename == NULL) {	// ls -al/-la directory
					printStat(path, d->d_name, &st);
					putchar('\n');
				}
				else if (dir != "." && strcmp(filename, d->d_name) == 0) {	// ls -al/-la dir/filename
					printStat(path, path, &st);
					putchar('\n');
					break;
				}
			}
		}
	}
	closedir(dp);
	exit(0);
}

//***************************************************************
//                   PRINT FILE STATUS INFO
//***************************************************************

void printStat(char *pathname, char *file, struct stat *st) {
	printf("%c%s ", type(st->st_mode), perm(st->st_mode));
	printf("%3d ", st->st_nlink);
	printf("%s %s ", getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name);
	printf("%9d ", st->st_size);
	printf("%.12s ", ctime(&st->st_mtime) + 4);
	printf("%s", file);
}

//***************************************************************
//                   CHECK FILE TYPE
//***************************************************************

char type(mode_t mode) {
	if (S_ISREG(mode))
		return('-');
	if (S_ISDIR(mode))
		return('d');
	if (S_ISCHR(mode))
		return('c');
	if (S_ISBLK(mode))
		return('b');
	if (S_ISLNK(mode))
		return('l');
	if (S_ISFIFO(mode))
		return('p');
	if (S_ISSOCK(mode))
		return('s');
}

//***************************************************************
//                   CHECK FILE ACCESS PERMISSION
//***************************************************************

char* perm(mode_t mode) {
	int i;
	static char perms[10] = "---------";
	strcpy(perms, "---------");
	
	for (i=0; i<3; i++) {
		if (mode & (S_IREAD >> i*3))
			perms[i*3] = 'r';
		if (mode & (S_IWRITE >> i*3))
			perms[i*3+1] = 'w';
		if (mode & (S_IEXEC >> i*3))
			perms[i*3+2] = 'x';
	}
	return (perms);
}
