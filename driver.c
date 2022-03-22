/*
 * Ben Hodsdon; Daniel Masker
 * CS3411
 * 3/16/2022
 *
 * Driver for the IPC Program.
 */




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
    // TODO: Open file.
    
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
        close_descs(2, allpipes);
        return -1;
    }
    
    if(pipe(oddInputPipe))
    {
        close_descs(4, allpipes);
        return -1;
    }

    if(pipe(oddOutputPipe))
    {
        close_descs(6, allpipes);
        return -1;
    }

    if(pipe(scannerInputPipe))
    {
        close_descs(8, allpipes);
        return -1;
    }

    // TODO: Create Even Process.
    
    pid_t fork_result = fork();
    if(fork_result == -1)
    {
        close_descs(10, allpipes);
        return -1;
    }
    else if(fork_result == 0)
    {
        if(close_descs(6, &allpipes[4]))
        {
            close_descs(4, allpipes);
            return -1;
        }
        if(close(evenInputPipe[1]) == -1)
        {
            close(evenInputPipe[0]);
            close_descs(2, &allpipes[2]);
            return -1;
        }
        if(close(evenOutputPipe[0]) == -1)
        {
            close(evenInputPipe[0]);
            close(evenOutputPipe[1]);
            return -1;.
        }
        close(0);
        dup(evenInputPipe[0]);
        close(evenInputPipe[0]);

        close(1);
        dup(evenOutputPipe[1]);
        close(evenOutputPipe[1]);


    }
    // TODO: Create Odd Process.
    // TODO: Create Scanner Process.
    // TODO: Pipe file input to Scanner Process.
    // TODO: Close Scanner Process stdin.
    // TODO: Gracefully wait for children to exit.
    return -1;
}
