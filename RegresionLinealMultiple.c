#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#define size 50000
#define size2 25000
 
 
float matriz_X[size][200];
float matriz_X_T[200][size];
float matriz_XT_por_X[500][500];
float matriz_XT_por_Y[200][1];
float matriz_Y[size][1];
float matriz_Inversa[500][500];    
float matriz_B[500][500];
float matriz_Y_circunfleja[size][1]; 
float matriz_Aux[size][200];
int filas = 47000;
int columnas = 50;

//*************************************************************************************************************************
/* retorna "a - b" en segundos */
double timeval_diff(struct timeval *a, struct timeval *b)
{
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
} 
 
//************************************************************************************************************************************* 
 
void Generar_MatrizX_Y(float matriz_X[size][200], float matriz_Y[size][1], int filas, int columnas){
    FILE *Archivo;
// GENERAR LA MATRIZ X CON LOS 1 Y LA MATRIZ Y
    Archivo = fopen("prueba.txt","r");
    int i,j;
    if(Archivo==NULL)
        printf("error");
    for(i=0;i<filas;i++){
				fscanf(Archivo, "%f ", &matriz_Y[i][0]);
				//printf("%.2f \n", matriz_Y[i][0]); 
        for(j=0;j<columnas;j++){
						if(j == 0){
								matriz_X[i][j] = 1;
								//printf("%.2f ", matriz_X[i][j]);
						}
						else{
								fscanf(Archivo, "%f ", &matriz_X[i][j]); //se guarda en un array
								//printf("%.2f  ", matriz_X[i][j]);  //  y se imprime a la vez (aprovechamos por que el bucle es el mismo)			
						}
        }
        //printf("\n");      //cada vez que se termina una fila hay que pasar a la siguiente linea
    }
    fclose(Archivo);	
 	
}
 
//************************************************************************************************************************************* 
 
void Generar_MatrizX_Traspuesta(float matriz_X[size][200], float matriz_X_T[200][size], int filas, int columnas){
 	
		int i,j;
    for(i=0;i<columnas;i++){
				for(j=0;j<filas;j++){
						matriz_X_T[i][j] = matriz_X[j][i];
				}
    }
    
    //for(i=0;i<columnas;i++){
      //  for(j=0;j<filas;j++){
		//	printf("%.2f ", matriz_X_T[i][j]);		
        //}
        //printf("\n");      //cada vez que se termina una fila hay que pasar a la siguiente linea
    //}	    
}
 
//*************************************************************************************************************************************

void Multiplicar_MatrizXT_por_MatrizX(float matriz_X[size][200], float matriz_X_T[200][size], float matriz_XT_por_X[500][500], int filas,int columnas){
  	
	int i, j, k;
  	float aux = 0;
  	for(k=0;k<filas;k++){
	    for(i=0;i<filas;i++){
			for(j=0;j<columnas;j++){
				aux = aux + (matriz_X_T[i][j]*matriz_X[j][k]);
			} 
			matriz_XT_por_X[i][k] = aux;
			//printf(" %.2f ", matriz_XT_por_X[i][k]);
			aux = 0;
		}
		//printf("\n");  
		 			
	}

}

//*************************************************************************************************************************************

void Multiplicar_MatrizXT_por_MatrizY(float matriz_X_T[200][size], float matriz_Y[size][1], float matriz_XT_por_Y[200][1],int filas,int columnas){
	
  	int i, j;
  	float aux = 0;
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

void Traspuesta_Adjunta(float MatrizAdjT[200][200], float matriz_XT_por_X[500][500]){
	
		int i,j;
    for(i=0;i<3;i++){
        for(j=0;j<3;j++){
						MatrizAdjT[j][i] = matriz_XT_por_X[i][j];
				}	
		}
}
 
//*************************************************************************************************************************************

void Determinar_Inversa(float matriz_XT_por_X[500][500], float matriz_Inversa[500][500], int filas, int columnas){
	
		float determinante;
		float aux1;
		float aux2;
		float MatrizAdjT[200][200];
	

		if(filas == 8 && columnas == 3){
				aux1 = matriz_XT_por_X[0][0]*matriz_XT_por_X[1][1]*matriz_XT_por_X[2][2]+matriz_XT_por_X[1][0]*matriz_XT_por_X[2][1]*matriz_XT_por_X[0][2]+matriz_XT_por_X[0][1]*matriz_XT_por_X[1][2]*matriz_XT_por_X[2][0];
				aux2 = (matriz_XT_por_X[2][0]*matriz_XT_por_X[1][1]*matriz_XT_por_X[0][2])+(matriz_XT_por_X[1][2]*matriz_XT_por_X[2][1]*matriz_XT_por_X[0][0])+(matriz_XT_por_X[0][1]*matriz_XT_por_X[1][0]*matriz_XT_por_X[2][2]);
				determinante = aux1 - aux2;
				//printf("%.4f ", determinante);
				Traspuesta_Adjunta(MatrizAdjT, matriz_XT_por_X);
				matriz_Inversa[0][0] = ((MatrizAdjT[1][1]*MatrizAdjT[2][2]) - (MatrizAdjT[2][1]*MatrizAdjT[1][2]))/determinante;
				matriz_Inversa[0][1] = (-((MatrizAdjT[1][0]*MatrizAdjT[2][2]) - (MatrizAdjT[2][0]*MatrizAdjT[1][2])))/determinante;
				matriz_Inversa[0][2] = ((MatrizAdjT[1][0]*MatrizAdjT[2][1]) - (MatrizAdjT[2][0]*MatrizAdjT[1][1]))/determinante;
				matriz_Inversa[1][0] = (-((MatrizAdjT[0][1]*MatrizAdjT[2][2]) - (MatrizAdjT[2][1]*MatrizAdjT[0][2])))/determinante;
				matriz_Inversa[1][1] = ((MatrizAdjT[0][0]*MatrizAdjT[2][2]) - (MatrizAdjT[2][0]*MatrizAdjT[0][2]))/determinante;
				matriz_Inversa[1][2] = (-((MatrizAdjT[0][0]*MatrizAdjT[2][1]) - (MatrizAdjT[2][0]*MatrizAdjT[0][1])))/determinante;
				matriz_Inversa[2][0] = ((MatrizAdjT[0][1]*MatrizAdjT[1][2]) - (MatrizAdjT[1][1]*MatrizAdjT[0][2]))/determinante;
				matriz_Inversa[2][1] = (-((MatrizAdjT[0][0]*MatrizAdjT[1][2]) - (MatrizAdjT[1][0]*MatrizAdjT[0][2])))/determinante;
				matriz_Inversa[2][2] = ((MatrizAdjT[0][0]*MatrizAdjT[1][1]) - (MatrizAdjT[1][0]*MatrizAdjT[0][1]))/determinante;
		
		}
		else{
				int i, j, k, s;  
				float coef, aux[250], elemento; 	  
				k = filas;
				for (i=0;i<k;i++)
						for(j=k;j<2*k;j++)
						{
								if(i==(j-k))
										matriz_XT_por_X[i][j]=1;
								else
									matriz_XT_por_X[i][j]=0;
						}
					//Iteraciones
				for (s=0;s<k;s++)
				{
						elemento=matriz_XT_por_X[s][s];
						for (j=0;j<2*k;j++)
								matriz_XT_por_X[s][j]=matriz_XT_por_X[s][j]/elemento;

						for(i=0;i<k;i++)
						{
								if (i==s)
										;
								else 
								{
										coef= matriz_XT_por_X[i][s];
										for (j=0;j<2*k;j++)
												aux[j]=matriz_XT_por_X[s][j]*(coef*-1);
										for (j=0;j<2*k;j++)
												matriz_XT_por_X[i][j]=matriz_XT_por_X[i][j]+aux[j];
								}
						}
				}
				int posicion = 0;
				for(i=0;i<k;i++){
        		for(j=k;j<2*k;j++){
								matriz_Inversa[i][posicion]=matriz_XT_por_X[i][j];
								//printf("%.1f  ", matriz_Inversa[i][posicion]);
								posicion++;
						}
						//printf("\n %.i", posicion);
						posicion =0;
    		}
		}
	
}
 
//*************************************************************************************************************************************

void Determinar_Betas(float matriz_B[500][500], float matriz_Inversa[500][500], float matriz_XT_por_Y[200][1], int filas, int columnas){
	
  	int i, j;
  	float aux = 0;
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

void Tabla_Anova(float matriz_Y_circunfleja[size][1], float matriz_Y[size][1], float matriz_Inversa[500][500], float matriz_X[size][200], float matriz_XT_por_Y[200][1], float matriz_Aux[size][200], int filas, int columnas){
	
  	int i, j, k;
  	float aux = 0;
  	float SCM=0;
  	float SCT=0;
  	float SCR=0;
  	int GL_Modelo = columnas - 1;
  	int GL_Residuos = filas - GL_Modelo - 1;
  	int GL_Totales = filas - 1;
  	float Media_Modelo;
  	float Media_Residuos;
  	float Coef_F;
  	
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
	
    float y_promedio = 0;
    for(i=0;i<filas;i++){
				y_promedio = y_promedio + matriz_Y[i][0];
		}
		
    y_promedio = y_promedio/filas;
		for(i=0;i<filas;i++){
				SCM = SCM + pow((matriz_Y_circunfleja[i][0] - y_promedio), 2);
		}

		//printf("SCM = %.6f \n", SCM );
	
    for(i=0;i<filas;i++){
				SCT = SCT + pow((matriz_Y[i][0] - y_promedio), 2);
		}
		//printf("SCT = %.6f \n", SCT );	
	
		for(i=0;i<filas;i++){
				SCR = SCR + pow((matriz_Y[i][0] - matriz_Y_circunfleja[i][0]), 2);
		}
		//printf("SCR = %.6f \n", SCR );
		Media_Modelo = SCM/GL_Modelo;
		//printf("%.6f \n", Media_Modelo );
		Media_Residuos = SCR/GL_Residuos;
		//printf("%.6f \n", Media_Residuos );
		Coef_F = Media_Modelo/Media_Residuos;
		//printf("%.6f \n", Coef_F );

		printf("Fuente de Variacion           SC          G.L.         Media           Cociente F\n");
		printf("     Modelo              %.6f       %.i         %.6f        %.6f \n",SCM, GL_Modelo, Media_Modelo, Coef_F);
		printf(" Residuos/Errores        %.6f       %.i         %.6f\n", SCR, GL_Residuos, Media_Residuos);
    printf("      Total              %.6f       %.i\n", SCT, GL_Totales);
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
		Multiplicar_MatrizXT_por_MatrizX(matriz_X, matriz_X_T, matriz_XT_por_X, columnas, filas);
		//printf("Fuente de Variacion 3 \n");
		Multiplicar_MatrizXT_por_MatrizY(matriz_X_T, matriz_Y, matriz_XT_por_Y, columnas, filas);
		//printf("Fuente de Variacion 4 \n");
		Determinar_Inversa(matriz_XT_por_X, matriz_Inversa, columnas, columnas);
		//printf("Fuente de Variacion 5 \n");
		Determinar_Betas(matriz_B, matriz_Inversa, matriz_XT_por_Y, columnas, filas);
		//printf("Fuente de Variacion 6 \n");
		Tabla_Anova(matriz_Y_circunfleja, matriz_Y, matriz_Inversa,matriz_X, matriz_XT_por_Y, matriz_Aux, filas, columnas);
		//printf("Fuente de Variacion 7 \n");
		gettimeofday(&t_fin, NULL);

		secs = timeval_diff(&t_fin, &t_ini);
		printf("%.16g milliseconds\n", secs * 1000.0);


		return 0;
}
//*************************************************************************************************************************************

