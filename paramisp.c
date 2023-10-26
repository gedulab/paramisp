/*
 * paramisp
 * A util to demonstrate the parameter misplacement bug by Raymond Zhang
 * The bug originally happened in 32bit application. I ported to 64bit for
 * the GDB Camp in Oct. 2023.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdint.h>

typedef unsigned __int128  ULONGLONG;

typedef struct
{
    char * data;
    ULONGLONG length; 
}SECTION_HEAD;

//int calc_md5(char * data, int nLen, unsigned int md5[4]);

int get_file_id(char * filename, unsigned int * fileid)
{
    SECTION_HEAD section;

    section.data = "testing data-xxxxxxx";
    section.length = strlen(section.data);

    return calc_md5(section.data,  //
        section.length, fileid);
}

static char* exec_name="hdtrap";
 
static void crash_handler2(int sig)
{
    int status=0;
    int pid;
    char * gdb_array[]={"sudo gdb", exec_name, "PID", NULL};
    char pid_str[40];

    sprintf(pid_str, "%d%c", getpid(), '\0');
    gdb_array[2]= pid_str;

    pid= fork();

    if (pid < 0) /* error */
        abort();
    else if (pid) /* parent */
    {
        sleep(60); /* Give GDB time to attach */
        //_exit(1); /* you can skip this line by telling gdb to "return" */
    }
    else /* child */
        execvp("gdb", gdb_array);
}

static void crash_handler(int sig)
{
    int status=0;
    int pid;
    char * gdb_array[]={"gdb", "", NULL};
    char pid_str[40];

    sprintf(pid_str, "--pid=%d%c", getpid(), '\0');
    printf("--pid=%d%c", getpid(), '\0');
    gdb_array[1]= pid_str;

    pid= fork();

    if (pid < 0) /* error */
        abort();
    else if (pid) /* parent */
    {
        while (1)
            sleep(6000); /* Give GDB time to attach */
        //_exit(1); /* you can skip this line by telling gdb to "return" */
    }
    else /* child */
        execvp("gdb", gdb_array);
}

void register_jit_gdb()
{
    signal(SIGQUIT, crash_handler); /* Normally got from Ctrl-\ */
    signal(SIGILL, crash_handler);
    signal(SIGTRAP, crash_handler);
    signal(SIGABRT, crash_handler);
    signal(SIGFPE, crash_handler);
    signal(SIGBUS, crash_handler);
    signal(SIGSEGV, crash_handler); /* This is the most common crash */
    signal(SIGSYS, crash_handler);
}
void help()
{
    printf("Header trap sample by Raymond Zhang\n"
           " [-jit]\n");
} 
int main(int argc, char * argv[])
{
    unsigned int fileid[4];

    printf("running...\n");
    if(argc>1)
    {
        if(strcmp(argv[1], "-jit")==0)
        {
            register_jit_gdb();
            printf("jit debug handler registered\n");
        }
        else if(strstr(argv[1], "-h")!=0)
        {
            help();
            goto TAG_RET;
        }        
    } 
    get_file_id("filea", fileid);

TAG_RET:
    return getchar();
}
