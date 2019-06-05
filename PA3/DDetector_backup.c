#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>

void init_arr();

struct __mutex_hold{
	pthread_t tid;
	int m_hold;
} mutex_hold[10];

int arr[100][100];
int mid[100];
//int m_hold;
//pthread_t tid[10];
int count = 0;
//int i = 0;
int first_exec = 0;

int pthread_mutex_lock(pthread_mutex_t *mutex){
	int i,j,k;
	int found = 0;
	int m = (int)mutex;
	int (*real_pthread_mutex_lock)(pthread_mutex_t *mutex);
	char * error;
	
	if(first_exec == 0){
                init_arr();
                first_exec = 1;
        }

	real_pthread_mutex_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if ((error = dlerror()) != 0x0)
		exit(1) ;

//	int ptr = real_pthread_mutex_lock(mutex);
	for(k = 0; k<=count; k++)
		if(mutex_hold[k].tid == pthread_self())	
			for(i=0; i<=count; i++){
				if(mid[i]== m){
					found = 1;
					for(j = 0; j <=count; j++){
						if(mid[j] == mutex_hold[k].m_hold){
							arr[j][i] = 1;
							break;
						}
					}
					break;
				}

//		arr[i][i] = 0;
		found = 0;
	}

	if(found ==0){
		arr[count][count] = 0;
		mid[count] = m;
		mutex_hold[count].tid = pthread_self();
		mutex_hold[count].m_hold = m;
		count++;
	}
/*
	for(i = 0; i<100;i++){
		if(mid[i] == m && arr[i][i]==0){
			found = 1;
			arr[i][count] = 1;
			break;	
		}
		else
			found = 0;
	}
	if(found == 0){
		arr[count][count] = 0;
		mid[count] = m;
	}
*/


	char buf[100];
	
	for(i = 0; i< count; i++){
		for(j = 0; j<count; j++)
			printf("%d ",arr[i][j]);
		printf("\n");
	}
	printf("\n");
//	snprintf(buf, 100, "mutex(%d) lock. count = %d\n", m,count);
//	fputs(buf, stderr) ;
//	for(i=0;i<count;i++)
//		printf("%d, ",mid[i]);
//	printf("\n");
//	for(i=0;i<count;i++)
//		printf("%d, ", (int)mutex_hold[i].tid);
//	printf("\n");
	int ptr = real_pthread_mutex_lock(mutex);
	
	return ptr;
//	return real_pthread_mutex_lock ;

}

int pthread_mutex_unlock(pthread_mutex_t *mutex){
	int i, j;
	int found = 0;
        int (*real_pthread_mutex_unlock)(pthread_mutex_t *mutex);
        char * error ;
	int m = (int)mutex;
//	linkedlist *tmp = (linkedlist *)malloc(sizeof(linkedlist));

        real_pthread_mutex_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
        if ((error = dlerror()) != 0x0)
                exit(1) ;
	
	for(i=0;i<count;i++){
		if(mid[i] == m){
			found = 1;
			break;	
		}
	}
	for(j=0; j<count; j++){
		arr[i][j] = -1;
		arr[j][i] = -1;
	}

        int ptr = real_pthread_mutex_unlock(mutex);
        char buf[100];	

	count --;
	snprintf(buf, 100, "mutex(%d)%p unlock. count = %d\n", m, &real_pthread_mutex_unlock, count);
        fputs(buf, stderr) ;
	
        return ptr;
}

void init_arr()
{
	int j = 0;
	int k = 0;
	for(j = 0; j <100; j++)
		for(k = 0; k <100; k++)
			arr[j][k] = -1;
}

void dfs(){

}

int is_cycle(){
	return 0;
}
