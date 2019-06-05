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
int count = 0;
int first_exec = 0;

void get_line(){

}

int pthread_mutex_lock(pthread_mutex_t *mutex){
	int i,j,k;
	int from, to;
	int state = 0;
	int m = (int)mutex;
	int (*real_pthread_mutex_lock)(pthread_mutex_t *mutex);
	char * error;
	
	if(first_exec == 0){
                init_arr();
                first_exec = 1;
		arr[count][count] = 0;
                mutex_hold[count].tid = pthread_self();
                mutex_hold[count].m_hold = m;
		mid[count] = m;
                count++;
        }

	real_pthread_mutex_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if ((error = dlerror()) != 0x0)
		exit(1) ;

	if(first_exec != 0){
		printf("tid :%p, mid:%p\n",pthread_self(), m);
		for(i = 0; i < count; i++){
			if(mutex_hold[i].tid == pthread_self()){
				state = 1;
				break;
			}
		}
		if(state != 1){
			mutex_hold[i].tid = pthread_self();
		}else if(state == 1){ 
			for(i = 0; i < count; i++){
		    		for(j = 0; j < count; j++){
					if(mutex_hold[j].tid == pthread_self() && mutex_hold[i].m_hold == m){
						state = 1;
						break;
//			}else if(mutex_hold[i].tid != pthread_self() && mutex_hold[i].m_hold == m){
//				state = 2;
				//break;
					}else if(mutex_hold[j].tid == pthread_self() && mutex_hold[i].m_hold != m){
						arr[j][i] = 1;
						state = 3;
						break;
					}else {
						state = 4;
					}
  		  		}
			}
		}
		
		if(state == 4){
			arr[i][i] = 0;
                        mid[i] = m;
                        mutex_hold[i].m_hold = m;
                        count++;
		}
	}
/*
		if(state == 1){
		//	printf("Deadlock!\n");
		}else if(state == 2){
			
		}else if(state ==3){
			for(k = 0; k < count; k++){
				if(mid[k] == m){
					arr[i][k] = 1;
					
				}
			}
		}else{
			arr[i][i] = 0;
			mid[i] = m;
			mutex_hold[i].m_hold = m;
			count++;
		}
	}
*/
	char buf[100];
	
	for(i = 0; i< count; i++){
		for(j = 0; j<count; j++)
			printf("%d ",arr[i][j]);
		printf("\n");
	}
	printf("\n");
	printf("get line : %p\n", get_line);
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
//	snprintf(buf, 100, "mutex(%d)%p unlock. count = %d\n", m, &real_pthread_mutex_unlock, count);
//        fputs(buf, stderr) ;
	
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
