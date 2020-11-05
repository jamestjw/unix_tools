#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("A filename should be passed in as an argument!\n");
		exit(-1);
	}

	const char *fd = argv[1];
	printf("Filename: %s\n", fd);

	struct stat buf;
	int res = stat(fd, &buf);

	if (res != 0) {
		printf("Unable to fetch stat of file, err code: %d\n", res);
		exit(-1);
	}

	printf("File size: %lld bytes\n", buf.st_size);
	printf("Num blocks: %lld\n", buf.st_blocks);
	printf("Num links: %hi\n", buf.st_nlink);
	printf("Inode number: %lld\n", buf.st_ino);

	return 0;
}
