#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>

typedef struct t_hread *Thread;
Thread pthread_info;

typedef struct t_hread{
    int mutex ;
    pthread_t p_thread;
    int is_unlock;
    Thread next;
} t_hread;

int thr_num = 0;
int lockcount = 0;
int unlockcount = 0;
int total = 0;

void alrm_handler(int sig){
	printf("Deadlock Detected!\n");
	exit(0);
}

Thread newNode(int mutex, pthread_t pthread_self){
    Thread node = (Thread)malloc(sizeof(t_hread));
    if(node == NULL) {
        printf("error\n");
        return NULL;
    }
    node -> mutex = mutex;
    node -> p_thread = pthread_self;
    node -> is_unlock = 0;
    node -> next = NULL;
    return node;
}


int pthread_mutex_lock(pthread_mutex_t *mutex){
    int (*real_pthread_mutex_lock)(pthread_mutex_t *mutex);
    char * error ;
    int is_deadlock = 1;
    char buf[100];
    char buff[50];
    struct itimerval t;
    
    real_pthread_mutex_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
    if ((error = dlerror()) != 0x0)
        exit(1) ;
    int ptr = real_pthread_mutex_lock(mutex);
    
    lockcount++;
    total++;
    
//    snprintf(buf, 100, "lock : mutex(%d)  thread(%d) , count : %d, total : %d\n",mutex,pthread_self(),lockcount,total) ;
//    fputs(buf, stderr) ;
    
    if(thr_num == 0){
        pthread_info = (Thread)malloc(sizeof(t_hread));
        pthread_info -> mutex = (int)mutex;
        pthread_info -> p_thread = pthread_self();
        pthread_info -> next = NULL;
        pthread_info -> is_unlock = 0;
        thr_num++;
    }else{
        Thread x = pthread_info;
        
        while(x){
            if(x -> mutex != (int)mutex && x -> p_thread != pthread_self() && x -> is_unlock == 0){
                Thread y = pthread_info;
                
                while(y){
                    if(y -> is_unlock == 1 && y -> mutex == (int)mutex && y -> p_thread != pthread_self()){
                        //       snprintf(buff,50,"--------------------pass----\n");
                        //       fputs(buff, stderr) ;
                        is_deadlock = 0;
                        break;
                    }
                    y = y->next;
                }
                
                if(is_deadlock == 1){
			signal(SIGALRM, alrm_handler);
			t.it_value.tv_sec = 1;
			t.it_value.tv_usec = 0;
			t.it_interval = t.it_value;
			setitimer(ITIMER_REAL, &t, 0x0);
                    //snprintf(buff,50,"------------------deadlock-------------\n");
                    //fputs(buff, stderr) ;
                    break;
                }
            }
            x = x->next;
        }
        Thread node = newNode((int)mutex,pthread_self());
        node -> next = pthread_info -> next;
        pthread_info -> next = node;
        thr_num++;
    }
    
    return ptr ;
    
}

int pthread_mutex_unlock(pthread_mutex_t *mutex){
    int (*real_pthread_mutex_unlock)(pthread_mutex_t *mutex);
    char * error;
    
    real_pthread_mutex_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
    if ((error = dlerror()) != 0x0)
        exit(1) ;
    int ptr = real_pthread_mutex_unlock(mutex);
    
    unlockcount++;
    total--;
    
    char buf[100];
    char buff[50];
//    snprintf(buf, 100, "unlock : mutex(%d)  thread(%d) , unlockcount : %d,total : %d\n",mutex,pthread_self(),unlockcount,total) ;
//    fputs(buf, stderr) ;
    
    Thread x = pthread_info;
    while(x){
        if(x -> mutex == (int)mutex && x -> p_thread == pthread_self()){
            x-> is_unlock = 1;
            break;
        }
        x = x -> next;
    }
    
    return ptr;
}
