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

void handle_one_dirent(char *parent, struct dirent *de, bool verbose);
void print_file_details(char *, char *);
void trim(char *, char *);

int main(int argc, char *argv[]) {
	char dirname[PATH_MAX];
	DIR *dir;
	struct dirent *de;

	bool verbose = false;
	// parse arguments
	for (int i = 1; i < 3; i++) {
		if (argv[i] != NULL) {
			char arg1[PATH_MAX];
			trim(arg1, argv[i]);

			if (strcmp(arg1, "-l") == 0) {
				verbose = true;
			} else {
				strcpy(dirname, arg1);
			}
		}
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
		handle_one_dirent(dirname, de, verbose);
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

void handle_one_dirent(char *parent_dir, struct dirent *de, bool verbose) {
	if (verbose) {
		print_file_details(parent_dir, de->d_name);
	} else {
		printf("%s\n", de->d_name);
	}
}

void print_file_details(char *base_path, char *filename) {
	// this method allocates memory on heap
	char *full_file_path = path_join(base_path, filename); 

	struct stat buf;
	int res = stat(full_file_path, &buf);

	if (res != 0) {
		printf("Unable to fetch stat of file, err code: %d\n", res);
		exit(-1);
	}

	// file size
	printf("%10lldB ", buf.st_size);
	// num blocks
	printf("%10lld ", buf.st_blocks);
	// num links
	printf("%5hi", buf.st_nlink);
	// inode number
	printf("%15lld ", buf.st_ino);
	// filename
	printf("%30s", filename);
	printf("\n");

	// free after done using
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
