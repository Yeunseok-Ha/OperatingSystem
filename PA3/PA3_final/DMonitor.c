#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>

struct Thread{
  pthread_t thread_id;
	int segment;
	int lock[10];
	int lock_count;
};
struct Thread thread_info[10] ;

int thread_index = 0 ;
int count = 0 ;
int lockcount = 0 ;
FILE *fp;

int file_open = 0;

int pre_mutex;
int pre_segment;
pthread_t pre_thread;
//int pre_address;

int pthread_mutex_lock(pthread_mutex_t *mutex){
  int (*real_pthread_mutex_lock)(pthread_mutex_t *mutex);
  char * error ;
  char buf[100];
  char buff[500];
	int already = 0;

  real_pthread_mutex_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");

  if ((error = dlerror()) != 0x0)
		exit(1) ;
  int ptr = real_pthread_mutex_lock(mutex);

	//snprintf(buf, 100, "lock : mutex(%d)  thread(%d) , lockcount : %d\n",mutex,pthread_self(),lockcount) ;
  //fputs(buf, stderr) ;

  //---------------------------

	if(file_open == 0){
		fp = fopen("dmonitor.trace", "a");
		count++;
	}

	int length = 0;
	int j = 0;

  for(j = thread_index-1; j > -1; j--){
		if(thread_info[j].thread_id == pthread_self()) {
		already = 1;
		break;
		}
  }

	if(already == 0){
		thread_info[thread_index].thread_id = pthread_self();
		thread_info[thread_index].lock_count = 0;
		j = thread_index;
		thread_index++;
		
	}
	 
   if(lockcount == 0 || (lockcount != 0 && pre_mutex == (int)mutex)){
    pre_mutex = (int)mutex;
    pre_segment = thread_info[j].segment ;
		pre_thread = pthread_self() ;
		lockcount++;
   }else{//PRINT
 		usleep(100);
  	snprintf(buff, 500, "%d,%d,%d,%d,%d",pre_mutex,(int)mutex,pre_segment,thread_info[j].segment,pthread_self());
		fputs(buff, fp) ;
 		  for(int i = 0; i < thread_info[j].lock_count; i++){
			snprintf(buff,500,",%d", thread_info[j].lock[i]);
			fputs(buff, fp) ;
		}
		snprintf(buff,500,"\n");
  	fputs(buff, fp) ;
		pre_mutex = (int)mutex;
		pre_segment = thread_info[j].segment ;
		pre_thread = pthread_self() ;
   }

	 thread_info[j].lock[thread_info[j].lock_count] = (int)mutex;
	 thread_info[j].lock_count++;

	return ptr;
}


int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg){
  char buff[50];
  int (*real_pthread_create)(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
  char * error;

  real_pthread_create = dlsym(RTLD_NEXT, "pthread_create");
  if ((error = dlerror()) != 0x0)
		exit(1) ;
  int ptr = real_pthread_create(thread,attr,start_routine,arg);

	thread_info[count].segment = (count+1)*2;
	snprintf(buff, 50, "segment(%d)\n",thread_info[count].segment) ;
  fputs(buff, stderr) ;

	count++;
  return ptr;
}


int pthread_mutex_unlock(pthread_mutex_t *mutex){
  int (*real_pthread_mutex_unlock)(pthread_mutex_t *mutex);
	char * error;
  char buf[100];
  char buff[50];

  real_pthread_mutex_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
  if ((error = dlerror()) != 0x0)
		exit(1) ;
  int ptr = real_pthread_mutex_unlock(mutex);

  //snprintf(buf, 100, "unlock : mutex(%d)  thread(%d)\n",mutex,pthread_self()) ;
  //fputs(buf, stderr) ;

	 for(int j = thread_index-1; j > -1; j--){
		if(thread_info[j].thread_id == pthread_self()) {
		thread_info[j].lock[thread_info[j].lock_count] = 0;
		thread_info[j].lock_count--;
		break;
		}
  }

	return ptr;
}
