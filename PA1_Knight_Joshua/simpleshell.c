#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>


int executeCommand(char * const * enteredCommand, 
                    const char * infile, 
                    const char * outfile)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        printf("%s", "fork Produced an Error: ");
        printf("%s", strerror(errno));
    }

    if (!strcmp(infile, ""))
    { // case for outfile, since no infile
        int fd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, 0666);
        dup2(fd, STDOUT_FILENO);

        // form command

        const char* command = enteredCommand[0];
        char * const * argv = enteredCommand[1];

        printf("%s", command);

        int val = execvp(command,argv);
        if (val < 0)
        {
            printf("%s", "execvp Produced an Error: ");
            printf("%s", strerror(errno));
            _exit(val);
            return 1;
        }
    }
    else if (!strcmp(outfile, ""))
    { // case for infile, since no outfile
        int fd = open(infile, O_RDONLY, 0666);
        dup2(fd, STDIN_FILENO);

        // form command

        const char* command = enteredCommand[0];
        char * const * argv = enteredCommand[1];

        int val = execvp(command, argv);
        if (val < 0)
        {
            printf("%s", "execvp Produced an Error: ");
            printf("%s", strerror(errno));
            _exit(val);
            return 1;
        }
    }

    return 0;
}

void changeDirectories(const char * path)
{
    int success = chdir(path);
    if (success == 0)
    {
        // printf("%s", path);
        return;
    }
    else
    {
        char error [200] = "chdir Failed: ";
        strcat(error, strerror(errno));

        printf("%s\n",error);
        return;
    }
}

//
int parseInput(char * input,
                char splitWords[][500],
                int maxWords)
{
    char * token;
    const char delimeter[1] = " ";
    int counter = 0;
    token = strtok(input, delimeter);


    while(token != NULL && counter < maxWords)
    {
        // printf("%s\n", token);
        strcpy(splitWords[counter], token);
        token = strtok(NULL, delimeter);
        counter++;
    }

    return counter;
}

int main(int argc, char **argv)
{

    char netid [] = "joshuaknight";

    //char cwd [] = "~";
    
    //testing parseInput()
    char words[100][500] = {0};

    // char word [] = "one two three four";

    // parseInput(word, words, argc);
    

    // for (int i = 0; i < 4; i++)
    // {
    //     printf("%s", words[i]);
    //     printf("\n");
    // }

    
    while (1) {
        char input[100] = {0};
        
        // construct preamble w/ color flare UwU
        char preamble[500] = "\033[35;1m"; //pink username
        strcat(preamble, netid);
        strcat(preamble, "\033[0;37m:\033[0;36m");

        char filepath[500];

        if (getcwd(filepath,500) == NULL)
        {
            printf("%s", "Issue finding cwd");
        }

        strcat(preamble, filepath); // blue filepath
        strcat(preamble, "\033[37;1m$\033[0m");

    
        printf("%s", preamble);

        // grab input 
        fgets(input, 100, stdin);

        parseInput(input, words, 100);
        // printf("%s", input);
        if (!strcmp(words[0], "exit\n"))
        {
            return 0;
        }
        else if (!strcmp(words[0], "cd"))
        {
            if (strcmp(words[2], ""))
            {
                printf("%s", "\033[31mPath Not Formatted Correctly!\033[0m");
            }
            else
            {
                // strcat(filepath, words[1]);

                // replace trailing \n from enter with \0 eof char
                words[1][strlen(words[1]) - 1] = '\0';
                changeDirectories(words[1]);
                //strcpy(cwd, words[1]);
            }
        }
        else 
        {

            //TODO: this needs work https://drive.google.com/file/d/1dB95Sc3mq-JWOppqj0LvD9VZQG8sPNFg/view
            // for (int i = 0; i < 100; i ++)
            // {
            //     const char * nullers = ""; 
            //     if (*words[i] == '>')
            //     {
            //         int executeCommand(words, nullers, words[i+1]);
            //     }
            //     else if (*words[i] == '<')
            //     {
            //         executeCommand(words, words[i-1], nullers);
            //     }
            // }
        }


    }

    return 0;
}