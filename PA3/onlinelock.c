#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>
typedef struct __linkedlist{
	struct __mutexinfo *head;
	struct __mutexinfo *cur;
	struct __mutexinfo *tail;
}linkedlist;

typedef struct __mutexinfo{
	int mid ; 
	int cn ;
	struct __mutexinfo *p;
} mutexinfo; 

int count = 0;
linkedlist *l;
int pthread_mutex_lock(pthread_mutex_t *mutex){
//	mutexinfo new;
	int found = 0;
	int m = (int)mutex;
	int (*real_pthread_mutex_lock)(pthread_mutex_t *mutex);
	char * error ;
//	linkedlist *l;
	if(count == 0){
		l = (linkedlist *)malloc(sizeof(linkedlist));
		l->cur = NULL;
		l->head = NULL;
		l->tail = NULL;
	}

	real_pthread_mutex_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if ((error = dlerror()) != 0x0)
		exit(1) ;

	int ptr = real_pthread_mutex_lock(mutex);
	char buf[100];
//	snprintf(buf, 50, "mutex(%d)=%p\n", (int) &mutex, &real_pthread_mutex_lock) ;
	count ++;
	
	l->cur = l->head;
	while(l->cur != NULL){
		if(l->cur->mid == m){
			found = 1;
//			if(l->cur != l->tail)
//				printf("deadlock!\n");
		}
		l->cur = l->cur->p;
	}
	
	if(found == 0){
		mutexinfo *new = (mutexinfo *)malloc(sizeof(mutexinfo));
		new->mid = (int)mutex;
		new->cn = count;
		new->p = NULL;

		if(l->head == NULL && l->tail == NULL){
       			l->head = new;
			l->tail = new;    
		}
    		else {
        		l->tail->p = new;
        		l->tail = new;
    		}	
	}

	l->cur = l->head;
        while(l->cur != NULL){
                printf("%d ->",l->cur->mid);
                l->cur = l->cur->p;
        }
        printf("\n");

	l->cur = l->head;
        while(l->cur != NULL){ 
                if(l->cur->mid == m){
                      if(l->cur != l->tail)
                              printf("deadlock!\n");
                }
                l->cur = l->cur->p;
        }

	snprintf(buf, 100, "mutex(%d)%p lock. count = %d\n", m, &real_pthread_mutex_lock,count);
	fputs(buf, stderr) ;

	return ptr;
//	return real_pthread_mutex_lock ;

}

int pthread_mutex_unlock(pthread_mutex_t *mutex){
	int found = 0;
        int (*real_pthread_mutex_unlock)(pthread_mutex_t *mutex);
        char * error ;
	int m = (int)mutex;
//	linkedlist *tmp = (linkedlist *)malloc(sizeof(linkedlist));

        real_pthread_mutex_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
        if ((error = dlerror()) != 0x0)
                exit(1) ;

        int ptr = real_pthread_mutex_unlock(mutex);
        char buf[100];

//delete last node
//	mutexinfo *tmp = l->head;
//    	while(tmp->p->p != NULL) tmp = tmp->p;
//    	tmp->p = tmp->p->p;
//    	l->tail = tmp;

//delete found node
/*
	tmp->cur = l->head;
	l->cur = l->head;
	while(tmp->cur != NULL){
                if(tmp->cur->mid == m){
//			if(tmp->cur->p == NULL)
//				free(tmp);
//			else{
			while(l->cur !=NULL && l->cur->p != tmp->cur){
				printf("buck\n");
				l->cur = l->cur->p;
			}
			printf("buck\n");
                	l->cur->p = tmp->cur;
	                l->cur->p = tmp->cur->p;
         		free(tmp);
			break;
		
			free(tmp->cur);	
		}
                tmp->cur = tmp->cur->p;	
	}
*/
	l->cur = l->head;
	while(l->cur != NULL){
		printf("%d ->",l->cur->mid);
		l->cur = l->cur->p;
	}
	printf("\n");
		

	count --;
	snprintf(buf, 100, "mutex(%d)%p unlock. count = %d\n", m, &real_pthread_mutex_unlock, count);
        fputs(buf, stderr) ;
	
        return ptr;
}

