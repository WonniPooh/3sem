#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>
#include <dirent.h>
#include <assert.h>

/*
 * Посмотрите мои комментарии. Критически осмыслите их, поправьте, что считаете нужным, т.к. это не ошибки в коде, а
 * скорее замечания по стилю написания кода, которые частично субъективны.
 * И засчитаем.
 */

void find_the_file(const char* name_to_open, const char* name_to_find, const int depth_of_search);

int main(int argc, char* argv[])
{
  /*
   * Магистр Йода? Выглядит старанно, но дело ваше.
   * Лучше бы switch написали.
   */
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

  /*
   * FIXIT:
   * Отдельная константа для 400.
   */
  char path[400] = {};

  while(current_file)
  {  
    stat(current_file -> d_name, &buf);
    /*
     * Что вы подразумевали, написав !!strcmp(...)
     * Почему два символа !?
     * Почему не просто strcmp(current_file -> d_name, ".")?
     */
    if(S_ISDIR(buf.st_mode) && depth_of_search && (!!strcmp(current_file -> d_name, ".") && !!strcmp(current_file -> d_name, "..")))
    {
      strcpy(path, name_to_open);  
      strcat(path, "/");  
      strcat(path, current_file -> d_name);  

      find_the_file(path, name_to_find, depth_of_search - 1); 
    }
    else
    {
      /*
       * Ваша цель писать как можно более просто и понятно. Т.к. работая в команде, напарникам надо будет разбираться в вашем коде.
       * 1) !strcmp(name_to_find, current_file -> d_name)
       * 2) strcmp(name_to_find, current_file -> d_name) == 0
       * Мне кажется, что 2й вариант более понятен, т.к. ф-я strcmp возвращает не булевы значения 0 или 1, а
       * числа < 0, ==0 и >0. Сходу 1й вариант заставляет задумываться.
       */
      if(!strcmp(name_to_find, current_file -> d_name))
      {
        printf("path:: %s\n", name_to_open); 
      }
    }
    current_file = readdir(current_dir); 
  }
  closedir(current_dir); 
}
