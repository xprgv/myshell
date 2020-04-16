#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/wait.h>
#include <dirent.h>


#define COUNT_OF_COMMANDS sizeof(commmands) / sizeof(char *)

#define BUFFER_SIZE 1024
#define SPLIT_SYMBOLS " \n\t\a\r"



char *commands[] =
{
    "exit",
    "cd",
    "ls"
};

char *read_line()
{
    int buffsize = BUFFER_SIZE;
    int position = 0;
    char *buffer = (char *) malloc(sizeof(BUFFER_SIZE));
    int c;

    if(buffer == NULL)
    {
        perror("error in read_line (memory not allocated)");
        exit(-1);
    }

    while(1)
    {
        c = getchar();
        if(c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        }
        else
            buffer[position] = c;

        position++;

        if(position >= buffsize)
        {
            buffsize = buffsize + BUFFER_SIZE;
            buffer = realloc(buffer, buffsize);
            if(buffer == NULL)
            {
                perror("error in read_line (memory not allocated)");
                exit(-1);
            }
        }
    }
}

char **split_line(char *line)
{
    int buffsize = BUFFER_SIZE;
    int position = 0;
    
    char **tokens = (char **) malloc(sizeof(char *) * buffsize);
    if(tokens == NULL)
    {
        perror("error in split_line (memory not allocated)");
        exit(-1);
    }

    char *token = strtok(line, SPLIT_SYMBOLS);
    while(token != NULL)
    {
        tokens[position] = token;
        position++;
        if(position >= buffsize)
        {
            buffsize = buffsize + BUFFER_SIZE;
            tokens = realloc(tokens, buffsize);
            if(tokens == NULL)
            {
                perror("error in split_line (memory not allocated");
                exit(-1);
            }
        }

        token = strtok(NULL, SPLIT_SYMBOLS);
    }
    tokens[position] == NULL;
    return tokens;
}


int main()
{

    return 0;
}