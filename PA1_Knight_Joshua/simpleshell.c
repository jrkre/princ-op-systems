#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
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


    if (pid == 0) //child process
    {
        //form command & args
        const char* command = enteredCommand[0];
        char * const * argv = &enteredCommand[0];

        if (pid < 0)
        {
            printf("%s\n", "fork Produced an Error: ");
            printf("%s", strerror(errno));
        }

        if (infile == NULL && outfile != NULL)
        { // case for outfile, since no infile, redirect to file provided
            printf("%s\n", "OUTFILE");

            int fd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, 0666);
            //dup2 duplicate file descriptor, in this case stdout
            int dup_return = dup2(fd, STDOUT_FILENO);

            if (dup_return < 0)
            {
                printf("%s\n", "dup2 Produced an Error: ");
                printf("%s", strerror(errno));
                return 1;
            }
            // printf("%s", command);

            //execute command
            int val = execvp(command, argv);

            if (val < 0)
            {
                printf("%s\n", "execvp Produced an Error: ");
                printf("%s\n", strerror(errno));
                _exit(val);
                return 1;
            }
        }
        else if (outfile == NULL && infile != NULL)
        { // case for infile, since no outfile
            printf("%s\n", "INFILE");
            int fd = open(infile, O_RDONLY, 0666);
            //dup2 to stdin
            int dup_return = dup2(fd, STDIN_FILENO);
            
            if (dup_return < 0)
            {
                printf("%s\n", "dup2 Produced an Error: ");
                printf("%s\n", strerror(errno));
                return 1;
            }

            //execute command
            int val = execvp(command, argv);

            if (val < 0)
            {
                printf("%s\n", "execvp Produced an Error: ");
                printf("%s\n", strerror(errno));
                return 1;
            }
            _exit(val);
        }
        else 
        {
            // since there is no infile/outfile, im pretty sure this shouldnt be neccessary
            // int fd = open(infile, O_RDONLY, 0666);
            // //dup2 to stdin
            // dup2(fd, STDIN_FILENO);

            //execute command
            int val = execvp(command, argv);

            if (val < 0)
            {
                printf("%s\n", "execvp Produced an Error: ");
                printf("%s", strerror(errno));
                return 1;
            }
            _exit(val);
        }

        return 0;
    }
    else 
    {
        wait(NULL);
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
    int success;
    if (0 == strcmp(path, "~"))
    {
        const char * new_path = getenv("HOME");

        success = chdir(new_path);

    }
    else 
    {
        success = chdir(path);
    }

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
        token[strcspn(token, "\n")] = 0;
        strcpy(splitWords[counter], token);
        token = strtok(NULL, delimeter);

        counter++;
    }

    return counter;
}

int main(int argc, char **argv)
{

    char* netid = "joshuaknight";

    //char cwd [] = "~";

    int arg_list_len = 100;
    int arg_len = 500;
    
    //testing parseInput()
    

    // char word [] = "one two three four";

    // parseInput(word, words, argc);
    

    // for (int i = 0; i < 4; i++)
    // {
    //     printf("%s", words[i]);
    //     printf("\n");
    // }

    
    while (1) {
        char input[100] = {0};

        char words[100][500];
        
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
        if (!strcmp(words[0], "exit"))
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
                // not needed anymore, taken care of in parseCommand - words[1][strlen(words[1]) - 1] = '\0';
                changeDirectories(words[1]);
                //strcpy(cwd, words[1]);
            }
        }
        else 
        {
            char ** ptr = (char**) malloc((token_count + 1) * sizeof(char)); // +1 for null terminator


            for (int i = 0; i < token_count; i ++)
            {
                ptr[i] = (char *) malloc((arg_len + 1) * sizeof(char)); // +1 for null terminator
                strcpy(ptr[i], words[i]);
            }
            //ptr[token_count] = (char *) malloc(sizeof(char*));

            ptr[token_count] = NULL; // null terminator

            // printf("HEREEEE");


            // for ( int i = 0; i <= token_count; i++)
            // {
            //     printf("%s%i\n", ptr[i], i);
            // }

            //printf("%s\n", "here");

            //TODO: this needs work https://drive.google.com/file/d/1dB95Sc3mq-JWOppqj0LvD9VZQG8sPNFg/view
            for (int i = 0; i < token_count; i ++)
            {
                //const char * nullers = ""; 
                if (*words[i] == '>')
                {
                    char outfile [100];
                    strcpy(outfile, words[i+1]);

                    
                    for (int j = i; j < token_count - 2; j++)
                    {
                        strcpy(words[j], words[j + 2]);
                    }
                    token_count -= 2;


                    executeCommand(ptr, NULL, outfile);
                }
                else if (*words[i] == '<')
                {
                    char infile [100];
                    strcpy(infile, words[i+1]);

                    for (int j = i; j < token_count - 2; j++)
                    {
                        // same for infile
                        strcpy(words[j], words[j + 2]);
                    }
                    token_count -= 2;

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