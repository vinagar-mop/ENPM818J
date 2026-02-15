#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#define STORAGE_SIZE sizeof(int)
#define __GNU_SOURCE

enum reading_line 
{
    r = 0,
    q
};

bool continue_program(char * buf);

int count = 0;

int main()
{
    int pipec1c2[2];
    int pid_pipec1c2[2];
    int pid_pipec2c1[2];

    int rlwrite, rlread;

    int n;
    
    int writebuf[2], readbuf[2];
    char buffer[100];
    char bufcont;

    enum reading_line program_run = r;

    pid_t parent_pid = 0, child1_pid = 0, child2_pid = 0;
    
    if (pipe(pipec1c2) == -1)
    {
        perror("pipe c1 -> c2");
    }
    if (pipe(pid_pipec2c1) == -1)
    {
        perror("Adress Pipe c2 -> c1");
    }
    
    if (pipe(pid_pipec1c2) == -1)
    {
        perror("adress pipe c1 -> c2");
    }
    parent_pid = fork();
    if (parent_pid > 0)
    {
        parent_pid = getpid();
        child2_pid = fork();
        if (child2_pid == 0)
        {
            // This is child process for child 2
            int child2_address = getpid();
            while (read(pid_pipec2c1[0], &rlread, sizeof(rlread)) != sizeof(rlread))
            {
                if (write(STDOUT_FILENO, &rlread, sizeof(rlread)) != sizeof(rlread))
                {
                    perror("write");
                    raise(SIGKILL);
                }
            }

            write(pid_pipec1c2[1], &child2_address, sizeof(child2_address)) != (sizeof(child2_address));

            while(continue_program(&bufcont))
            { 
                if (bufcont == 'q') 
                {
                    program_run = q;
                    kill(SIGINT, rlread);
                    raise(SIGKILL);                
                } 
                else
                {
                    program_run = r;
                    kill(SIGCONT, rlread);
                }

                while ((n = getchar()) != '\n' && n != EOF);                
                if (program_run == r)
                {
                    printf("Place the first number and press enter.\n");
                    scanf("%d", &writebuf[0]);
                    while ((n = getchar()) != '\n' && n != EOF);                
                    
                    printf("Place the second number and press enter.\n");
                    scanf("%d", &writebuf[1]);
                    while ((n = getchar()) != '\n' && n != EOF);                

                    if (write(pipec1c2[1], writebuf, sizeof(writebuf)) != (sizeof(writebuf)))
                    {
                        perror("EXITING");
                        program_run = q;
                    }
                    usleep(1000);
                }
            }
            kill(SIGKILL, rlread);
            raise(SIGKILL);
        }

        else if (child2_pid > 0)
        {
            // Hold the parent process till child processes are done.
            wait(NULL);
        }

        else if (child2_pid < 0)
        {
            perror("Parent process was not able to print out.\n");
            exit(0);
        }
    }
    else if (parent_pid == 0)
    {   
        // This is child process for child 1
        rlwrite = getpid();
        write(pid_pipec2c1[1], &rlwrite, sizeof(rlwrite)) != (sizeof(rlwrite));

        int child2_address;
        while (read(pid_pipec1c2[0], &child2_address, sizeof(child2_address)) != sizeof(child2_address))
        {
            if (write(STDOUT_FILENO, &child2_address, sizeof(child2_address)) != sizeof(child2_address))
            {
                perror("write");
                raise(SIGKILL);
            }
        }

        while(1)
        {
            while (read(pipec1c2[0], &readbuf, sizeof(readbuf)) != sizeof(readbuf))
            {

                if (write(STDOUT_FILENO, &readbuf, sizeof(readbuf)) != sizeof(readbuf))
                {
                    perror("write");
                    raise(SIGKILL);
                }
            }
            kill(SIGINT, child2_address);
            printf("The sum is %i + %i = %i \n", readbuf[0], readbuf[1], readbuf[0] + readbuf[1]);
            kill(SIGCONT, child2_address);
        }
    }
    else
    {
        perror("Parent process was not able to print out.\n");
        exit(0);
    }
    return 0;
}

bool continue_program(char *buf)
{
    printf("select r to run the program\n");
    if(scanf("%c", buf) == 1)
    {
        return true;
    }
    return false;
}