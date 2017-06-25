/******************************************************************************
 * 
 * FILE: InversaAll-1Threads.c 
 *
 * DESCRIPTION: 
 *             
 *                   
 * 
 * AUTHOR: Nicolas Perez Poblete
 *
 * LAST REVISED: Santiago de Chile, 20/01/2017
 *
 *****************************************************************************/

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

double matriz_XT_por_X[10000][10000], matriz_Inversa[10000][10000], AUX_temp;
int numthreads;
int matsize; // TAMAÑO DE LA MATRIZ
int AUX_k, AUX_i;
int SEMAFORO = 0, SEMAFORO2 = 0, SEMAFORO3 = 0;   //0 ROJO - 1 VERDE
int fin = 0;

int count=0;
sem_t barrier_sems[max];
sem_t barrier_sems2[max];
sem_t count_sem;


struct Message2 {

   int start_i,k,i, stop_i, thread, NumeroThreads, NumeroThreads2;
   double temp;
};

void *first_function(void *ptr)
{
    struct Message2 *data;
    data = (struct Message2 *) ptr;
    int h, j;
	float E_cpu;
    clock_t cs, ce;
    long E_wall;

    time_t ts, te;  
  	ts = time(NULL);
    cs = clock();	

    
	for( data->k = 0;data->k < matsize;data->k = data->k + 1){
		
		data->temp=matriz_XT_por_X[data->k][data->k];
      	 
	      sem_wait(&count_sem);
	      if (count == numthreads - 1) {
	         count = 0;
	         sem_post(&count_sem);
	         for (j = 0; j < numthreads-1; j++){
	         	sem_post(&barrier_sems[data->k]);
	         }
	            
	      } else {
	         count++;
	         sem_post(&count_sem);
	         sem_wait(&barrier_sems[data->k]);
	      }
	   			
	      //printf("%i  %i\n", data->thread,data->k);	
	    
		for(h = data->start_i; h < data->stop_i; h++){
			matriz_XT_por_X[data->k][h]/=data->temp;							
			matriz_Inversa[data->k][h]/=data->temp;	
    	}			
						
		for(data->i=0; data->i < matsize; data->i = data->i + 1){
			
			data->temp=matriz_XT_por_X[data->i][data->k];
			
		      sem_wait(&count_sem);
		      if (count == numthreads - 1) {
		      	sem_post(&count_sem);
		         count = 0;
	        	 for (j = 0; j < numthreads-1; j++){
	         		sem_post(&barrier_sems2[data->i]);
	         	 }
		      } else {
		         count++;
		         sem_post(&count_sem);
		         sem_wait(&barrier_sems2[data->i]);
		      }
		   		
			for(h = data->start_i; h < data->stop_i; h++){
				if(data->i == data->k)
					break;
				matriz_XT_por_X[data->i][h]-=matriz_XT_por_X[data->k][h]*data->temp;						//R0=R0-R2*A[0][2]
				matriz_Inversa[data->i][h]-=matriz_Inversa[data->k][h]*data->temp;
    		} 
    	//printf("%i  %i\n", data->thread,data->i);	
		}
	}
	
    ce = clock();
    te = time(NULL);
    E_wall = (long) (te - ts);
    E_cpu = (float)(ce - cs) / CLOCKS_PER_SEC;     
    printf("Elapsed CPU Time: %f [Secs] Elapsed Wall Time: %ld [Secs]\n",E_cpu/numthreads,E_wall);	
    
    pthread_exit(0);
}

int main(int argc, char **argv)
{
             
        
	int i,j,h;
    int tope;
 	if (argc != 3)
      exit(1);
	else{
		matsize = atoi(argv[1]);
		numthreads = atoi(argv[2]);
	}
    pthread_attr_t attribute;
	pthread_attr_init(&attribute);
    pthread_attr_setdetachstate(&attribute,PTHREAD_CREATE_JOINABLE);
   for (i = 0; i < matsize; i++){
   		sem_init(&barrier_sems[i], 0, 0);  // Init barrier_sems to 0
   		sem_init(&barrier_sems2[i], 0, 0);  // Init barrier_sems to 0
   }
      
   sem_init(&count_sem, 0, 1);  // Initialize count_sem to 1

    pthread_t *thread;
    void *exit_status;  
    thread = calloc(numthreads,sizeof(pthread_t));
    struct Message2 **data;
    tope = matsize/numthreads;
    data = calloc(numthreads,sizeof(struct Message2 *)); 

    for (i = 0; i < numthreads; i = i + 1)
        data[i] = calloc(1,sizeof(struct Message2));
        
    
    for(i=0;i<matsize;i++)
		for(j=0;j<matsize;j++)
			matriz_XT_por_X[i][j] = ((100 + rand() % (1000 - 1)));	    
    	      
	for(i=0;i<matsize;i++)									//automatically initialize the unit matrix, e.g.
		for(j=0;j<matsize;j++)								//	-       -
			if(i==j)										// | 1  0  0 |
				matriz_Inversa[i][j]=1;						// | 0  1  0 |
			else											// | 0  0  1 |
				matriz_Inversa[i][j]=0;									//  -       -
/*---------------LoGiC starts here------------------*/		//procedure to make the matrix A to unit matrix


	for(h = 0; h < numthreads; h = h + 1){
		if(h == 0){
			data[h]->k=0;
			data[h]->i=0;
			data[h]->start_i=0;
			data[h]->stop_i=tope; 
			data[h]->thread=h; 

		} 
		else{
			data[h]->k=0;
			data[h]->i=0;
			data[h]->start_i = data[h-1]->start_i + tope;
			data[h]->thread=h; 
;
			if(h == (numthreads - 1))
				data[h]->stop_i = matsize;
			else
				data[h]->stop_i = data[h-1]->stop_i + tope;
					
		}
	}
	

	for(h = 0; h < numthreads; h = h + 1)
		pthread_create(&thread[h], &attribute, first_function, (void *) data[h]);

     
    for (i = 0; i < numthreads; i = i + 1)
        pthread_join(thread[i],&exit_status);
	pthread_attr_destroy(&attribute);	

	sem_destroy(&count_sem);
   for (i = 0; i < matsize; i++){
   		sem_destroy(&barrier_sems[i]);
   		sem_destroy(&barrier_sems2[i]);
   }

	return 0;
}












