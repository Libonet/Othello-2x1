#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define CANT_FILAS 8
#define CANT_COLUMNAS 8
#define TRUE 1
#define FALSE 0

// esta estructura define la informacion necesaria
// para representar la partida de Othello
typedef struct Partida{
    char tablero[CANT_FILAS][CANT_COLUMNAS];
    char* jugadorBlanco;
    char* jugadorNegro;
    char jugadas[60][3];
    int cant_jugadas;
    char colorInicial;
    char colorActual;
} info_tablero;

// -------------------------------------------------------

// funcion principal
int analizarPartida(char* origen, char* destino);
// checkeo del formato
int procesarArchivo(char* origen, info_tablero* partida);
int leerNombre(FILE* fp, info_tablero* partida);
int colorInicial(FILE* fp, info_tablero* partida);
// analisis de jugadas
void analizarJugadas(info_tablero* partida, char* destino);
int validarJugada(info_tablero* partida);
int validarLinea(info_tablero* partida);
void realizarJugada(info_tablero* partida);
// imprimir
void mostrarTablero(info_tablero* partida);
// utilidades
void initPartida(info_tablero* partida);
void liberarPartida(info_tablero* partida);
void tests();

// -------------------------------------------------------

int main(int argc, char* argv[]){
    tests();
    if (argc >= 3){
        return analizarPartida(argv[1], argv[2]);
    }
    printf("error: no se ingreso un archivo\n");
    return -1; // error
}

// -------------------------------------------------------

int analizarPartida(char* origen, char* destino){
    info_tablero partida;

    initPartida(&partida);

    // si no se pudo procesar correctamente, se retorna un error
    if (procesarArchivo(origen, &partida)){
        return -1; // error
    }

    analizarJugadas(&partida, destino);

    liberarPartida(&partida);
    return 0;
}

// -------------------------------------------------------

int procesarArchivo(char* origen, info_tablero* partida){
    FILE* fp;
    char c;
    int formatoValido = TRUE, j=0;

    fp = fopen(origen, "r");
    if(fp == NULL) {
      printf("error: no se pudo abrir el archivo");
      return -1;
    }
    // leo las primeras 2 lineas, que deberian tener nombre y color
    for (int i = 0; i < 2 && formatoValido; i++)
    {
        formatoValido = leerNombre(fp, partida);
    }
    if (!formatoValido){
        return -1; // error: formato invalido
    }
    // determino el color inicial, en la 3ra linea
    formatoValido = colorInicial(fp, partida);
    if (!formatoValido){
        return -1; // error: formato invalido
    }
    // despues de la 3ra linea, deberian ser todas jugadas
    j=0;
    while((c=fgetc(fp))!=EOF){
        partida->jugadas[partida->cant_jugadas][j++] = c;
        if (c=='\n'){
            partida->jugadas[partida->cant_jugadas][j] = '\0';
            partida->cant_jugadas++;
            j=0;
        }
    }
    fclose(fp);

    return 0;
}

// -------------------------------------------------------

int leerNombre(FILE* fp, info_tablero* partida){
    char fbuffer[100], nombre[100];
    char color=' ', c;
    int cantidad_de_argumentos, terminado=FALSE, size_nombre=0, j=0;
    if (fgets(fbuffer, 100, fp)!=NULL){
        for(int i=0; (c = fbuffer[i]) != '\n' && c != '\r' && !terminado; i++){
            if (c==',' && i!=0){
                nombre[size_nombre] = '\0';
                // despues de ',' ignora todos los espacios y obtiene el primer caracter
                for(j=i+1; color==' '; j++){ 
                    color = fbuffer[j];
                }
                cantidad_de_argumentos = 2;
                terminado=TRUE;
            }
            else if (c!=' '){
                nombre[size_nombre] = c;
                size_nombre++;
            }
        }
        if (cantidad_de_argumentos != 2){
            printf("error: cantidad de argumentos erronea, se necesita nombre y color\n");
            return FALSE;
        }
        if (color=='B' && partida->jugadorBlanco==NULL && (fbuffer[j]==' ' || fbuffer[j]=='\n' || fbuffer[j]=='\r')){
            partida->jugadorBlanco = malloc(sizeof(char)*size_nombre);
            if (partida->jugadorBlanco==NULL){
                return FALSE;
            }
            strcpy(partida->jugadorBlanco, nombre);
            return TRUE;
        }
        if(color=='N' && partida->jugadorNegro==NULL && (fbuffer[j]==' ' || fbuffer[j]=='\n' || fbuffer[j]=='\r')){
            partida->jugadorNegro = malloc(sizeof(char)*size_nombre);
            if (partida->jugadorNegro==NULL){
                return FALSE;
            }
            strcpy(partida->jugadorNegro, nombre);
            return TRUE;
        }

        printf("error: formato del color de jugador invalido\n");
        return FALSE;
    }
    printf("error: no se pudo obtener el nombre\n");
    return FALSE;
}

// -------------------------------------------------------

int colorInicial(FILE* fp, info_tablero* partida){
    char fbuffer[100];
    char c;

    if (fgets(fbuffer, 100, fp)!=NULL){
        for(int i = 0; (c = fbuffer[i])==' '; i++);
        partida->colorInicial = c;

        if (partida->colorInicial != 'B' && partida->colorInicial != 'N'){
            printf("error: color inicial invalido\n");
            return FALSE;
        }
        partida->colorActual = partida->colorInicial;
        return TRUE;
    }
    printf("error: no se pudo obtener el color\n");
    return FALSE;
}

// -------------------------------------------------------

void analizarJugadas(info_tablero* partida, char* destino){
    for(int i=0; i<partida->cant_jugadas; i++){

    }
}

// -------------------------------------------------------

void mostrarTablero(info_tablero* partida){
    for(int fila=0; fila<CANT_FILAS; fila++){
        for(int columna=0; columna<CANT_COLUMNAS; columna++){
            printf("%c", partida->tablero[fila][columna]);
        }
        printf("\n");
    }
    printf("%c\n", partida->colorActual);
}

// -------------------------------------------------------

void initPartida(info_tablero* partida){
    int centroF, centroC;
    for(int i=0; i<CANT_FILAS; i++){
        for(int j=0; j<CANT_COLUMNAS; j++){
            partida->tablero[i][j] = 'X';
        }
    }
    // como estamos jugando Othello, el tablero comienza con 4 fichas en el centro del tablero
    centroF = CANT_FILAS/2;
    centroC = CANT_COLUMNAS/2;
    partida->tablero[centroF-1][centroC-1] = 'B';
    partida->tablero[centroF-1][centroC] = 'N';
    partida->tablero[centroF][centroC-1] = 'N';
    partida->tablero[centroF][centroC] = 'B';

    partida->cant_jugadas = 0;

    partida->jugadorBlanco=NULL;
    partida->jugadorNegro=NULL;
}

// -------------------------------------------------------

void liberarPartida(info_tablero* partida){
    free(partida->jugadorBlanco);
    free(partida->jugadorNegro);
}

// -------------------------------------------------------

void tests(){

}

// -------------------------------------------------------
