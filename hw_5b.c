#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define STR_MAX_LENGTH 1000
#define FILENAME_MAX_LENGTH 255
#define MAX_CMD_NUM 255

void split(char* string, char* delimiters, char*** token_ptr, int* tokensCount);
int read_the_string(FILE* opened_file, char* file_input);


int main()
{
  pid_t pid = 1;
  int tokens_count = 0;
  int is_it_EOF = 0;
  int line_counter = 1;
  int wait_arg = 0;
  char** word_ptr = (char**)calloc(MAX_CMD_NUM, sizeof(char*));
  char filename[FILENAME_MAX_LENGTH] = {};
  char current_command[STR_MAX_LENGTH] = {};
  
  printf("Enter please a filename with its extension\n");
  scanf("%s", filename);

  FILE* opened_file = fopen(filename, "r");
  assert(opened_file);

  while(!is_it_EOF)
  {
    if(pid != 0)
    { 
      tokens_count = 0;

      /*
       * Почему не fgets?
       */
      is_it_EOF = read_the_string(opened_file, current_command);

      split(current_command, " ", &word_ptr, &tokens_count);
     
      pid = fork();
    }
    else if (pid == 0)
    {
      int wait_time = atoi(word_ptr[0]);
      printf("wait time is %d\n", wait_time);
      sleep(wait_time);
      execvp(word_ptr[1], word_ptr + 1); 
    }

  }
  
  fclose(opened_file);
}

int read_the_string(FILE* opened_file, char* file_input)
{
  int counter = -1;
  int ret_val = 0;

  do
  {
    counter++;
    file_input[counter] = fgetc(opened_file);  
  }
  while (file_input[counter] != '\n' && file_input[counter] != EOF);
        
  ret_val = (EOF == file_input[counter]) ? 1 : 0; 
  
  file_input[counter] = '\0';
  
  return  ret_val;
}

void split(char* string, char* delimiters, char*** token_ptr, int* tokensCount)
{
    char** token = *token_ptr;
    int counter = 0;
    token[counter] = strtok(string, delimiters);
    
    while(token[counter])
    {
      counter++;
      token[counter] = strtok(NULL, delimiters);
    }
    
    *tokensCount = counter;
}
