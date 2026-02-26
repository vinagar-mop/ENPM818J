#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LENGHT_OF_BUFFER 40

int buffer = 0;

pthread_mutex_t mutex_lock;

sem_t sem_sig;

// void *adding_data(void *i)
// {
//     pthread_mutex_lock(&locker);
//     ++*(int*)i; 
//     printf("I=%i\n", *(int*) i);
//     buffer = *(int*)i;
//     printf("buffer: %d\n", buffer);
//     pthread_mutex_unlock(&locker);
//     return 0;
// }


void *prints_when_released(void *i)
{
    // Prints key released but waits when it is pressed
    FILE *file_read;
    char *buffer = NULL;
    size_t bufsize = LENGHT_OF_BUFFER;
    char *sub_str = "release";
    const char delim[] = " ";
    char *token;

    pthread_mutex_lock(&mutex_lock);
    buffer = (char*) malloc(LENGHT_OF_BUFFER * sizeof(char));
    file_read = fopen("key_pressed.txt", "r");

    if (buffer == NULL)
    {
        exit(1);
    }

    while(fgets(buffer, bufsize, file_read) != NULL)
    {
        if(strstr(buffer, sub_str) != NULL)
        {
            printf("%s",buffer);
        }
    }
    fclose(file_read);
    pthread_mutex_unlock(&mutex_lock);

}

void *prints_when_pressed(void *i)
{
    // Print when the key is pressed but wait when it is releases
    FILE *file_read;
    char *buffer = NULL;
    size_t bufsize = LENGHT_OF_BUFFER;
    char *sub_str = "press";
    const char delim[] = " ";
    char *token;

    pthread_mutex_lock(&mutex_lock);
    buffer = (char*) malloc(LENGHT_OF_BUFFER * sizeof(char));
    file_read = fopen("key_pressed.txt", "r");

    if (buffer == NULL)
    {
        exit(1);
    }

    while(fgets(buffer, bufsize, file_read) != NULL)
    {
        if(strstr(buffer, sub_str) != NULL)
        {
            printf("%s",buffer);
        }
    }
    fclose(file_read);
    pthread_mutex_unlock(&mutex_lock);
}


int main() 
{
    int counter = 0;
    pthread_t thread1, thread2;

    // tell the user to input system
    printf("Start Typing the input\n");

    // Testing key presses
    if (system("sudo showkey > key_pressed.txt ") == 0)
    {
        printf("command for keypress failed");
    }
    // pthread_mutex_init(&locker, NULL);
    // sem_init(&sem_sig, 0, 1);
    pthread_mutex_init(&mutex_lock, NULL);

    pthread_create(&thread1, NULL, prints_when_released, (void*)&counter);
    pthread_create(&thread2, NULL, prints_when_pressed, (void*)&counter);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // sem_destroy(&sem_sig);
    pthread_mutex_destroy(&mutex_lock);
    //pthread_mutex_destroy(&locker);

    return 0;
}
