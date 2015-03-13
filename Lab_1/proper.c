#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

int main(int argc, char** argv)
{
struct stat info;
DIR *dir;
struct dirent *entry;

dir = opendir("/home/protodwarf/lolfolder");
if (!dir) {
        perror("diropen");
        exit(1);
    };

    while ( (entry = readdir(dir)) != NULL) {
        printf("%s [%d]\n",
            entry->d_name, entry->d_type);
    };

return 0;
}
