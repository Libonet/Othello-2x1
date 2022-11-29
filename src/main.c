#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CANT_FILAS 8
#define CANT_COLUMNAS 8
#define TRUE 1
#define FALSE 0

typedef struct Partida{
    char tablero[CANT_FILAS][CANT_COLUMNAS];
    char* jugadorBlanco;
    char* jugadorNegro;
    char colorInicial;
} info_tablero;

int procesarPartida(char* path);
int leerNombre(FILE* fp, info_tablero* partida);
void liberarPartida(info_tablero* partida);

int main(int argc, char* argv[]){
    if (argc >= 2){
        return procesarPartida(argv[1]);
    }
    return -1; // error
}

int procesarPartida(char* path){
    FILE* fp;
    info_tablero partida;
    partida.jugadorBlanco=NULL;
    partida.jugadorNegro=NULL;

    int valido = TRUE;
    fp = fopen(path, "r");

    // leo las primeras 2 lineas, que deberian tener nombre y color
    for (int i = 0; i < 2 && valido; i++)
    {
        valido = leerNombre(fp, &partida);
    }
    if (!valido){
        return -1; // error: formato invalido
    }
    // determino el color inicial
    partida.colorInicial = fgetc(fp);

    fclose(fp);

    liberarPartida(&partida);

    return 0;
}

int leerNombre(FILE* fp, info_tablero* partida){
    char buffer[100];
    char color, c;
    int cantidad_de_argumentos, terminado=FALSE, size_nombre=0;
    fgets(buffer, 100, fp);
    for(int i=0; (c = buffer[i]) != '\n' && c != '\r' && !terminado; i++){
        if (c==',' && i!=0){
            buffer[i] = '\0';
            color = buffer[i+1];
            size_nombre = i;
            cantidad_de_argumentos = 2;
            terminado=TRUE;
        }
    }
    if (cantidad_de_argumentos != 2){
        printf("error: cantidad de argumentos erronea\n");
        return FALSE;
    }
    if (color=='B' && partida->jugadorBlanco==NULL){
        partida->jugadorBlanco = malloc(sizeof(char)*size_nombre+1);
        strcpy(partida->jugadorBlanco, buffer);
        printf("%s,%c\n", partida->jugadorBlanco, color);
        return TRUE;
    }
    if(color=='N' && partida->jugadorNegro==NULL){
        partida->jugadorNegro = malloc(sizeof(char)*size_nombre+1);
        strcpy(partida->jugadorNegro, buffer);
        printf("%s,%c\n", partida->jugadorNegro, color);
        return TRUE;
    }

    printf("error: color invalido\n");
    return FALSE;
}

void liberarPartida(info_tablero* partida){
    free(partida->jugadorBlanco);
    free(partida->jugadorNegro);
}