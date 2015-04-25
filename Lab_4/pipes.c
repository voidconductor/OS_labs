#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
int file_pipes_data[2];
pid_t pid_data, pid_gamma;

off_t file_siz;

char *buf;

if (pipe(file_pipes_data) == 0)
{
	int file_pipes_gamma[2];
    pid_data = fork();
    if (pid_data == (pid_t)-1)
    {
	fprintf(stderr, "Fork failure");
	exit(EXIT_FAILURE);
    } else if (pid_data == (pid_t)0)
    {
		int file;
	close(1);
	dup(file_pipes_data[1]);
	close(file_pipes_data[1]);
	close(file_pipes_data[0]);

	if ((file = open(argv[1], O_RDONLY)) == -1)
			printf("Can't open file data.txt\n");

	file_siz = lseek(file, 0, SEEK_END);
	lseek(file, 0, SEEK_SET);

	while (file_siz > 0)
	{
	    file_siz -= read(file, buf, 1);
	    write(1, buf, 1);
	}
		close(file);
    } else if (pipe(file_pipes_gamma) == 0)
    {
	pid_gamma = fork();
	if (pid_gamma == (pid_t)-1)
	{
	    fprintf(stderr, "Fork failure");
	    exit(EXIT_FAILURE);
	} else if (pid_gamma == (pid_t)0)
	{
			char buf2[256] = {0};
			int file;
	    close(1);
	    dup(file_pipes_gamma[1]);
	    close(file_pipes_gamma[1]);
	    close(file_pipes_gamma[0]);

	    if ((file = open("gamma.txt", O_RDONLY)) == -1)
		printf("Can't open file gamma.txt\n");
	    read(file, buf2, sizeof(buf2));
	    write(1, buf2, sizeof(buf2));
			close(file);
	} else
	{
			char buf2[256] = {0};
			int i = 0;
			int ifread = 1;
			int file;
	    close(file_pipes_data[1]);
	    close(file_pipes_gamma[1]);
	    if ((file = open("output", O_CREAT | O_WRONLY, S_IWUSR|S_IRUSR)) == -1)
		printf("Can't open file output.txt\n");
	    read(file_pipes_gamma[0], buf2, sizeof(buf2));

	    while (ifread != 0)
	    {
		ifread = read(file_pipes_data[0], buf, 1);
		buf[0] = buf[0]^buf2[i];
		i++;

		if (i == 256)
		    i = 0;
		write(file, buf, 1);
	    }
		close(file);
	}
    }
}
return 0;
}
