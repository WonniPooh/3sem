#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>
#include <dirent.h>
#include <assert.h>

void find_the_file(const char* name_to_open, const char* name_to_find, const int depth_of_search);

int main(int argc, char* argv[])
{
  if(3 == argc)
  {
    find_the_file(".", argv[1], atoi(argv[2]));
  }
  else if(4 == argc)
  {
    find_the_file(argv[1], argv[2], atoi(argv[3]));
  }
  else
  {
    printf("Error: wrong number of arguments.\n");
    system("pause");
  }
  return 0;
  
}

void find_the_file(const char* name_to_open, const char* name_to_find, const int depth_of_search)
{
  assert(name_to_open);
  assert(name_to_find);

  DIR* current_dir = opendir(name_to_open);
  struct dirent *current_file = readdir(current_dir);
  struct stat buf = {};

  char path[400] = {};

  while(current_file)
  {  
    stat(current_file -> d_name, &buf);  
    if(S_ISDIR(buf.st_mode) && depth_of_search && (!!strcmp(current_file -> d_name, ".") && !!strcmp(current_file -> d_name, "..")))
    {
      strcpy(path, name_to_open);  
      strcat(path, "/");  
      strcat(path, current_file -> d_name);  

      find_the_file(path, name_to_find, depth_of_search - 1); 
    }
    else
    {
      if(!strcmp(name_to_find, current_file -> d_name))
      {
        printf("path:: %s\n", name_to_open); 
      }
    }
    current_file = readdir(current_dir); 
  }
  closedir(current_dir); 
}
