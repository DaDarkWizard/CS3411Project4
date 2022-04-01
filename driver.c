/*
 * Ben Hodsdon; Daniel Masker
 * CS3411
 * 3/16/2022
 *
 * Driver for the IPC Program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

int close_descs(int amount, int* fds)
{
    int output = 0;
    for(int i = 0; i < amount; i++)
    {
        if(close(fds[i]) == -1)
        {
            output = -1;
        }
    }
    return output;
}

int main(int argc, char** argv)
{
    // Open file.
    int input_file = open(argv[1], O_RDONLY);
    if(input_file == -1)
    {
        return -1;
    }

    // Create variables to access all pipes and macros to access it for easy cleanup.
    int allpipes[10];
    int* evenInputPipe = allpipes;
    int* evenOutputPipe = &allpipes[2];
    int* oddInputPipe = &allpipes[4];
    int* oddOutputPipe = &allpipes[6];
    int* scannerInputPipe = &allpipes[8];

    if(pipe(evenInputPipe))
    {
        return -1;
    }
    
    if(pipe(evenOutputPipe))
    {
        return -1;
    }
    
    if(pipe(oddInputPipe))
    {
        return -1;
    }

    if(pipe(oddOutputPipe))
    {
        return -1;
    }

    if(pipe(scannerInputPipe))
    {
        return -1;
    }

    
    // Create Scanner Process.
    int fork_result = fork();
    if(fork_result == -1)
    {
        return -1;
    }
    else if(fork_result == 0)
    {
        if(close(evenInputPipe[0]) == -1)
        {
            return -1;
        }
        if(close(evenOutputPipe[1]) == -1)
        {
            return -1;
        }
        if(close(oddInputPipe[0]) == -1)
        {
            return -1;
        }
        if(close(oddOutputPipe[1]) == -1)
        {
            return -1;
        }
        if(close(scannerInputPipe[1]) == -1)
        {
            return -1;
        }

        if(close(0) == -1)
        {
            return -1;
        }

        if(dup(scannerInputPipe[0]) == -1)
        {
            return -1;
        }

        if(close(scannerInputPipe[0]) == -1)
        {
            return -1;
        }

        char** args = malloc(sizeof(char*) * 6);
        if(args == NULL)
        {
            return -1;
        }

        args[0] = malloc(10);
        if(args[0] == NULL)
        {
            return -1;
        }
        for(int i = 1; i < 5; i++)
        {
            args[i] = malloc(10);
            if(args[i] == NULL)
            {
                return -1;
            }
        }

        strcpy(args[0], "./scanner");
        sprintf(args[1], "%d", evenInputPipe[1]);
        sprintf(args[2], "%d", evenOutputPipe[0]);
        sprintf(args[3], "%d", oddInputPipe[1]);
        sprintf(args[4], "%d", oddOutputPipe[0]);
        args[5] = NULL;

        execvp(args[0], args);
        return -1;
    }

    int scanner_id = fork_result;

    // Create Even Process.
    
    fork_result = fork();
    if(fork_result == -1)
    {
        return -1;
    }
    else if(fork_result == 0)
    {
        if(close_descs(6, &allpipes[4]))
        {
            return -1;
        }
        if(close(evenInputPipe[1]))
        {
            return -1;
        }
        if(close(evenOutputPipe[0]))
        {
            return -1;
        }
        
        if(close(0))
        {
            return -1;
        }
        if(dup(evenInputPipe[0]))
        {
            return -1;
        }
        if(close(evenInputPipe[0]))
        {
            return -1;
        }

        if(close(1) == -1)
        {
            return -1;
        }

        if(dup(evenOutputPipe[1]) == -1)
        {
            return -1;
        }

        if(close(evenOutputPipe[1]))
        {
            return -1;
        }

        char** args = malloc(sizeof(char*) * 3);
        if(args == NULL)
        {
            return -1;
        }

        args[2] = NULL;
        args[0] = malloc(7);
        if(args[0] == NULL)
        {
            return -1;
        }
        args[1] = malloc(15);
        if(args[1] == NULL)
        {
            return -1;
        }
        strcpy(args[0], "./even");
        sprintf(args[1], "%d", scanner_id);

        execvp(args[0], args);
        return -1;
    }
    
    // Create Odd Process.
    fork_result = fork();
    if(fork_result == -1)
    {
        return -1;
    }
    else if(fork_result == 0)
    {
        if(close_descs(2, &allpipes[8]))
        {
            return -1;
        }
        if(close_descs(4, allpipes))
        {
            return -1;
        }
        if(close(oddInputPipe[1]))
        {
            return -1;
        }
        if(close(oddOutputPipe[0]))
        {
            return -1;
        }
        
        if(close(0))
        {
            return -1;
        }
        if(dup(oddInputPipe[0]))
        {
            return -1;
        }
        if(close(oddInputPipe[0]))
        {
            return -1;
        }

        if(close(1) == -1)
        {
            return -1;
        }

        if(dup(oddOutputPipe[1]) == -1)
        {
            return -1;
        }

        if(close(oddOutputPipe[1]))
        {
            return -1;
        }

        char** args = malloc(sizeof(char*) * 3);
        if(args == NULL)
        {
            return -1;
        }

        args[2] = NULL;
        args[0] = malloc(6);
        if(args[0] == NULL)
        {
            return -1;
        }
        args[1] = malloc(2);
        if(args[1] == NULL)
        {
            return -1;
        }
        strcpy(args[0], "./odd");
        args[1][0] = '1';
        args[1][1] = '\0';

        execvp(args[0], args);
        return -1;
    }



    // Close extra descriptors.
    if(close_descs(8, allpipes) == -1)
    {
        return -1;
    }
    if(close(scannerInputPipe[0]) == -1)
    {
        return -1;
    }


    // Pipe file input to Scanner Process.
    char* buffer = malloc(1024);
    if(buffer == NULL)
    {
        return -1;
    }
    int bytes_read = read(input_file, buffer, 1024);
    while(bytes_read > 0)
    {
        if(write(scannerInputPipe[1], buffer, bytes_read) == -1)
        {
            return -1;
        }
        bytes_read = read(input_file, buffer, 1024);
    }
    if(bytes_read == -1)
    {
        return -1;
    }

    // Close Scanner Process stdin.
    if(close(scannerInputPipe[1]) == -1)
    {
        return -1;
    }
    // Gracefully wait for children to exit.
    wait(NULL);
    wait(NULL);
    wait(NULL);
    return 0;
}
