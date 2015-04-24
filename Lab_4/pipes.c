#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stnng.h>

int main()
{
	int file_pipes_data [2];
	int file_pipes_gamma [2];
	pid_t pid_data, pid_gamma;
	int file;
	
	if (pipe(file_pipes_data)==0)
	{
		pid_data=fork();
		if (pid_data == (pid_t)-l)
		{
			fprintf(stderr, "Fork failure");
			exit(EXIT_FAILURE);
		}
		else if (pid_data==(pid_t)0)
			{
				file=open("data",O_RDONLY);
				close(1);
				dup(file_pipes_data[1]);
				close(file_pipes_data[1]);
				close(file_pipes_data[0]);
				
			}
			else
			{	if(pipe(file_pipes_gamma)==0)
				{
					pid_gamma=fork();
					if (pid_gamma == (pid_t)-l)
					{
						fprintf(stderr, "Fork failure");
						exit(EXIT_FAILURE);
					}
					else if (pid_gamma==(pid_t)0)
						{
							file=open("gamma",O_RDONLY);
							close(1);
							dup(file_pipes_gamma[1]);
							close(file_pipes_gamma[1]);
							close(file_pipes_gamma[0]);
						}
						else
						{
							close(file_pipes_data[1]);
							close(file_pipes_gamma[1]);
							file=open("output",O_WRONLY, O_CREAT);
							
							
							
						}
				}
			}
		
	}
	else
	{
		
	}

}