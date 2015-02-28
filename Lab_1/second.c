#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/poll.h>
#include <limits.h>
#include <sys/stat.h> 
#include <fcntl.h>

#define MAX_EVENTS 1024 	//Максимальное кличество событий для обработки за один раз
#define LEN_NAME 16 		//Будем считать, что длина имени файла не превышает 16 символов
#define EVENT_SIZE  ( sizeof (struct inotify_event) ) 			//размер структуры события
#define BUF_LEN     ( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME ))   //буфер для хранения данных о событиях

int main( int argc, char **argv ) 
{
int length, i = 0, wd;
int tsec = atoi(argv[2]);
int fd, outf;
char buffer[BUF_LEN];
char str[256];
int sprint_cnt;

outf = open("outfile",O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
// Инициализация Inotify
fd = inotify_init();
if ( fd < 0 )
{
	perror( "Couldn't initialize inotify");
}

// добавим наблюдение для директории, заданной первым параметром командной строки
wd = inotify_add_watch(fd, argv[1], IN_CREATE | IN_MODIFY | IN_DELETE);

if (wd == -1)
{
	sprint_cnt=sprintf(str,"Couldn't add watch to %s\n",argv[1]);
	write(outf, str, sprint_cnt);
}
else
{
	sprint_cnt=sprintf(str,"Watching:: %s\n",argv[1]);
	write(outf, str, sprint_cnt);
}

struct pollfd pfd = {fd, POLLIN, 0};

//Open or create output file.
// бесконечный цикл
while(1)
{
	int ret = poll(&pfd, 1, tsec*1000);
	if(ret < 0)
	{
		sprint_cnt=sprintf(str,"Poll failed: \n");
		write(outf, str, sprint_cnt);
	}
	else if (ret == 0)
	{
		sprint_cnt=sprintf(str,"Nothing happened in %i seconds.\n",tsec);
		write(outf, str, sprint_cnt);
	}
	else
	{
		i = 0;
		length = read( fd, buffer, BUF_LEN );

		if ( length < 0 )
		{
			perror( "read" );
		}

		while ( i < length )
		{
			struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
			if ( event->len )
			{
				if ( event->mask & IN_CREATE)
				{
					if (event->mask & IN_ISDIR)
						printf( "The directory %s was Created.\n", event->name );
					else
						printf( "The file %s was Created with WD %d\n", event->name, event->wd );
				}
				if ( event->mask & IN_MODIFY)
				{
					if (event->mask & IN_ISDIR)
						printf( "The directory %s was modified.\n", event->name );
					else
						printf( "The file %s was modified with WD %d\n", event->name, event->wd );
				}
				if ( event->mask & IN_DELETE)
				{
					if (event->mask & IN_ISDIR)
						printf( "The directory %s was deleted.\n", event->name );
					else
						printf( "The file %s was deleted with WD %d\n", event->name, event->wd );
				}
				i += EVENT_SIZE + event->len;
			}
		}
	}
}

// Освобождение ресурсов
inotify_rm_watch( fd, wd );
close( fd );
return 0;
}
