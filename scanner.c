/*
 * Ben Hodsdon; Daniel Masker
 * CS3411
 * 3/16/2022
 *
 * Scanner for the IPC Project.
 */

int main(int argc, char** argv)
{
    // Usage: ./scanner <evenIn> <evenOut> <oddIn> <oddOut>

    if (argc < 5) return -1;

    int even_in = atoi(argv[1]);
    int even_out = atoi(argv[2]);
    int odd_in = atoi(argv[3]);
    int odd_out = atoi(argv[4]);

    int working_word_size = 1024;
    char* working_word = malloc(working_word_size);
    if(working_word == NULL)
    {
        return -1;
    }

    int current_word_index = 0;
    char* buffer = malloc(1024);

    int read_return_value = read(stdin, buffer, 1024);
    while(read_return_value != 0)
    {
        if(read_return_value == -1)
        {
            return -1;
        }
        for(int i = 0; i < read_return_value; i++)
        {
            char cur_char = buffer[i];
            if(cur_char == '\t' || cur_char == ' ' || cur_char == '\n')
            {
                if(current_word_index == 0) continue;
                int copy_distance = 0;
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
                int write_location;
                if((current_word_index - copy_distance) % 2 == 0)
                {
                    write_location = even_in;
                }
                else
                {
                    write_location = odd_in;
                }
                if(write(write_location, working_word,
                        current_word_index - copy_distance) == -1)
                {
                    return -1;
                }
                current_word_index = 0;
            }
            else
            {
                if(working_word_size == current_word_index)
                {
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
                working_word[current_word_index++] = cur_char;
            }
        }
        int read_return_value = read(stdin, buffer, 1024);
    }
    if(close(even_in) == -1)
    {
        return -1;
    }
    if(close(odd_in) == -1)
    {
        return -1;
    }
    

    return -1;
}
