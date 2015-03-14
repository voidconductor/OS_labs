#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <string.h>

struct keeper
{
    char filename[256];
    int filetype;
    int id;
};

void supergood_message()
{
    printf("\nERROR! \n");
    printf("Необходимо 2 аргумента командной строки: полный путь и время ожидания в секундаx. \n");
    printf("Завершение программы. \n");
    printf("ERROR! \n \n");
}

char *dumb_types(int filetype)
{
    if(filetype == 4)
        return "folder";
    if(filetype == 8)
        return "file";
}



int main(int argc, char** argv)
{
struct timespec tw;
struct timespec tr;

int i,j,k;
int flag; //Just another logic var.
struct keeper old[128];
int size_prev;
struct keeper fresh[128];
int size_curr;

DIR *dir; long int location;
struct dirent *entry;

if(argc < 3)
{
    supergood_message();
    return 0;
}

dir = opendir(argv[1]);
if (!dir) {
        perror("diropen");
        return 0;
    }

    location = telldir(dir); //Выявление локации - начала папки.

    i=0;
    while ( (entry = readdir(dir)) != NULL) {
        sprintf(old[i].filename,"%s", entry->d_name);
        old[i].id = entry->d_ino;
        old[i].filetype = entry->d_type;
        i++;
    }
    size_prev = i;

// Здесь помещаем время ожидания из входных аргументов в структуру.
tw.tv_sec = atoi(argv[2]);
tw.tv_nsec = 0;

// Вот тут вот у нас начинается бесконечный цикл.
while(1)
{
    nanosleep(&tw,&tr);

    seekdir(dir,location);
    i=0;
    while ( (entry = readdir(dir)) != NULL) {
        sprintf(fresh[i].filename,"%s", entry->d_name);
        fresh[i].id = entry->d_ino;
        fresh[i].filetype = entry->d_type;
        i++;
    }
    size_curr = i;

    printf("--------------------- \n new \n -------------------- \n");
for(j = 0; j < size_curr; j++)
    printf("%s \n", fresh[j].filename);
printf("---------------------\n old \n -------------------- \n");
for(j = 0; j < size_prev; j++)
    printf("%s \n", old[j].filename);



// Проверка на добавление.
    for(j = 0; j < size_curr; j++)
    {
        flag = 0;
        for(k = 0; k < size_prev; k++)
            {
                if(fresh[j].id == old[k].id)
                    flag = 1;
            }
        if (!flag)
            printf("Добавлен объект типа %s c именем %s \n", dumb_types(fresh[j].filetype), fresh[j].filename);
    }

// Проверка на удаление
    for(j = 0; j < size_prev; j++)
    {
        flag = 0;
        for(k = 0; k < size_curr; k++)
            {
                if(old[j].id == fresh[k].id)
                    flag = 1;
            }
        if (!flag)
            printf("Удален объект типа %s c именем %s \n", dumb_types(old[j].filetype), old[j].filename);
    }
// Переприсваивание

    for(j = 0; j < size_curr; j++)
    {
        sprintf(old[j].filename,"%s", fresh[j].filename);
        old[j].filetype=fresh[j].filetype;
        old[j].id=fresh[j].id;
    }


    size_prev = size_curr;
}

closedir(dir);
return 0;
}
