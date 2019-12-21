#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


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


int shell_execute(char **args)
{
    if(args[0] == NULL)
    {
        // empty command
        return 1;
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



