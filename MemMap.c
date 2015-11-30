#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

const int ASCII_SHIFT = 1;

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("Check amount of args!\n");
		exit(-1);
	}
	
	if(ASCII_SHIFT < 0)
	{
		printf("Check ASCII SHIFT ranges!\n");
		exit(-1);
	}
		
    int fd = 0;
    struct stat file_info;
    char* map_mem_ptr = NULL;
    
    if ((fd = open(argv[1], O_RDWR, 0666)) == -1)
    {
        printf("Can't open the file\n");
        printf("%d\n", errno);
        exit(-1);
    }
    
    stat(argv[1], &file_info);
    size_t size = file_info.st_size;
        
    if ((map_mem_ptr = (char*)mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        printf("Mapping failed\n");
        printf("%d\n", errno);
		exit(-1);
    }
    
    close(fd);
    
    for (int i = 0; i < size; i++) 
    {
		map_mem_ptr[i] = (map_mem_ptr[i] + ASCII_SHIFT) % 127;
	}  
    
    if (munmap((void *)map_mem_ptr, size) == -1)
    {
        printf("Can not unmup!\n");
        printf("%d\n", errno);
		exit(-1);
    }
    return 0;
}

/*
    Файл 2,85 Mb, результаты:
    real	0m0.018s
    user	0m0.015s
    sys		0m0.003s
    
    При read() и write() результаты:
    real	0m4.132s
    user	0m0.145s
    sys		03m.987s
*/
