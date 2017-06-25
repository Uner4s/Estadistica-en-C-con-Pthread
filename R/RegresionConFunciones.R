
##############################################
#ACA COMIENZA EL PROGRAMA
#Este programa resuelve la regresion multiple utilizando la multiplicacion de R y la inversa de R como funciones propias
matriz.inicial <- read.table("prueba.txt", sep = " ")

tiempo_inicio<-Sys.time()
matriz.y <- as.matrix(matriz.inicial[, 1:1])

filas = round(nrow(matriz.inicial), 0)
columnas = round(ncol(matriz.inicial), 0)

x1<-2:columnas

matriz.x <- as.matrix(cbind(1, matriz.inicial[,x1]))

matriz.beta <- solve(t(matriz.x)%*%matriz.x)%*%t(matriz.x)%*%matriz.y

#anova

SCM <- 0
SCT <- 0
SCR <- 0
GL_Modelo <- columnas - 1
GL_Residuos <- filas - columnas
GL_Totales <- filas - 1
i <- 1

#y gorrito
matriz.circunfleja <-matriz.x%*%matriz.beta

# y promedio

y_promedio <- sum(matriz.y)/filas

#SCM

cuadradomodelo <- function(matriz.circunfleja,y_primedio){
  for (i in 1:filas){
    SCM <- SCM + (matriz.circunfleja[i,1] - y_promedio)^2
  }
  return(SCM)
}


SCM <- cuadradomodelo(matriz.circunfleja,y_primedio)

#SCT

cuadradototal <- function(matriz.y,filas, y_promedio){
  for (i in 1:filas){
    SCT <- SCT + (matriz.y[i,1]- y_promedio)^2
  }
  return(SCT)
}

SCT <- cuadradototal(matriz.y,filas,y_promedio)

#SCR

cuadradoresiduos <- function(matriz.y,matriz.circunfleja,filas){
  for (i in 1:filas){
    SCR <- SCR + (matriz.y[i,1] - matriz.circunfleja[i,1])^2
  }
  return(SCR)
}

SCR <- cuadradoresiduos(matriz.y,matriz.circunfleja,filas)

#CMM
Media_Modelo <- SCM/GL_Modelo

#CME
Media_Residuos <- SCR/GL_Residuos

#F
Coef_F <- Media_Modelo/Media_Residuos

tiempo_fin <- Sys.time()     # guarda tiempo de fin de ejecucion
tiempo_fin-tiempo_inicio     # calcula duracion total de procesos


