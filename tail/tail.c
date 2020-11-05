#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>

const int BUFFER_SZ = 100;

int count_newlines(char *);
char *extract_after_newline(char *inp, int excess_nls);

int main(int argc, char *argv[]) {
	char *fname = NULL;
	// num of new lines to meet before calling it a day
	
	int targ_nl = 5 + 1;
	if (argc < 2) {
		fprintf(stderr, "Please pass in a filename");
		exit(-1);
	} else {
		for (int i = 1; i <= 2; i++) {
			char *arg;
			if ((arg = argv[i]) != NULL) {
				if (arg[0] == '-') {
					targ_nl = atoi(arg + sizeof(char)) + 1;
				} else {
					fname = arg;
				}
			}
		}
	}
	
	if (fname == NULL || fname[0] == '\0') {
		fprintf(stderr, "Input filename is required!\n");
		exit(-1);
	}	
	
	// num of new lines we encountered so far
	int num_nl = 0;

	int fd = open(fname, O_RDONLY);
	int block_end = lseek(fd, 0, SEEK_END);
	
	// num of reads done so far
	int num_reads = 0;
	// we will store read characters here
	char *read_chars[500];

	while (num_nl < targ_nl && block_end > 0) {	
		int required_nls = targ_nl- 1 - num_nl;

		// start offset of next block
		int block_start = block_end - BUFFER_SZ;
		if (block_start < 0) {
			block_start = 0;
		}
		int block_sz = block_end - block_start;
		char buffer[BUFFER_SZ];

		// seek to start of the next block
		lseek(fd, block_start, SEEK_SET);
		read(fd, buffer, block_sz);
		
		char *read_char = strdup(buffer);

		int found_nls = count_newlines(read_char);

		num_nl = num_nl + found_nls; 

		if (found_nls > required_nls) {
			int excess_nls = found_nls - required_nls;
			read_char = extract_after_newline(read_char, excess_nls);
		}

		read_chars[num_reads] = read_char;

		block_end = block_start;
		num_reads++;
	}

	int counted_nls = 0;

	// we can safely do this as num_reads is at least one if we get to this point
	for (int i = num_reads - 1; i >= 0; i--) {
		counted_nls += count_newlines(read_chars[i]);
		printf("%s", read_chars[i]);
	}
	
	// close file before ending program
	close(fd);
}

int count_newlines(char *inp) {
	int count = 0;
	char nl = '\n';
	for (int i = 0; i < strlen(inp); i++) {
		if ((char) inp[i] == nl) {
			count++;
		}
	}
	return count;
}

char *extract_after_newline(char *inp, int excess_nls) {
	int count = 0;
	int offset = 0;
	char nl = '\n';

	for (int i = 0; i < strlen(inp); i++) {
		if ((char) inp[i] == nl) {
			// we are okay with doing a plus one here since strlen doesn't include the null
			// terminate char
			offset = (i + 1) * sizeof(char);
			count ++;
			if (count == excess_nls) {
				break;
			}
		}
	}
	return inp + offset;
}
