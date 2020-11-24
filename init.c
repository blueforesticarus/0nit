#define _GNU_SOURCE

#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <unistd.h>
#include <stdlib.h>
#define MAXLEN 1024

void reap(int signum){
    wait(NULL);
}

int main()
{
    printf(":: 0nit ::\n");
	sigset_t set;
	int status;

    //reaper
    signal(SIGCHLD, reap); 

    setsid();
	setpgid(0, 0); //TODO what does this do?

    char * path = "/run/0.ctrl";
    mkfifo(path, 0666); 
	
    if ( ! fork()){
        char * init = "/bin/init.1";
        execve(init, (char *[]){ init, 0 }, (char *[]){ 0 });
    }

    int fd = open(path, O_RDWR);
    FILE* fp = fdopen(fd, "rw");
    if (fp == NULL) {
        perror("Failed: ");
        return 1;
    }

    char buffer[MAXLEN];
    while(1){
        if (fgets(buffer, MAXLEN - 1, fp)){
            printf("asdf %s", buffer);
            buffer[strcspn(buffer, "\n")] = 0;

            if(! fork()){
                char* b = buffer;
                if (buffer[0] != '!'){
                    unshare(CLONE_NEWNS|CLONE_NEWPID);
                    printf("a %s\n", b);
                }else{
                    b = buffer + 1;
                }

                if(! fork()){
                    mount("none", "/", NULL, MS_REC|MS_PRIVATE, NULL);
                    mount("none", "/proc", NULL, MS_REC|MS_PRIVATE, NULL);
                    mount("proc", "/proc", "proc", MS_NOSUID|MS_NODEV|MS_NOEXEC, NULL);
                    //execve("/bin/dash", (char *[]){"/bin/dash","-c", b, 0 }, (char *[]){ 0 });
                    execve(b, (char *[]){ b, 0 }, (char *[]){ 0 });
                }
                exit(0);//should never be reached
            }
        }
    }
}
