/*
 * Ben Hodsdon; Daniel Masker
 * CS3411
 * 3/16/2022
 *
 * Scanner for the IPC Project.
 */

// Public variables so they can be access from the signal handler.
int even_out;
int odd_out;
char* buffer;

// Handler for termination signal.
void sigterm_handler(int sig_num)
{
    // Print out the even words.
    if(write(1, "Words with even letters:\n") == -1) exit(-1);
    int read_result = read(even_out, buffer, 1024);
    while(read_result != 0)
    {
        if(read_result == -1)
        {
            exit(-1);
        }
        if(write(1, buffer, read_result) == -1) exit(-1);
        read_result = read(even_out, buffer, 1024);
    }

    // Close the even pipe.
    if(close(even_out) == -1) exit(-1);

    // Read the odd words.
    if(write(1, "Words with odd letters:\n") == -1) exit(-1);
    read_result = read(odd_out, buffer, 1024);
    while(read_result != 0)
    {
        if(read_result == -1)
        {
            exit(-1);
        }
        if(write(1, buffer, read_result) == -1) exit(-1);
        read_result = read(odd_out, buffer, 1024);
    }

    // Close the odd pipe.
    if(close(odd_out) == -1) exit(-1);

    // Exit with good code.
    exit(0);
}

// Main entry point for the program.
int main(int argc, char** argv)
{
    // Usage: ./scanner <evenIn> <evenOut> <oddIn> <oddOut>

    if (argc < 5) return -1;

    // Parse the input pipes locations.
    int even_in = atoi(argv[1]);
    even_out = atoi(argv[2]);
    int odd_in = atoi(argv[3]);
    odd_out = atoi(argv[4]);

    // Create initial buffer for scanning the word.
    int working_word_size = 1024;
    char* working_word = malloc(working_word_size);
    if(working_word == NULL)
    {
        return -1;
    }

    // Create buffer for reading input.
    int current_word_index = 0;
    buffer = malloc(1024);
    if(buffer == NULL)
    {
        return -1;
    }

    // Setup the signal handler.
    struct sigaction act;

    act.sa_handler = sigterm_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGTERM, &act, NULL);

    // Read all input.
    int read_return_value = read(stdin, buffer, 1024);
    while(read_return_value != 0)
    {
        // Check for read failure.
        if(read_return_value == -1)
        {
            return -1;
        }
        // Iterate over the letter input.
        for(int i = 0; i < read_return_value; i++)
        {
            // Get the current character.
            char cur_char = buffer[i];

            // Check if the character is a word separater.
            if(cur_char == '\t' || cur_char == ' ' || cur_char == '\n')
            {
                // If we haven't started on a word, skip to the next.
                if(current_word_index == 0) continue;

                // Handle removed punctuation.
                int copy_distance = 0;

                // Clean up the completed word.
                for(int j = 0; j < current_word_index; j++)
                {
                    if(ispunct(working_word[j]))
                    {
                        copy_distance++;
                        continue;
                    }
                    working_word[j-copy_distance] = 
                        tolower(working_word[j]);
                }

                // Get the correct pipe to write to.
                int write_location;
                if((current_word_index - copy_distance) % 2 == 0)
                {
                    write_location = even_in;
                }
                else
                {
                    write_location = odd_in;
                }

                // Write the word to the pipe.
                if(write(write_location, working_word,
                        current_word_index - copy_distance) == -1)
                {
                    return -1;
                }

                // Reset the current word index.
                current_word_index = 0;
            }
            else
            {
                // Ensure the word buffer is large enough.
                if(working_word_size == current_word_index)
                {
                    // Double the size of the buffer.
                    working_word_size *= 2;
                    char* temp = malloc(working_word_size);
                    if(temp == NULL)
                    {
                        return -1;
                    }
                    for(int j = 0; j < current_word_index; j++)
                    {
                        temp[j] = working_word[j];
                    }
                    free(working_word);
                    working_word = temp;
                }
                // Write the character to the word.
                working_word[current_word_index++] = cur_char;
            }
        }

        // Read the next set of characters.
        int read_return_value = read(stdin, buffer, 1024);
    }

    // Free our word buffer.
    free(working_word);
    if(close(even_in) == -1)
    {
        return -1;
    }

    // Close the input pipes.
    if(close(odd_in) == -1)
    {
        return -1;
    }
    
    // Write a '*' every second.
    while(true)
    {
        sleep(1);
        write(1, "*", 1);
    }

    // Should never reach this point,
    // So return bad value.
    return -1;
}

