#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

double matriz_XT_por_X[10000][10000], matriz_Inversa[10000][10000], AUX_temp;
int semaforo[128];
int semaforo2[128];
int matsize; // TAMAÑO DE LA MATRIZ
int AUX_k, AUX_i;
int SEMAFORO = 0, SEMAFORO2 = 0, SEMAFORO3 = 0, play = 0;   //0 ROJO - 1 VERDE
int fin = 0;
int numthreads;

double timeval_diff(struct timeval *a, struct timeval *b)
{
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
}

struct Message2 {

   int start_i, stop_i, thread, sem1, sem2;
};

void *first_function(void *ptr)
{
    struct Message2 *data;
    data = (struct Message2 *) ptr;
    int h, j;
	data->sem1=0;
	data->sem2=0;
	semaforo[data->thread]=0;
	semaforo2[data->thread]=0;

	float E_cpu;
    clock_t cs, ce;
    long E_wall;
    time_t ts, te;
  	ts = time(NULL);
    cs = clock();
/*
 	struct timeval t_ini, t_fin;
  	double secs;
  	gettimeofday(&t_ini, NULL);
  	*/
	while( AUX_k < matsize){
		data->sem1 = data->sem1 + 1;

		while( data->sem1 != SEMAFORO);

		for(h = data->start_i; h < data->stop_i; h++){
			matriz_XT_por_X[AUX_k][h]/=AUX_temp;									//it performs the following row operations to make A to unit matrix
			matriz_Inversa[AUX_k][h]/=AUX_temp;
    	}
		semaforo2[data->thread]++;


		while( AUX_i < matsize ){

			data->sem2 = data->sem2 + 1;

			while( data->sem2 != SEMAFORO3);

			for(h = data->start_i; h < data->stop_i; h++){
				if(AUX_i == AUX_k)
					break;
				matriz_XT_por_X[AUX_i][h]-=matriz_XT_por_X[AUX_k][h]*AUX_temp;
				matriz_Inversa[AUX_i][h]-=matriz_Inversa[AUX_k][h]*AUX_temp;
    		}
    		semaforo[data->thread]++;

    		if(AUX_i==(matsize-1))
    			break;


		}
		if(AUX_k==(matsize-1))
			break;
	}
	/*
  	gettimeofday(&t_fin, NULL);

  	secs = timeval_diff(&t_fin, &t_ini);
  	printf("%.3g seconds\n", secs );
  	*/

    ce = clock();
    te = time(NULL);
    E_wall = (long) (te - ts);
    E_cpu = (float)(ce - cs) / CLOCKS_PER_SEC;
    printf("Elapsed CPU Time: %f [Secs] Elapsed Wall Time: %ld [Secs]\n",E_cpu/(numthreads+1),E_wall);

    pthread_exit(0);
}

int main(int argc, char **argv)
{

	int i,j,k,h, play=0;
	
 	if (argc != 3)
      exit(1);
	else{
		matsize = atoi(argv[1]);
		numthreads = atoi(argv[2]);
	}
	
    int tope;
    pthread_attr_t attribute;
	pthread_attr_init(&attribute);
    pthread_attr_setdetachstate(&attribute,PTHREAD_CREATE_JOINABLE);
    pthread_t *thread;
    void *exit_status;
    thread = calloc(numthreads,sizeof(pthread_t));
    struct Message2 **data;
    tope = matsize/numthreads;
    data = calloc(numthreads,sizeof(struct Message2 *));
   	SEMAFORO = 0;
	SEMAFORO2 = 0;
	SEMAFORO3 = 0;
	int cont = 0;
	int cont2 = 0;


    for (i = 0; i < numthreads; i = i + 1)
        data[i] = calloc(1,sizeof(struct Message2));

    for(i=0;i<matsize;i++)
		for(j=0;j<matsize;j++)
			matriz_XT_por_X[i][j] = ((100 + rand() % (100 - 1)));

	for(i=0;i<matsize;i++)
		for(j=0;j<matsize;j++)
			if(i==j)
				matriz_Inversa[i][j]=1;
			else
				matriz_Inversa[i][j]=0;


//***************************************************************************************
			//ACA SE CREAN LOS DATOS DE LAS ESTRUCTURAS PARA LA FUNCION PARALELA
	for(h = 0; h < numthreads; h = h + 1){
		if(h == 0){
			data[h]->start_i=0;
			data[h]->stop_i=tope;
			data[h]->thread=h;
		}
		else{
			data[h]->start_i = data[h-1]->start_i + tope;
			data[h]->thread=h;
			if(h == (numthreads - 1))
				data[h]->stop_i = matsize;
			else
					data[h]->stop_i = data[h-1]->stop_i + tope;

		}
		pthread_create(&thread[h], &attribute, first_function, (void *) data[h]);
	}
//***************************************************************************************


//***************************************************************************************
//***************************************************************************************
	//ACA COMIENZA LA INVERSION


	float E_cpu;
    clock_t cs, ce;
    long E_wall;
    time_t ts, te;
  	ts = time(NULL);
    cs = clock();

	for(k=0;k<matsize;k++)
	{

		AUX_temp=matriz_XT_por_X[k][k];
		AUX_k=k;

		SEMAFORO = SEMAFORO + 1;
		play = 0;


		while(play < numthreads){
			play = 0;
			for(h = 0; h < numthreads; h = h + 1){
				if(semaforo2[h]!=cont2){
					play = play + 1;
				}
			}
			if(play == numthreads)
				break;
		}
		cont2++;
		for(i=0;i<matsize;i++){

			AUX_temp=matriz_XT_por_X[i][k];
			AUX_i=i;
			SEMAFORO3 = SEMAFORO3 + 1;
			play = 0;

			while(play < numthreads){
				play = 0;
				for(h = 0; h < numthreads; h = h + 1){
					if(semaforo[h]!=cont){
						play = play + 1;
					}
				}
				if(play == numthreads)
					break;
			}
			cont++;
		}
	}
    //***************************************************************************************
	//***************************************************************************************
    ce = clock();
    te = time(NULL);
    E_wall = (long) (te - ts);
    E_cpu = (float)(ce - cs) / CLOCKS_PER_SEC;
    printf("Elapsed CPU Time: %f [Secs] Elapsed Wall Time: %ld [Secs]\n",E_cpu/(numthreads+1),E_wall);
	pthread_attr_destroy(&attribute);
    for (i = 0; i < numthreads; i = i + 1)
        pthread_join(thread[i],&exit_status);

	//ESTO MUESTRA LA MATRIZ INVERSA
    /*
	printf(" \n");
	for(i=0;i<matsize;i++)
	{
		for(j=0;j<matsize;j++)
			printf("%lf	",I[i][j]);
		printf("\n ");
	}
	*/
	return 0;
}
