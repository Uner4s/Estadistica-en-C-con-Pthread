#Este programa realiza la regresión con los comandos de R
matriz.inicial <- read.table("prueba.txt", sep = " ")

tiempo_inicio<-Sys.time()
modelo<-lm(matriz.inicial)

modelo

anova(modelo)

tiempo_fin <- Sys.time()  
   
tiempo_fin-tiempo_inicio

