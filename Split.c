#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELIMITERS_NUM 100
#define STR_LENGTH 500
#define TOKENS_NUM 500

void Split(char* string, char* delimiters, char*** token_ptr, int* tokensCount)
{
    char** token = *token_ptr;
    int counter = *tokensCount;
    token[counter] = strtok(string, delimiters);
    counter++;
    
    do
    {
        token[counter] = strtok(NULL, delimiters);
    }
    while(token[counter++]);
    
    *tokensCount = counter;
}

int main()
{
    char* my_string = (char*)calloc(STR_LENGTH, sizeof(char));
            
    char* delimiters = (char*)calloc(DELIMITERS_NUM, sizeof(char));
    
    char** tokens = (char**)calloc(TOKENS_NUM, sizeof(char*));
    
    int tokensCount = 0;
    
    printf("Enter a string to divide\n");
    gets(my_string);
    printf("%s\n",my_string);
    
    printf("Enter delimiters without spaces\n");
    gets(delimiters);
    printf("%s\n",delimiters);
        
    Split(my_string, delimiters, &tokens, &tokensCount);
    
    int counter = 0;
    
    while(tokens[counter])
    {
        printf("%s\n",tokens[counter++]);
    }
}
