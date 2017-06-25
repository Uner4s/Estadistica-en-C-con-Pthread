/******************************************************************************
 * 
 * FILE: MultiplicacionMatricesParalela.c 
 *
 * DESCRIPTION: Este programa multiplica dos matrices cuadradas de forma paralela.  
 *              Se pueden utilizar tantos threads como nucleos tenga el procesador del equipo.
 *                   
 * 
 * AUTHOR: Nicolas Perez Poblete, Ana Alvarez Ojeda
 *
 * LAST REVISED: Santiago de Chile, 20/01/2017
 *
 *****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <time.h>

double a[7000][7000];
double b[7000][7000];
double c[7000][7000];
int numthreads;
int filas;
int columnas;
int matsize;

struct Message {

   int start_i, stop_i;
};

//************************************************************************************************************************************
//*************************************************************************************************************************************
/* retorna "a - b" en segundos */
double timeval_diff(struct timeval *a, struct timeval *b)
{
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
}

//*************************************************************************************************************************************
//*************************************************************************************************************************************

void *multiply_matrix_helper(void *ptr)
{
    float E_cpu;
    clock_t cs, ce;
    long E_wall;
    time_t ts, te;  
    struct Message *data;
    data = (struct Message *) ptr;
    int i, j, k;
    //****************************************************
    //       SE TOMA EL TIEMPO
    ts = time(NULL);
    cs = clock();
    for(i = data->start_i; i < data->stop_i; i++){
        for(j = 0; j < columnas; j++){
            for(k = 0; k < filas; k++)
                c[i][j] += a[i][k]*b[k][j];
        }
    }
    ce = clock();
    te = time(NULL);
    E_wall = (long) (te - ts);
    E_cpu = (float)(ce - cs) / CLOCKS_PER_SEC;     
    printf("Elapsed CPU Time: %f [Secs] Elapsed Wall Time: %ld [Secs]\n",E_cpu/numthreads,E_wall);
	//****************************************************
    pthread_exit(0);
}

void multiply_matrix_with_threading()
{
    
    int i, tope;
    pthread_attr_t attribute;
	pthread_attr_init(&attribute);
    pthread_attr_setdetachstate(&attribute,PTHREAD_CREATE_JOINABLE);
    pthread_t *thread;
    void *exit_status;  
    thread = calloc(numthreads,sizeof(pthread_t));
    struct Message **data;
    columnas = matsize;
    tope = columnas/numthreads;
    data = calloc(numthreads,sizeof(struct Message *));
    
    for (i = 0; i < numthreads; i = i + 1)
        data[i] = calloc(1,sizeof(struct Message));
       

	for(i = 0; i < numthreads; i = i + 1){
		if(i == 0){
			data[i]->start_i=0;
			data[i]->stop_i=tope; 
		} 
		else{
			data[i]->start_i = data[i-1]->start_i + tope;
			if(i == (numthreads - 1))
				data[i]->stop_i = columnas;
			else
				data[i]->stop_i = data[i-1]->stop_i + tope;
		}
		pthread_create(&thread[i], &attribute, multiply_matrix_helper, (void *) data[i]);
		
	}
	pthread_attr_destroy(&attribute); 
    for (i = 0; i < numthreads; i = i + 1)
        pthread_join(thread[i],&exit_status);
    
    free(data);
    free(thread);
}

int main(int argc, char **argv)
{

    if (argc != 3)
      exit(1);
    else{
        matsize = atoi(argv[1]);
        numthreads = atoi(argv[2]);
    }
  	struct timeval t_ini, t_fin;
  	double secs;   
    int i, j;
    
    columnas = matsize;
    filas = matsize;
    //****************************************
    // 	SE LLENAN LAS MATRICES CON DATOS DE PRUEBA
    for(i = 0; i < filas; i++){
        for(j = 0; j < columnas; j++){
            a[i][j] = 1;//rand() / (double) RAND_MAX;
        }
    }
    for(i = 0; i < filas; i++){
        for(j = 0; j < columnas; j++){
            b[i][j] = 1;//rand() / (double) RAND_MAX;
        }
    }
    //****************************************
    gettimeofday(&t_ini, NULL);
    multiply_matrix_with_threading(a, b, c);
    gettimeofday(&t_fin, NULL);
	printf("Multiplicación terminada \n");
  	secs = timeval_diff(&t_fin, &t_ini);
  	printf("Tiempo de Programa: %.6g Segundos\n", secs);    
    return 0;
}
