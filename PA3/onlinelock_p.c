#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>
#include <signal.h>

typedef struct t_hread *Thread;
Thread pthread_info;

typedef struct t_hread{
  int mutex ;
  pthread_t p_thread;
  int rock_unrock;
  Thread next;
} t_hread;

int thr_num = 0;

int rockcount = 0;
int unrockcount = 0;
int total = 0;

Thread newNode(int mutex, pthread_t pthread_self){
  Thread node = (Thread)malloc(sizeof(t_hread));
  if(node == NULL) {
    printf("error\n");
    return NULL;
  }
  node -> mutex = mutex;
  node -> p_thread = pthread_self;
  node -> next = NULL;
  return node;
}


int pthread_mutex_lock(pthread_mutex_t *mutex){
  int (*real_pthread_mutex_lock)(pthread_mutex_t *mutex);
  char * error ;

  real_pthread_mutex_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
  if ((error = dlerror()) != 0x0)
		exit(1) ;
  int ptr = real_pthread_mutex_lock(mutex);
  rockcount++;
  total++;
  char buf[100];
  char buff[50];

	snprintf(buf, 100, "rock : mutex(%d)  thread(%d) , count : %d, total : %d\n",mutex,pthread_self(),rockcount,total) ;
  fputs(buf, stderr) ;
  int i = thr_num;


  if(thr_num == 0){
    pthread_info = (Thread)malloc(sizeof(t_hread));
    pthread_info -> mutex = (int)mutex;
    pthread_info -> p_thread = pthread_self();
    pthread_info -> next = NULL;
    thr_num++;
  }else{
    Thread x = pthread_info;

    while(x){
      if(x -> mutex != (int)mutex && x -> p_thread != pthread_self()){
        snprintf(buff,50,"deadlock detected!\n");
        fputs(buff, stderr) ;
	exit(0);
        break;
      }else if(x -> mutex != (int)mutex || x -> p_thread != pthread_self()){
        Thread node = newNode((int)mutex,pthread_self());
        node -> next = pthread_info -> next;
        pthread_info -> next = node;
        thr_num++;
        break;
      }
      x = x->next;
    }
  }
//  snprintf(buff,50,"thr_num : %d\n",thr_num);
//  fputs(buff, stderr) ;
	return ptr ;

}

int pthread_mutex_unlock(pthread_mutex_t *mutex){
  int (*real_pthread_mutex_unlock)(pthread_mutex_t *mutex);
	char * error;

  real_pthread_mutex_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
  if ((error = dlerror()) != 0x0)
		exit(1) ;
  int ptr = real_pthread_mutex_unlock(mutex);
unrockcount++;
total--;
  char buf[100];
  char buff[50];
  snprintf(buf, 100, "unrock : mutex(%d)  thread(%d) , count : %d,total : %d\n",mutex,pthread_self(),unrockcount,total) ;
  fputs(buf, stderr) ;


  int linkedList_pointer = 0;
  Thread x = pthread_info;

  while(x){
    if(x -> mutex == (int)mutex && x -> p_thread == pthread_self()){
      Thread y = pthread_info;
      for(int i = linkedList_pointer-1; i > 0; i--){
        y = y->next;
      }
      y->next = x->next;
      x->next = NULL;
      free(x);
      thr_num--;
      break;
    }
    x = x->next;
    linkedList_pointer++;
  }

//  snprintf(buff,50,"thr_num : %d\n",thr_num);
//  fputs(buff, stderr) ;

	return ptr;
}
