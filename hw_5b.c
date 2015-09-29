#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main()
{

   printf("Enter please a filename with its extension\n");
   char filename[255] = {};
   scanf("%s", filename);
   
   char** line_ptr[255] = {}; 
   char* commands_ptr[255] = {}; 
   char file_input[1000] = {};
   
   FILE* opened_file = fopen(filename, "r");
   assert(opened_file);
   
   char ch = 1;
   int counter = 0;
   int commands_counter = 1;
   int line_counter = 1;
   
   line_ptr[0] = commands_ptr;
   commands_ptr[0] = file_input; 

   do
   {
     file_input[counter] = fgetc(opened_file);  
   }
   while (file_input[counter++] != EOF);

   fclose(opened_file);

   counter--;
 
   for(int i = 0; i < counter; i++)
   {
      if(file_input[i] == ' ')
      { 
         file_input[i] = NULL;
         if(file_input[i+1] != EOF)
            commands_ptr[commands_counter++] = &file_input[i+1];
      }
      else if(file_input[i] == '\n')
      {  
          file_input[i] = NULL;
          if(file_input[i+1] != EOF)
          {
             commands_ptr[commands_counter++] = NULL;
             commands_ptr[commands_counter] = &file_input[i+1];
             line_ptr[line_counter++] = &commands_ptr[commands_counter++];
          }
  
      }
   }

   commands_ptr[commands_counter] = NULL;
int wait_arg = 0;
   for(int i = 0; i < line_counter; i++)
   {
     pid_t pid = fork();
     if (pid == 0)
     {
        int wait_time = atoi(line_ptr[i][0]);
        printf("wait time is %d\n", wait_time);
        sleep(wait_time);
	printf("wait time is %d\n", wait_time);
        execvp(line_ptr[i][1], line_ptr[i] + 1); 
     }
   }
}
