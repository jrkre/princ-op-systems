#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>


/**
 * @brief execute a command on the command line in the INCLUDEPATH using child processes
 *        aand execvp
 * 
 * @param enteredCommand 
 * @param infile 
 * @param outfile 
 * @return int 
 */
int executeCommand(char * const * enteredCommand, 
                    const char * infile, 
                    const char * outfile)
{
    //fork process, creates child process
    pid_t pid = fork();

    //form command & args
    const char* command = enteredCommand[0];
    char * const * argv = &enteredCommand[1];

    if (pid < 0)
    {
        printf("%s\n", "fork Produced an Error: ");
        printf("%s", strerror(errno));
    }

    if (infile == NULL)
    { // case for outfile, since no infile
        int fd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, 0666);
        //dup2 duplicate file descriptor, in this case stdout
        dup2(fd, STDOUT_FILENO);

        printf("%s", command);

        //execute command
        int val = execvp(command, argv);

        if (val < 0)
        {
            printf("%s\n", "execvp Produced an Error: ");
            printf("%s", strerror(errno));
            _exit(val);
            return 1;
        }
    }
    else if (outfile==NULL)
    { // case for infile, since no outfile
        int fd = open(infile, O_RDONLY, 0666);
        //dup2 to stdin
        dup2(fd, STDIN_FILENO);

        //execute command
        int val = execvp(command, argv);

        if (val < 0)
        {
            printf("%s\n", "execvp Produced an Error: ");
            printf("%s", strerror(errno));
            _exit(val);
            return 1;
        }
    }
    else 
    {
        // since there is no infile/outfile, this shouldnt be neccessary
        // int fd = open(infile, O_RDONLY, 0666);
        // //dup2 to stdin
        // dup2(fd, STDIN_FILENO);

        //execute command
        int val = execvp(command, argv);

        if (val < 0)
        {
            printf("%s\n", "execvp Produced an Error: ");
            printf("%s", strerror(errno));
            _exit(val);
            return 1;
        }
    }

    return 0;
}

/**
 * @brief implimentation of cd using chdir
 * 
 * @param path 
 */
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

/**
 * @brief tokenize input into splitWords with strtok
 * 
 */
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

    int arg_list_len = 100;
    int arg_len = 500;
    
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
        char preamble[100] = "\033[35;1m"; //pink username
        strcat(preamble, netid);
        strcat(preamble, "\033[0;37m:\033[0;36m");

        char filepath[100];

        if (getcwd(filepath, arg_len) == NULL)
        {
            printf("%s", "Issue finding cwd");
        }

        strcat(preamble, filepath); // blue filepath
        strcat(preamble, "\033[37;1m$\033[0m");

    
        printf("%s", preamble);

        // grab input 
        fgets(input, arg_list_len, stdin);

        int token_count = parseInput(input, words, arg_list_len);
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
            char ** ptr = (char**) malloc(token_count * sizeof(char*));

            ptr[0] = malloc(arg_len * sizeof(char));

            for (int i = 0; i < token_count; i ++)
            {
                printf("%s\n", words[i]);
                if (!strcmp(words[i], ""))
                {

                    *ptr[i] = *words[i];
                }
            }

            printf("%s\n", "here");

            //TODO: this needs work https://drive.google.com/file/d/1dB95Sc3mq-JWOppqj0LvD9VZQG8sPNFg/view
            for (int i = 0; i < token_count; i ++)
            {
                //const char * nullers = ""; 
                if (*words[i] == '>')
                {
                    char outfile [100];
                    strcpy(outfile, words[i+1]);
                    executeCommand(ptr, NULL, outfile);
                }
                else if (*words[i] == '<')
                {
                    char infile [100];
                    strcpy(infile, words[i+1]);
                    executeCommand(ptr, infile, NULL);
                }
                else
                {
                    executeCommand(ptr, NULL, NULL);
                }
            }
            free(ptr);
        }


    }

    return 0;
}