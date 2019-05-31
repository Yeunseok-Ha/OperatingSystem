#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;

void 
noise()
{
	usleep(rand() % 1000) ;
}

void *
thread(void *arg) 
{
		pthread_mutex_lock(&mutex);	 noise() ;
//		printf("mutex1 by %p\n",&mutex);
		pthread_mutex_lock(&mutex2); noise() ;
//		printf("mutex2 by %p\n",&mutex2);
		pthread_mutex_lock(&mutex3); noise();
		pthread_mutex_unlock(&mutex3); noise();
		pthread_mutex_unlock(&mutex2); noise() ;
		pthread_mutex_unlock(&mutex); noise() ;

		return NULL;
}

void *
thread2(void *arg)
{
		pthread_mutex_lock(&mutex3);	noise();
                pthread_mutex_lock(&mutex);      noise() ;
//              printf("mutex1 by %p\n",&mutex);
                pthread_mutex_lock(&mutex2); noise() ;
//              printf("mutex2 by %p\n",&mutex2);
                pthread_mutex_unlock(&mutex2); noise() ;
                pthread_mutex_unlock(&mutex); noise() ;
		pthread_mutex_unlock(&mutex3); noise();

                return NULL;
}


int 
main(int argc, char *argv[]) 
{
	pthread_t tid[2];
	srand(time(0x0)) ;

	pthread_create(&tid[0], NULL, thread, NULL);
	pthread_create(&tid[1], NULL, thread2, NULL);
		
	pthread_mutex_lock(&mutex2); noise() ; 
//	printf("mutex2 by %p in main\n",&mutex2);
	pthread_mutex_lock(&mutex3);	noise();
	pthread_mutex_lock(&mutex);	noise() ;
//	printf("mutex1 by %p in main\n",&mutex); 
	pthread_mutex_unlock(&mutex); noise() ;
	pthread_mutex_unlock(&mutex3); noise();
	pthread_mutex_unlock(&mutex2); noise() ;

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	return 0;
}

