#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>
#include <string.h>

struct Edge{
	int mutex1;
	int mutex2;
	int segment1;
	int thread_id;
	int lock_mutex[10];
	int segment2;
	int num_lock;
};
struct Edge *edge_info;
int total_num = 0;

int check_gatelock(struct Edge first_edge, struct Edge second_edge){
	int first_lock_num = first_edge.num_lock;
	int second_lock_num = second_edge.num_lock;

	for(int i = 0; i < first_lock_num; i++){
		for(int j = 0; j < second_lock_num; j++){
			if(first_edge.lock_mutex[i] == second_edge.lock_mutex[j]) return 1;
		}
	}

	return 0;
}

void find_potential_deadlock(struct Edge *edge_info,struct Edge one_edge,int start_index,int num_edge){

	for(int j = start_index; j < num_edge; j++){
		//printf("\n%d\n",j);
		if(one_edge.mutex1 == edge_info[j].mutex2 && one_edge.mutex2 == edge_info[j].mutex1){//사이클 확인
			if(one_edge.segment2 > edge_info[j].segment1){//segment확인
				if(check_gatelock(one_edge,edge_info[j]) == 0){//gatelock확인
					if(one_edge.thread_id != edge_info[j].thread_id){//thread id 일치 확인
					//포텐셜 데드락!
						printf(".trace line : %d\n",j);
						printf("(m11(%d),(s11(%d), t(%d), { r",one_edge.mutex1,one_edge.segment1,one_edge.thread_id);

						for(int k = 0; k < one_edge.num_lock; k++){
							printf(" (%d) ",one_edge.lock_mutex[k]);
						}
						printf("}, s12(%d)),m12(%d)\n",one_edge.segment2,one_edge.mutex2);

						printf("(m21(%d),(s21(%d), t(%d), {",edge_info[j].mutex1,edge_info[j].segment1,edge_info[j].thread_id);
						//printf("\n%d\n",one_edge.num_lock);
						for(int k = 0; k < edge_info[j].num_lock; k++){
							printf(" r(%d) ",edge_info[j].lock_mutex[k]);
						}
						printf("}, s22(%d)),m22(%d)\n",edge_info[j].segment2,edge_info[j].mutex2);

						total_num++;
						printf("\n");
					}
				}
			}
		}
	}
}

int main(){
	char edge[1000][1000];
	int num_edge = 0;

	FILE *fp = fopen("dmonitor.trace", "r");

	while( 0 < fscanf( fp, "%s", edge[num_edge])){
			//printf("%s\n", edge[num_edge]);
			num_edge++;
	}

	edge_info = malloc(sizeof(struct Edge)*num_edge);

	for(int j = 0; j < num_edge; j++){
		//find_edge(edge,edge[j],j,num_edge);
		char *tok;
		int num_loof = 0;
		int lock = 0;

		tok = strtok(edge[j],",");
		while(tok){
			switch(num_loof){
				case 0:
				edge_info[j].mutex1 = atoi(tok);
				break;

				case 1:
				edge_info[j].mutex2 = atoi(tok);
				break;

				case 2:
				edge_info[j].segment1 = atoi(tok);
				break;

				case 3:
				edge_info[j].segment2 = atoi(tok);
				break;

				case 4:
				edge_info[j].thread_id = atoi(tok);
				break;

				default:
				edge_info[j].lock_mutex[lock] = atoi(tok);
				lock++;
			//	printf("%d\n",edge_info[j].lock_mutex[edge_info[j].num_lock]);
				break;
			}
			tok = strtok(NULL,",");
			num_loof++;
		}
		edge_info[j].num_lock = lock;
	}

	// for(int j = 0; j < num_edge; j++){
	// 	printf("(m1(%d),(s1(%d), t(%d), { r",edge_info[j].mutex1,edge_info[j].segment1,edge_info[j].thread_id);
	//
	// 	for(int k = 0; k < edge_info[j].num_lock; k++){
	// 		printf(" (%d) ",edge_info[j].lock_mutex[k]);
	// 	}
	// 	printf("}, s2(%d)),m2(%d)\n",edge_info[j].segment2,edge_info[j].mutex2);
	//
	// }

	for(int j = 0; j < num_edge; j++){
		find_potential_deadlock(edge_info,edge_info[j],j,num_edge);
	}
	printf("total : %d\n",total_num);

	fclose(fp);

	return 0;
}
