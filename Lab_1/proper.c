#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

struct keeper
{
    char *filename;
    int filetype;
    int id;
};

int main(int argc, char** argv)
{
int i,j;
struct keeper old[128];
int size_prev;
struct keeper fresh[128];
int size_curr;

DIR *dir;
struct dirent *entry;

dir = opendir("/home/protodwarf/lolfolder");
if (!dir) {
        perror("diropen");
        exit(1);
    }
/*
    while ( (entry = readdir(dir)) != NULL) {
        printf("%d - %s [%d]\n",
            entry->d_ino,entry->d_name, entry->d_type);
    };
*/

    i=0;
    while ( (entry = readdir(dir)) != NULL) {
        fresh[i].filename = entry->d_name;
        fresh[i].id = entry->d_ino;
        fresh[i].filetype = entry->d_type;
        i++;
    }
    size_curr = i;

    for(j = 0; j < size_curr; j++)
    {
        old[j]=fresh[j];
    }
    size_prev = size_curr;

    printf("%d \n", size_curr);

return 0;
}
