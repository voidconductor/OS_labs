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
int file_pipes_data [2];
int file_pipes_gamma [2];
pid_t pid_data, pid_gamma;
int file;
int i;

char buf[8] = {0};
char buf2[8] = {0};


if (pipe(file_pipes_data)==0)
{
    pid_data=fork();
    if (pid_data == (pid_t)-1)
    {
        fprintf(stderr, "Fork failure");
        exit(EXIT_FAILURE);
    }
    else if (pid_data==(pid_t)0)
    {
        close(1);
        dup(file_pipes_data[1]);
        close(file_pipes_data[1]);
        close(file_pipes_data[0]);

        if((file=open("data.txt",O_RDONLY)) == -1)
			printf("Can't open file data.txt\n");
        read(file,buf, sizeof(buf));
        printf("%s",buf);
    }
    else if(pipe(file_pipes_gamma)==0)
    {
        pid_gamma=fork();
        if (pid_gamma == (pid_t)-1)
        {
            fprintf(stderr, "Fork failure");
            exit(EXIT_FAILURE);
        }
        else if (pid_gamma==(pid_t)0)
        {
            close(1);
            dup(file_pipes_gamma[1]);
            close(file_pipes_gamma[1]);
            close(file_pipes_gamma[0]);

            if((file=open("gamma.txt",O_RDONLY)) == -1)
                printf("Can't open file gamma.txt\n");
            read(file,buf2, sizeof(buf2));
            printf("%s",buf2);
        }
        else
        {
            close(file_pipes_data[1]);
            close(file_pipes_gamma[1]);
            if ((file = open("output.txt",O_CREAT | O_WRONLY,S_IWUSR|S_IRUSR)) == -1)
                printf("Can't open file output.txt\n");
            read(file_pipes_data[0], buf, sizeof(buf));
            read(file_pipes_gamma[0], buf2, sizeof(buf2));
            printf("%s \n", buf);

            for(i = 0; i < 8; i++)
            {
                buf[i] = buf[i]^buf2[i];
            }
            printf("%s \n", buf);
            write(file,buf,sizeof(buf));
        }
    }

}
close(file);
return 0;
}
