#Este programa multiplica una matriz cuadrada sacando los datos de un archivo.

matriz.inicial <- read.table("inversa.txt", sep = " ")
filas = round(nrow(matriz.inicial), 0)
columnas = round(ncol(matriz.inicial), 0)

x1<-2:columnas

matriz.x <- as.matrix(cbind(1, matriz.inicial[,x1]))
tiempo_inicio<-Sys.time()
matriz.multiplicada <- matriz.x%*%matriz.x
tiempo_fin <- Sys.time()  
   
tiempo_fin-tiempo_inicio
#matriz.beta <- solve(t(matriz.x)%*%matriz.x)
