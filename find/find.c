#include <sys/stat.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <stdbool.h>

bool path_hidden(char *);
void print_fname(char *, int);
void handle_one_dirent(char *parent, struct dirent *de, int indentation_lvl);
void print_file_details(char *, char *);
void trim(char *, char *);
bool may_crawl_dir(char *);

int main(int argc, char *argv[]) {
	char dirname[PATH_MAX];
	DIR *dir;
	struct dirent *de;

	// parse arguments
	if (argv[1] != NULL) {
		strcpy(dirname, argv[1]);	
	}

	if (dirname[0] == '\0' && getcwd(dirname, PATH_MAX) == NULL) {
		fprintf(stderr, "Unable to fetch cwd\n");
		exit(-1);
	}

	printf("Directory: %s\n", dirname);
	
	if ((dir = opendir(dirname)) == NULL) {
		fprintf(stderr, "Unable to open directory %s\n", dirname);
	}

	for (de = readdir(dir); de != NULL; de = readdir(dir)) {
		// start with indentation_lvl of 0
		handle_one_dirent(dirname, de, 1);
	}

	return 0;
}

// Returns a pointer to a new string, memory should be free-ed after using
char *path_join(char *base, char *child) {
	// we need to add 2 because of the '/' and terminating 0
	char *fullpath = malloc(strlen(base) + strlen(child) + 2);
	sprintf(fullpath, "%s/%s", base, child);
	return fullpath;
}

// Given a path this function will print out
// details of this file
void file_handler(char *basepath, char *filename, int indentation_lvl) {
	print_fname(filename, indentation_lvl);
}

// Given a path , this function will
// traverse this directory and print all files and directories
// within it.
void crawl_dir(char *dirname, int indentation_lvl) {
	DIR *dir;
	struct dirent *de;

	if ((dir = opendir(dirname)) == NULL) {
		fprintf(stderr, "Unable to open directory %s\n", dirname);
	}

	for (de = readdir(dir); de != NULL; de = readdir(dir)) {
		handle_one_dirent(dirname, de, indentation_lvl);
	}
}

void handle_one_dirent(char *parent_dir, struct dirent *de, int indentation_lvl) {
	// this method allocates memory on heap
	char *full_file_path = path_join(parent_dir, de->d_name); 

	struct stat buf;
	int res = stat(full_file_path, &buf);

	if (res != 0) {
		printf("Unable to fetch stat of file, err code: %d\n", res);
		exit(-1);
	}
	
	if (S_ISDIR(buf.st_mode)) {
		print_fname(de->d_name, indentation_lvl);
		if (may_crawl_dir(de->d_name)) {
			// in a deeper level now, increment indentation_lvl
			crawl_dir(full_file_path, indentation_lvl + 1);
		}
	} else {
		if (!path_hidden(de->d_name)) {
			// no need to increment indentation_lvl here as we are in the same level
			file_handler(parent_dir, de->d_name, indentation_lvl);
		}
	}

	free(full_file_path);
}

// trim: leading and trailing whitespace of string
void trim (char *dest, char *src)
{
    if (!src || !dest)
       return;

    int len = strlen (src);

    if (!len) {
        *dest = '\0';
        return;
    }
    char *ptr = src + len - 1;

    // remove trailing whitespace
    while (ptr > src) {
        if (!isspace (*ptr))
            break;
        ptr--;
    }

    ptr++;

    char *q;
    // remove leading whitespace
    for (q = src; (q < ptr && isspace (*q)); q++)
        ;

    while (q < ptr)
        *dest++ = *q++;

    *dest = '\0';
}

bool path_hidden(char *path) {
	bool is_hidden = path[0] == '.';
	return is_hidden;
}

bool may_crawl_dir(char *path) {
	char *dot = ".";
	char *dotdot = "..";
	
	bool isdotdot = strcmp(path, dotdot) == 0;
	bool isdot= strcmp(path, dot) == 0;

	bool is_hidden = path_hidden(path); 

	return !isdot && !isdotdot && !is_hidden;
}

void print_fname(char *fname, int indentation_lvl) {
	char identation_char = '-';
	int space_per_lvl = 2;
	int repetition_count = indentation_lvl * space_per_lvl;
	
	// handle indentation	
	// TODO: Handle adding of many spaces here as the length of its parent dir for aesthetic purposes
	putchar('|');

	for (int i = 0; i < repetition_count; i++) {
		putchar(identation_char);
	}

	putchar(' ');

	// print filename
	printf("%s\n", fname);
}
