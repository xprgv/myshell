#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/dir.h>


#define ARG_LEN 16 
#define SPLIT_SYMBOLS " \t\r\n\a"
char **shell_split_line(char *line) 
{
    char **arguments = malloc(sizeof(char *) * ARG_LEN);
    char *argument = NULL;
    int position = 0;

    if(!arguments)
    {
        fprintf(stderr, "memory allocation error!");
        exit(-1);
    }

    argument = strtok(line, SPLIT_SYMBOLS);
    while(argument != NULL)
    {
        arguments[position] = argument;
        position++;
        argument = strtok(NULL, SPLIT_SYMBOLS);
    }
    arguments[position] = NULL;
    return arguments;
}


#define BUFFSIZE 1024
char *shell_read_line()
{
    char *line = malloc(sizeof(char) * BUFFSIZE);
    int line_length = BUFFSIZE;
    int position = 0;

    if(!line)
    {
        fprintf(stderr, "memory allocation error!");
        exit(-1);
    }

    while(1)
    {
        line[position] = getchar();
        if(line[position] == '\n')
            return line;
        else
            position++;
    }
}


int shell_launch(char **arguments)
{
    pid_t pid, wait_pid;
    int status;

   pid = fork();
    if(pid == 0)
    {
        // child process
        if(execvp(arguments[0], arguments) == -1)
            perror("shell");
    }
    else if(pid > 0)
    {
        // parent process
        do
        {
            wait_pid = waitpid(pid, &status, WUNTRACED);        
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    else
    {
        // error in creating process
        perror("shell");
    }
    return 1;
}


int shell_exit(char **args)
{
    return 0;
}


#define MAX_PATH_LEN 1024
int shell_ls(char **args)
{
    char pathname[MAX_PATH_LEN];
    int file_count;
    struct direct **files;
    if(getwd(pathname) == NULL)
    {
        fprintf(stderr, "Error getting path");
    }
    printf("Current working directory: %s", pathname);
    
    // edit syscall to get files and folders in current directory

    return 1;
}


int shell_cd(char **args)
{
    if(args[1] == NULL)
    {
        fprintf(stderr, "shell: expected position argument\n");
    }
    else if(!chdir(args[1]))
    {
        perror("shell");
    }
    return 1;
}


char *shell_commands[] = 
{
    "exit",
    "dir",
    "cd"
};


int (*shell_functions[]) (char **) = 
{
    &shell_exit,
    &shell_ls,
    &shell_cd
};



int shell_execute(char **args)
{
    if(args[0] == NULL)
    {
        // empty command
        return 1;
    }    
    for(int i = 0; i < (sizeof(shell_commands) / sizeof(char *)); i++)
    {
        if(!strcmp(args[0], shell_commands[i]))
        {
            return (*shell_functions[i]) (args);
        }
    }
    return shell_launch(args);
}


void shell_loop(void)
{
    char *line;
    char **arguments;
    
    int status = 1;
    while(status)
    { 
        printf("> ");
        line = shell_read_line();

        //  printf("line: %s\n", line);

        arguments = shell_split_line(line);

        /*
        int i = 0;
        while(arguments[i] != NULL)
        {
            printf("arg[%d]: %s\n", i, arguments[i]);
            i++;
        }
        */

        status = shell_execute(arguments);
        free(line);
        free(arguments);
    }
}


int main(int argc, char *argv[])
{
    shell_loop();

    return 0; 
}



