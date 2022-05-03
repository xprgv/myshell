#include <dirent.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define SPLIT_SYMBOLS " \n\t\a\r"

// terminal colors
#define RESET "\033[0m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[0;34m"
#define GREEN "\u001b[32m"
#define WHITE "\u001b[37m"
#define MAGENTA "\u001b[35m"
#define CYAN "\u001b[36m"

char* commands[] = {"exit",  "ls",    "cd",   "touch", "rm",
                    "mkdir", "rmdir", "help", "clear"};

#define COUNT_OF_COMMANDS sizeof(commands) / sizeof(char*)

char* read_line() {
    int buffsize = BUFFER_SIZE;
    int position = 0;
    char* buffer = (char*)malloc(sizeof(BUFFER_SIZE));
    int c;

    if (buffer == NULL) {
        perror("error in read_line (memory not allocated)");
        exit(-1);
    }

    while (1) {
        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else
            buffer[position] = c;

        position++;
        if (position >= buffsize) {
            buffsize = buffsize + BUFFER_SIZE;
            buffer = realloc(buffer, buffsize);
            if (buffer == NULL) {
                perror("error in read_line (memory not allocated)");
                exit(-1);
            }
        }
    }
}

char** split_line(char* line) {
    int buffsize = BUFFER_SIZE;
    int position = 0;

    char** tokens = (char**)malloc(sizeof(char*) * buffsize);
    if (tokens == NULL) {
        perror("error in split_line (memory not allocated)");
        exit(-1);
    }

    char* token = strtok(line, SPLIT_SYMBOLS);
    while (token != NULL) {
        tokens[position] = token;
        position++;
        if (position >= buffsize) {
            buffsize = buffsize + BUFFER_SIZE;
            tokens = realloc(tokens, buffsize);

            if (tokens == NULL) {
                perror("error in split_line (memory not allocated");
                exit(-1);
            }
        }
        token = strtok(NULL, SPLIT_SYMBOLS);
    }
    tokens[position] == NULL;
    return tokens;
}

int shell_exit(char** args) {
    return 0;
}

int shell_ls(char** args) {
    struct dirent** namelist;
    int n = scandir(".", &namelist, NULL, alphasort);
    if (n == -1) {
        perror("scandir");
        return 0;
    }

    struct stat buffer;

    while (n--) {
        if (n <= 1)
            break;

        if (stat(namelist[n]->d_name, &buffer) == 0 && S_ISDIR(buffer.st_mode))
            printf("%s%s%s  ", YELLOW, namelist[n]->d_name, RESET);
        else if (stat(namelist[n]->d_name, &buffer) == 0 &&
                 S_ISREG(buffer.st_mode))
            printf("%s  ", namelist[n]->d_name);

        free(namelist[n]);
    }

    printf("\n");
    return 1;
}

int shell_cd(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected position argument\n");
    } else if (chdir(args[1]))
        perror("shell");

    return 1;
}

int shell_touch(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Error: Excepted argument to \"touch\"\n");
        return 1;
    }

    int fd = open(args[1], O_WRONLY);
    if (fd != -1) {
        fprintf(stderr, "Error: file %s already exits\n", args[1]);

        if (close(fd) != 0)
            fprintf(stderr,
                    "Error: Existing file %s was not closed correctly\n",
                    args[1]);

        return 1;
    }

    fd = creat(args[1], O_CREAT | O_TRUNC | S_IRWXU | S_IRWXG);
    if (fd == -1) {
        fprintf(stderr, "Error: Coudn't create file %s\n", args[1]);
        return 1;
    }

    return 1;
}

int shell_rm(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Error: Excepted argument to \"rm\"\n");
        return 1;
    }

    if (unlink(args[1]) == -1) {
        fprintf(stderr, "Error: Couldn't remove file %s\n", args[1]);
        return 1;
    }

    return 1;
}

int shell_mkdir(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Error: Expected argument to \"mkdir\"\n");
        return 1;
    }

    struct stat buffer;
    if (stat(args[1], &buffer) == 0 && S_ISDIR(buffer.st_mode)) {
        fprintf(stderr, "Error: Can't create directory: already exist\n");
        return 1;
    }

    if (mkdir(args[1], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
        fprintf(stderr, "Error: Can't create directory.\n");
        return 1;
    }

    return 1;
}

int shell_rmdir(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Error: Excepted argument to \"rmdir\"\n");
        return 1;
    }

    if (rmdir(args[1]) != 0) {
        fprintf(stderr, "Error: Can't remove directory: no such directory\n");
        return 1;
    }

    return 1;
}

int shell_help(char** args) {
    printf("List of contained commands\n");
    for (int i = 0; i < COUNT_OF_COMMANDS; i++)
        printf("%d) %s\n", i, commands[i]);

    printf("Also you can use a standart linux utilities (apt, man or e.t.c)\n");
    return 1;
}

int shell_clear(char** args) {
    system("clear");
    return 1;
}

int (*shell_functions[])(char**) = {&shell_exit,  &shell_ls,   &shell_cd,
                                    &shell_touch, &shell_rm,   &shell_mkdir,
                                    &shell_rmdir, &shell_help, &shell_clear};

int execute(char** args) {
    int status;
    pid_t pid, wait_pid;

    pid = fork();
    switch (pid) {
        case -1:
            // error
            perror("shell");

        case 0:
            // child process
            if (execvp(args[0], args) == -1)
                perror("shell");

        default:
            // parent process
            do {
                wait_pid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int launch(char** args) {
    if (args[0] == NULL) {
        // empty command
        return 1;
    }

    for (int i = 0; i < COUNT_OF_COMMANDS; i++) {
        if (!strcmp(args[0], commands[i]))
            return (*shell_functions[i])(args);
    }

    return execute(args);
}

void shell_loop() {
    system("clear");
    char hostname[_SC_HOST_NAME_MAX + 1];
    gethostname(hostname, _SC_HOST_NAME_MAX + 1);

    char pathname[PATH_MAX + 1];

    char* line;
    char** arguments;

    int status = 1;
    while (status) {
        getcwd(pathname, PATH_MAX + 1);
        printf("%s%s%s:%s%s%s$ ", RED, hostname, RESET, BLUE, pathname, RESET);

        line = read_line();
        arguments = split_line(line);

        status = launch(arguments);

        free(line);
        free(arguments);
    }
}

int main() {
    shell_loop();

    return 0;
}