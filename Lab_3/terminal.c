#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_NUMBER_OF_PARAMS 10

// Разделение введённой команды на аргументы
void parseCmd(char* cmd, char** params)
{
int i;
for(i = 0; i < MAX_NUMBER_OF_PARAMS; i++)
{
    params[i] = strsep(&cmd, " ");
    if(params[i] == NULL) break;
}
}


int executeCmd(char** params)
{
// Fork
pid_t pid = fork();

if (pid == -1)
{
    char* error = strerror(errno);
    printf("fork: %s\n", error);
    return 1;
}

// Child
else if (pid == 0)
{
    // Execute
    execvp(params[0], params);

    // Error
    char* error = strerror(errno);
    printf("shell: %s: %s\n", params[0], error);
    return 0;
}

// Parent
else
{
    // Wait for child
    int childStatus;
    waitpid(pid, &childStatus, 0);
    return 1;
}
}

int main()
{
char cmd[MAX_COMMAND_LENGTH + 1];
char* params[MAX_NUMBER_OF_PARAMS + 1];

while(1)
{
    // Приглашение.
    printf("pew-pew>");

    // Считаем команду.
    if(fgets(cmd, sizeof(cmd), stdin) == NULL) break;

    // Удаляем символы перхода если есть.
    if(cmd[strlen(cmd)-1] == '\n')
    {
        cmd[strlen(cmd)-1] = '\0';
    }
    // Разбивка ввдённой команды.
    parseCmd(cmd, params);

    // Если выход, то выход.
    if(strcmp(params[0], "exit") == 0) break;

    // Выполнение!
    if(executeCmd(params) == 0) break;
}
return 0;
}


