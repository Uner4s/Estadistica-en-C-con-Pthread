#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define size 50000
#define size2 25000
#define max 5000


double matriz_X[size][500];
double matriz_X_T[500][size];
double matriz_XT_por_X[500][500];
double matriz_XT_por_Y[500][1];
double matriz_Y[size][1];
double matriz_Inversa[500][500];
double matriz_B[500][500];
double matriz_Y_circunfleja[size][1];
double matriz_Aux[size][500];
double AUX_temp;
int SEMAFORO = 0, SEMAFORO2 = 0, SEMAFORO3 = 0;
int AUX_k, AUX_i, matsize;
int semaforo[8];
int semaforo2[8]; 
int semaforo3[8]; 
int semaforo4[8]; 
int filas = 47000;
int columnas = 50;
int numthreads;


int count=0;
sem_t barrier_sems[max];
sem_t barrier_sems2[max];
sem_t count_sem;

struct Message {

   int start_i, stop_i;
};

struct Message2 {

   int start_i,k,i, stop_i, thread, NumeroThreads, NumeroThreads2;
   double temp;
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

void Generar_MatrizX_Y(double matriz_X[size][500], double matriz_Y[size][1], int filas, int columnas){
    FILE *Archivo;
// GENERAR LA MATRIZ X CON LOS 1 Y LA MATRIZ Y
    Archivo = fopen("prueba.txt","r");
    int i,j;
    if(Archivo==NULL)
        printf("error");
    for(i=0;i<filas;i++){
		fscanf(Archivo, "%lf ", &matriz_Y[i][0]);
		//printf("%.2f \n", matriz_Y[i][0]);
        for(j=0;j<columnas;j++){
			if(j == 0){
				matriz_X[i][j] = 1;
				//printf("%.2f ", matriz_X[i][j]);
			}
			else{
				fscanf(Archivo, "%lf ", &matriz_X[i][j]); //se guarda en un array
				//printf("%.2f  ", matriz_X[i][j]);  //  y se imprime a la vez (aprovechamos por que el bucle es el mismo)
			}
        }
        //printf("\n");      //cada vez que se termina una fila hay que pasar a la siguiente linea
    }
    fclose(Archivo);

}

//*************************************************************************************************************************************
//*************************************************************************************************************************************

void Generar_MatrizX_Traspuesta(double matriz_X[size][500], double matriz_X_T[500][size], int filas, int columnas){

	int i,j;
    for(i=0;i<columnas;i++){
		for(j=0;j<filas;j++){
			matriz_X_T[i][j] = matriz_X[j][i];
		}
    }

    //for(i=0;i<columnas;i++){
    //for(j=0;j<filas;j++){
	//	printf("%.2f ", matriz_X_T[i][j]);
        //}
        //printf("\n");      //cada vez que se termina una fila hay que pasar a la siguiente linea
    //}
}

//*************************************************************************************************************************************
//*************************************************************************************************************************************

void *multiply_matrix_helper(void *ptr)
{
	struct timeval t1, t2;
    double elapsed_time;
    struct Message *data;
    data = (struct Message *) ptr;
    int i, j, k;
    double aux=0;
    gettimeofday(&t1, NULL);
    for(i = data->start_i; i < data->stop_i; i++){
        for(j = 0; j < columnas; j++){
            for(k = 0; k < filas; k++){
            	aux = aux + (matriz_X_T[i][k]*matriz_X[k][j]);
			}
            matriz_XT_por_X[i][j] = aux;
            aux=0;
        }
    }
    gettimeofday(&t2, NULL);
    elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    //printf("Time taken with threading: %f ms\n", elapsed_time/1000); 
    pthread_exit(0);
}


void multiply_matrix_with_threading()
{
    int i, tope;
    pthread_attr_t attribute;
	pthread_attr_init(&attribute);
    pthread_attr_setdetachstate(&attribute,PTHREAD_CREATE_JOINABLE);
	numthreads = 4;
    pthread_t *thread;
    void *exit_status;  
    thread = calloc(numthreads,sizeof(pthread_t));
    struct Message **data;
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
    
}



void *multiply_matrix_helper_two(void *ptr)
{
	struct timeval t1, t2;
    double elapsed_time;
    struct Message *data;
    data = (struct Message *) ptr;
    int i, j, k;
    double aux=0;
    gettimeofday(&t1, NULL);
    for(k = data->start_i; k < data->stop_i; k++){
        for(i = 0; i < filas; i++){
            for(j = 0; j < columnas; j++){
            	aux = aux + (matriz_X[i][j]*matriz_Inversa[j][k]);
			}
            matriz_Aux[i][k] = aux;
            aux = 0;
        }
    }
    gettimeofday(&t2, NULL);
    elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    //printf("Time taken with threading: %f ms\n", elapsed_time/1000); 
    pthread_exit(0);
}


void multiply_matrix_with_threading_two()
{
    int i, tope;
    pthread_attr_t attribute;
	pthread_attr_init(&attribute);
    pthread_attr_setdetachstate(&attribute,PTHREAD_CREATE_JOINABLE);
	numthreads = 4;
    pthread_t *thread;
    void *exit_status;  
    thread = calloc(numthreads,sizeof(pthread_t));
    struct Message **data;
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
		pthread_create(&thread[i], &attribute, multiply_matrix_helper_two, (void *) data[i]);
		
	}
	pthread_attr_destroy(&attribute); 
    for (i = 0; i < numthreads; i = i + 1)
        pthread_join(thread[i],&exit_status);
    
}



//*************************************************************************************************************************************
//*************************************************************************************************************************************

void Multiplicar_MatrizXT_por_MatrizY(double matriz_X_T[500][size], double matriz_Y[size][1], double matriz_XT_por_Y[500][1],int filas,int columnas){

  	int i, j;
  	double aux = 0;
    for(i=0;i<filas;i++){
        for(j=0;j<columnas;j++){
			aux = aux + (matriz_X_T[i][j]*matriz_Y[j][0]);
		}
		matriz_XT_por_Y[i][0] = aux;
		//printf("%.2f \n", matriz_XT_por_Y[i][0]);
		aux = 0;
    }
}

//*************************************************************************************************************************************
//*************************************************************************************************************************************

void Traspuesta_Adjunta(double MatrizAdjT[500][500], double matriz_XT_por_X[500][500]){

	int i,j;
    for(i=0;i<3;i++){
        for(j=0;j<3;j++){
			MatrizAdjT[j][i] = matriz_XT_por_X[i][j];
		}
	}
}

//*************************************************************************************************************************************
//*************************************************************************************************************************************


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
	   			
	      printf("%i  %i\n", data->thread,data->k);	
	    
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
    printf("Elapsed CPU Time: %f [Secs] Elapsed Wall Time: %ld [Secs]\n",E_cpu,E_wall);	
    
    pthread_exit(0);
}


//*************************************************************************************************************************************
//*************************************************************************************************************************************

void Determinar_Betas(double matriz_B[500][500], double matriz_Inversa[500][500], double matriz_XT_por_Y[500][1], int filas, int columnas){

  	int i, j;
  	double aux = 0;
    for(i=0;i<filas;i++){
        for(j=0;j<filas;j++){
			aux = aux + (matriz_Inversa[i][j]*matriz_XT_por_Y[j][0]);
		}
			matriz_B[i][0] = aux;
			aux = 0;
    }
    //for(i=0;i<filas;i++){
		//for(j=0;j<1;j++){
			//printf("%.7f  ", matriz_B[i][j]);
		//}
        //printf("\n");      //cada vez que se termina una fila hay que pasar a la siguiente linea
    //}
}

//*************************************************************************************************************************************
//*************************************************************************************************************************************

void Tabla_Anova(double matriz_Y_circunfleja[size][1], double matriz_Y[size][1], double matriz_Inversa[500][500], double matriz_X[size][500], double matriz_XT_por_Y[500][1], double matriz_Aux[size][500], int filas, int columnas){

	//struct timeval t_ini, t_fin;
  	//double secs;
  	int i, j;
  	double aux = 0;
  	double SCM=0;
  	double SCT=0;
  	double SCR=0;
  	int GL_Modelo = columnas - 1;
  	int GL_Residuos = filas - GL_Modelo - 1;
  	int GL_Totales = filas - 1;
  	double Media_Modelo;
  	double Media_Residuos;
  	double Coef_F;
  	/*
  	for(k=0;k<columnas;k++){
		for(i=0;i<filas;i++){
        	for(j=0;j<columnas;j++){
				aux = aux + (matriz_X[i][j]*matriz_Inversa[j][k]);
				//printf("%.2f  %.2f \n", matriz_X_T[i][j], matriz_Y[j][0] );
			}
			//printf("\n");
			matriz_Aux[i][k] = aux;
			//printf("%f \n", matriz_Aux[i][k]);
			aux = 0;
		}
    }
    aux = 0;
    */
    
    multiply_matrix_with_threading_two();
    //gettimeofday(&t_ini, NULL);
    for(i=0;i<filas;i++){
        for(j=0;j<columnas;j++){
			aux = aux + (matriz_Aux[i][j]*matriz_XT_por_Y[j][0]);
			//printf("%.2f  %.2f \n", matriz_X_T[i][j], matriz_Y[j][0] );
		}
		//printf("\n");
		matriz_Y_circunfleja[i][0] = aux;
		//printf("%i  %f  \n",i, matriz_Y_circunfleja[i][0]);
		aux = 0;
    }

    double y_promedio = 0;
    for(i=0;i<filas;i++){
			y_promedio = y_promedio + matriz_Y[i][0];
	}

    y_promedio = y_promedio/filas;
	for(i=0;i<filas;i++){
		SCM = SCM + ((matriz_Y_circunfleja[i][0] - y_promedio)*(matriz_Y_circunfleja[i][0] - y_promedio));
	}

	//printf("SCM = %.6f \n", SCM );

	for(i=0;i<filas;i++){
		SCR = SCR + ((matriz_Y[i][0] - matriz_Y_circunfleja[i][0])*(matriz_Y[i][0] - matriz_Y_circunfleja[i][0]));
	}
	
	SCT = SCR + SCM;
	//gettimeofday(&t_fin, NULL);
	//printf("SCR = %.6f \n", SCR );
	Media_Modelo = SCM/GL_Modelo;
	//printf("%.6f \n", Media_Modelo );
	Media_Residuos = SCR/GL_Residuos;
	//printf("%.6f \n", Media_Residuos );
	Coef_F = Media_Modelo/Media_Residuos;
	//printf("%.6f \n", Coef_F );
	//secs = timeval_diff(&t_fin, &t_ini);
	//printf("%.3g Segundos\n", secs);

	printf("Fuente de Variacion           SC          G.L.         Media           Cociente F\n");
	printf("     Modelo              %.4lf       %.i         %.4lf        %.4lf \n",SCM, GL_Modelo, Media_Modelo, Coef_F);
	printf(" Residuos/Errores        %.4lf       %.i         %.4lf\n", SCR, GL_Residuos, Media_Residuos);
    printf("      Total              %.4lf       %.i\n", SCT, GL_Totales);
}






//*************************************************************************************************************************************
//*************************************************************************************************************************************
int main()
{
	struct timeval t_ini, t_fin;
  	double secs;

	gettimeofday(&t_ini, NULL);
	Generar_MatrizX_Y(matriz_X, matriz_Y, filas, columnas);
	//printf("Fuente de Variacion 1 \n");
	Generar_MatrizX_Traspuesta(matriz_X, matriz_X_T, filas, columnas);
	//printf("Fuente de Variacion 2 \n");
	//Multiplicar_MatrizXT_por_MatrizX(matriz_X, matriz_X_T, matriz_XT_por_X, columnas, filas);
	multiply_matrix_with_threading();
	//printf("Fuente de Variacion 3 \n");
	Multiplicar_MatrizXT_por_MatrizY(matriz_X_T, matriz_Y, matriz_XT_por_Y, columnas, filas);
	//printf("Fuente de Variacion 4 \n");
	//Determinar_Inversa(matriz_XT_por_X, matriz_Inversa, columnas, columnas);

	//Determinar_Inversa();




	int i,j,h;
    int tope;
    pthread_attr_t attribute;
	pthread_attr_init(&attribute);
    pthread_attr_setdetachstate(&attribute,PTHREAD_CREATE_JOINABLE);
	numthreads = 3; // NUMERO DE THREADS
    pthread_t *thread;
    matsize = columnas;
    void *exit_status;  
    thread = calloc(numthreads,sizeof(pthread_t));
    struct Message2 **data;
    tope = matsize/numthreads;
    data = calloc(numthreads,sizeof(struct Message2 *)); 

    for (i = 0; i < numthreads; i = i + 1)
        data[i] = calloc(1,sizeof(struct Message2));
        	    
        
    /*    
    matriz_XT_por_X[0][0]=1;
	matriz_XT_por_X[0][1]=0;    
	matriz_XT_por_X[0][2]=0;    
	matriz_XT_por_X[1][0]=0;    
	matriz_XT_por_X[1][1]=1;    
	matriz_XT_por_X[1][2]=0;    
	matriz_XT_por_X[2][0]=0;    
	matriz_XT_por_X[2][1]=0;    
	matriz_XT_por_X[2][2]=1;        
    */    
    
	for(i=0;i<matsize;i++)									//automatically initialize the unit matrix, e.g.
		for(j=0;j<matsize;j++)								//	-       -
			if(i==j)										// | 1  0  0 |
				matriz_Inversa[i][j]=1;						// | 0  1  0 |
			else											// | 0  0  1 |
				matriz_Inversa[i][j]=0;									//  -       -
/*---------------LoGiC starts here------------------*/		//procedure to make the matrix A to unit matrix

   for (i = 0; i < matsize; i++){
   		sem_init(&barrier_sems[i], 0, 0);  // Init barrier_sems to 0
   		sem_init(&barrier_sems2[i], 0, 0);  // Init barrier_sems to 0
   }
    sem_init(&count_sem, 0, 1);  // Initialize count_sem to 1 
	for(h = 0; h < numthreads; h = h + 1){
		if(h == 0){
			data[h]->k=0;
			data[h]->i=0;
			data[h]->start_i=0;
			data[h]->stop_i=tope; 
			data[h]->thread=h; 
			data[h]->NumeroThreads=numthreads;
			data[h]->NumeroThreads2=numthreads;
		} 
		else{
			data[h]->k=0;
			data[h]->i=0;
			data[h]->start_i = data[h-1]->start_i + tope;
			data[h]->thread=h;
			data[h]->NumeroThreads=numthreads;
			data[h]->NumeroThreads2=numthreads;
			if(h == (numthreads - 1))
				data[h]->stop_i = matsize;
			else
				data[h]->stop_i = data[h-1]->stop_i + tope;
					
		}
		pthread_create(&thread[h], &attribute, first_function, (void *) data[h]);
	}
	
	//for(h = 0; h < numthreads; h = h + 1)
		
		
	
    for (i = 0; i < numthreads; i = i + 1)
        pthread_join(thread[i],&exit_status);
    
    pthread_attr_destroy(&attribute); 
    
	sem_destroy(&count_sem);
   for (i = 0; i < matsize; i++){
   		sem_destroy(&barrier_sems[i]);
   		sem_destroy(&barrier_sems2[i]);
   }








	//printf("Fuente de Variacion 5 \n");
	Determinar_Betas(matriz_B, matriz_Inversa, matriz_XT_por_Y, columnas, filas);
	//printf("Fuente de Variacion 6 \n");
	Tabla_Anova(matriz_Y_circunfleja, matriz_Y, matriz_Inversa,matriz_X, matriz_XT_por_Y, matriz_Aux, filas, columnas);
	//printf("Fuente de Variacion 7 \n");
	gettimeofday(&t_fin, NULL);
	secs = timeval_diff(&t_fin, &t_ini);
	printf("%.3g Segundos\n", secs);


	return 0;
}
//*************************************************************************************************************************************
//*************************************************************************************************************************************
