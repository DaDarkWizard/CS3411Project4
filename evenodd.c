/*
 * Ben Hodsdon; Dasker Masker
 * CS3411
 * 3/16/2022
 *
 * Parses input passed in from the scanner.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>


// Entry for the word linked list.
struct entry
{
    char* word;
    int count;
    struct entry *next;
};

// Ensure the size of an array is greater than the amount used.
char* ensure_size(char* buffer, int used, int* size)
{
    while(used >= *size)
    {
        char* new_buffer = malloc(*size * 2);
        if(new_buffer == NULL) return NULL;
        for(int i = 0; i < used; i++)
        {
            new_buffer[i] = buffer[i];
        }
        free(buffer);
        buffer = new_buffer;
        *size = *size * 2;
    }
    return buffer;
}

// Main entry point for the program.
int main(int argc, char** argv){

    // Dictionary start variable.
    struct entry *dict_start;

    // Setup our buffer.
    char*  buf = malloc(1024);
    if(buf == NULL) return -1;
    
    // Setup the word buffer.
    int size = 1024;
    int index = 0;
    char* word = malloc(1024);
    if(word == NULL) return -1;

    // Read in the words from the pipe.
    int read_result = read(0, buf, 1024);
    while(read_result != 0)
    {
        if(read_result == -1) return -1;
        // Clean out the buffer.
        for(int i = 0; i < read_result; i++)
        {
            // Check for word end.
            if(buf[i] == ' ')
            {
                if(index > 0)
                {
                    // Write word to table.
                    word = ensure_size(word, index, &size);
                    if(word == NULL) return -1;
                    word[index] = '\0';
                    index++;
                    struct entry *dict_entry = dict_start;
                
                    // Find a match in the list.
                    while(dict_entry != NULL && dict_entry->next != NULL)
                    {
                        if(strcmp(dict_entry->word, word) == 0)
                        {
                            dict_entry->count++;
                            break;
                        }
                        dict_entry = dict_entry->next;
                    }

                    
                    if(dict_entry == NULL) // Put the first entry in the list.
                    {
                        char* new_word = malloc(strlen(word) + 1);
                        if(new_word == NULL) return -1;
                        strcpy(new_word, word);
                        struct entry *new_entry = malloc(sizeof(struct entry));
                        if(new_entry == NULL) return -1;
                        new_entry->word = new_word;
                        new_entry->count = 1;
                        new_entry->next = NULL;
                        dict_start = new_entry;
                    }
                    else if(dict_entry->next == NULL) // Append the word to the list.
                    {
                        if(strcmp(dict_entry->word, word) == 0)
                        {
                            dict_entry->count++;
                        }
                        else
                        {
                            char* new_word = malloc(strlen(word) + 1);
                            if(new_word == NULL) return -1;
                            strcpy(new_word, word);
                            struct entry *new_entry = malloc(sizeof(struct entry));
                            if(new_entry == NULL) return -1;
                            new_entry->word = new_word;
                            new_entry->count = 1;
                            new_entry->next = NULL;
                            dict_entry->next = new_entry;
                        }
                    }
                    index = 0;
                }
            }
            else
            {
                word = ensure_size(word, index, &size);
                if(word == NULL) return -1;
                word[index] = buf[i];
                index++;
            }
        }

        read_result = read(0, buf, 1024);
    }

    // Get the last word (if any) from the buffer.
    if(read_result == 0 && index > 0)
    {
        word = ensure_size(word, index, &size);
        if(word == NULL) return -1;
        word[index] = '\0';
        struct entry *dict_entry = dict_start;
        if(dict_entry == NULL) // Put the first entry in the list.
        {
            dict_start = malloc(sizeof(struct entry));
            dict_start->next = NULL;
            dict_start->count = 1;
            dict_start->word = malloc(sizeof(word) + 1);
            if(dict_start->word == NULL) return -1;
            strcpy(dict_start->word, word);
        }
        else
        {
            while(dict_entry->next != NULL)
            {
                if(strcmp(dict_entry->word, word) == 0)
                {
                    dict_entry->count++;
                    break;
                }
                dict_entry = dict_entry->next;
            }
            if(dict_entry->next == NULL)
            {
                if(strcmp(dict_entry->word, word) == 0)
                {
                    dict_entry->count++;
                }
                else
                {
                    struct entry *new_entry = malloc(sizeof(struct entry));
                    if(new_entry == NULL) return -1;
                    new_entry->next = NULL;
                    new_entry->word = malloc(sizeof(word) + 1);
                    if(new_entry->word == NULL) return -1;
                    new_entry->count = 1;
                    strcpy(new_entry->word, word);
                    dict_entry->next = new_entry;
                }
            }
        }
    }

    // Sleep for 10ish seconds.
    sleep(10);0

    // Signal scanner if it's even.
    if(strcmp(argv[0], "./even") == 0)
    {
        if(kill(atoi(argv[1]), SIGTERM) == -1)
        {
            return -1;
        }
    }

    // Output all characters to the scanner.
    char number_buffer[20];
    struct entry *dict_entry = dict_start;
    struct entry *last_entry = NULL;
    while(dict_entry != NULL)
    {
        if(last_entry != NULL)
        {
            free(last_entry->word);
            free(last_entry);
            last_entry = NULL;
        }
        if(write(1, dict_entry->word, strlen(dict_entry->word)) == -1) return -1;
        sprintf(number_buffer, "\t%d\n\t", dict_entry->count);
        if(write(1, number_buffer, strlen(number_buffer)) == -1) return -1;
        last_entry = dict_entry;
        dict_entry = dict_entry->next;
    }
    if(last_entry != NULL)
    {
        free(last_entry->word);
        free(last_entry);
        last_entry = NULL;
    }
    free(buf);
    free(word);

    // Clean return.
    return 0;
}
