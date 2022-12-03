#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

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
    char colorActual;
} info_tablero;

// -------------------------------------------------------
// funcion principal

int analizarPartida(char* origen, char* destino);
// -------------------------------------------------------
// checkeo del formato

int procesarArchivo(char* origen, info_tablero* partida);
int leerNombre(FILE* fp, info_tablero* partida);
int colorInicial(FILE* fp, info_tablero* partida);
int obtenerJugadas(FILE* fp, info_tablero* partida);
// -------------------------------------------------------
// analisis de jugadas

void analizarJugadas(info_tablero* partida, char* destino);
int validarJugada(info_tablero* partida, int* jugada, int* direcciones);
int validarLinea(info_tablero* partida, int* jugada, int direccion_x, int direccion_y);
void realizarJugada(info_tablero* partida, int* jugada, int* direcciones);
int casillaEnRango(int* jugada);
void traducirJugada(char* jugada, int* jugadaTraducida);
int movimientoDisponible(info_tablero* partida);
// -------------------------------------------------------
// imprimir

void mostrarTablero(info_tablero* partida);
void exportarPartida(info_tablero* partida, char* destino);
void indicarGanador(info_tablero* partida);
// -------------------------------------------------------
// utilidades

void initPartida(info_tablero* partida);
void liberarPartida(info_tablero* partida);
int tests();

// -------------------------------------------------------

int main(int argc, char* argv[]){

    if (argc == 2 && strcmp("test", argv[1])==0){
        if (!tests()){
            printf("\033[H\033[J");
            printf("Todos los tests correctos\n");
            return 0;
        }
        else{
            return -1;
        }
    }
    if (argc >= 3){
        return analizarPartida(argv[1], argv[2]);
    }
    // printf("Error!! no se ingreso un archivo\n");
    return -1; // Error
}

// -------------------------------------------------------

// recibe un archivo de origen con jugadas que analizar
// y devuelve 0 si se analizó correctamente, -1 si
// hubo un error de formateo. si la partida resulta en empate, se
// imprime el tablero y el color actual en el destino
int analizarPartida(char* origen, char* destino){
    info_tablero partida;

    initPartida(&partida);

    // si no se pudo procesar correctamente, se retorna un error
    if (procesarArchivo(origen, &partida)==-1){
        return -1; // Error
    }

    analizarJugadas(&partida, destino);

    liberarPartida(&partida);
    return 0;
}

// -------------------------------------------------------

int procesarArchivo(char* origen, info_tablero* partida){
    FILE* fp;
    int formatoValido = TRUE;

    fp = fopen(origen, "r");
    if(fp == NULL) {
    //   printf("Error!! no se pudo abrir el archivo\n");
      return -1;
    }
    // leo las primeras 2 lineas, que deberian tener nombre y color
    for (int i = 0; i < 2 && formatoValido; i++)
    {
        formatoValido = leerNombre(fp, partida);
    }
    if (!formatoValido) return -1; // Error!! formato invalido

    // determino el color inicial, en la 3ra linea
    formatoValido = colorInicial(fp, partida);
    if (!formatoValido) return -1; // Error!! formato invalido

    // despues de la 3ra linea, deberian ser todas jugadas
    formatoValido = obtenerJugadas(fp, partida);
    if (!formatoValido) return -1; // Error!! formato invalido

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
            // printf("Error!! cantidad de argumentos erronea, se necesita nombre y color\n");
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

        // printf("Error!! formato del color de jugador invalido\n");
        return FALSE;
    }
    // printf("Error!! no se pudo obtener el nombre\n");
    return FALSE;
}

// -------------------------------------------------------

int colorInicial(FILE* fp, info_tablero* partida){
    char fbuffer[100];
    char c;

    if (fgets(fbuffer, 100, fp)!=NULL){
        for(int i = 0; (c = fbuffer[i])==' '; i++);
        partida->colorActual = c;

        if (partida->colorActual != 'B' && partida->colorActual != 'N'){
            // printf("Error!! color inicial invalido\n");
            return FALSE;
        }
        return TRUE;
    }
    // printf("Error!! no se pudo obtener el color\n");
    return FALSE;
}

// -------------------------------------------------------

int obtenerJugadas(FILE* fp, info_tablero* partida){
    char buffer[10];
    int terminado=FALSE, error=FALSE;

    for (int j = 0; !terminado && !error;){
        j = 0;
        if (fgets(buffer, 10, fp)!=NULL){
            if (buffer[0]==' ' || buffer[0]=='\n' || buffer[0]=='\r'){
                partida->jugadas[partida->cant_jugadas++][0]='\0';
                continue;
            }
            else{
                partida->jugadas[partida->cant_jugadas][j]=buffer[j];
                j++;
            }
            if (buffer[1]!=' ' && buffer[1]!='\n' && buffer[1]!='\r'){
                partida->jugadas[partida->cant_jugadas][j]=buffer[j];
                j++;
            }
            else{
                error=TRUE;
                // printf("Error!! formato de jugada invalido\n");
            }
            if(buffer[2]=='\n' || buffer[2]=='\r'){
                partida->jugadas[partida->cant_jugadas++][j]='\0';
                continue;
            }
            else{
                error=TRUE;
                // printf("Error!! formato de jugada invalido\n");
            }
        }
        else{
            terminado=TRUE;
        }
    }

    if (error){
        return FALSE;
    }
    return TRUE;
}

void analizarJugadas(info_tablero* partida, char* destino){
    int terminado=FALSE, direcciones[8], jugada_valida=0, jugada[2], previoSaltoDeTurno=FALSE;
    // cada posicion en direcciones determina si cierta
    // direccion debe ser modificada con un 1
    // direcciones[0] = (-1, -1) = NorOeste; direcciones[5] = (1, -1) = SurOeste

    for(int i=0; i<partida->cant_jugadas && !terminado; i++){
        if (partida->jugadas[i][0]!='\0'){ // no trata de saltar el turno
            previoSaltoDeTurno=FALSE;
            traducirJugada(partida->jugadas[i], jugada);
            jugada_valida = validarJugada(partida, jugada, direcciones);
            if (jugada_valida){
                realizarJugada(partida, jugada, direcciones);
            }
        }
        else{ // trata de saltar el turno
            jugada_valida=TRUE;
            if(movimientoDisponible(partida)){
                // printf("Error!! no se puede saltear el turno con movimientos disponibles\n");
                jugada_valida=FALSE;
            }
            if (previoSaltoDeTurno && jugada_valida){ // si ocurre 2 veces seguidas, termina la partida
                terminado=TRUE;
            }
            previoSaltoDeTurno=TRUE;
        }
        if(!jugada_valida){
            terminado=TRUE;
            if (partida->colorActual=='B'){
                printf("El jugador %s realizo una jugada invalida\n", partida->jugadorBlanco);
            }
            else{
                printf("El jugador %s realizo una jugada invalida\n", partida->jugadorNegro);
            }
            mostrarTablero(partida);
        }
        partida->colorActual = (partida->colorActual=='B') ? 'N' : 'B';
    }
    if (jugada_valida){
        terminado = !movimientoDisponible(partida);
        if (!terminado){
            exportarPartida(partida, destino);
        }
        else{
            indicarGanador(partida);
        }
    }
}

// -------------------------------------------------------

int validarJugada(info_tablero* partida, int* jugada, int* direcciones){
    int direccion=0, jugada_valida=FALSE, cant_de_cambios=FALSE, adyacente[2];
    char color_adyacente;

    if (!casillaEnRango(jugada)){
        // printf("Error!! la jugada no esta en el tablero\n");
        return FALSE;
    }
    if (partida->tablero[jugada[0]][jugada[1]]!='X'){
        // printf("Error!! la casilla no esta vacia\n");
        return FALSE;
    }
    // recorro todas las direcciones
    for (int i = -1; i < 2; i++){
        for (int j = -1; j < 2; j++){
            if (i==0 && j==0) continue;
            adyacente[0] = jugada[0]+i;
            adyacente[1] = jugada[1]+j;
            // si el adyacente está fuera del tablero:
            if (!casillaEnRango(adyacente)){
                direcciones[direccion] = 0; // no vamos a considerar esta direccion
                direccion++;
                continue;
            }

            color_adyacente = partida->tablero[jugada[0]+i][jugada[1]+j];
            if (color_adyacente=='X' || color_adyacente==partida->colorActual){
                direcciones[direccion] = 0; // no vamos a considerar esta direccion
                direccion++;
                continue;
            }
            else{
                cant_de_cambios = validarLinea(partida, jugada, i, j);
                direcciones[direccion] = cant_de_cambios;
                direccion++;
                if (!jugada_valida && cant_de_cambios!=0){
                    jugada_valida = TRUE;
                }
            }
        }
    }
    if (!jugada_valida){
        // printf("Error!! no hay fichas del adversario adyacentes\n");
        return FALSE;
    }
    return TRUE;
}

// -------------------------------------------------------

int validarLinea(info_tablero* partida, int* jugada, int direccion_x, int direccion_y){
    int adyacente[2];
    int cant_de_cambios=0, terminado=FALSE;
    char color_adyacente;

    adyacente[0] = jugada[0];
    adyacente[1] = jugada[1];
    while(!terminado){
        adyacente[0] += direccion_x;
        adyacente[1] += direccion_y;
        if (!casillaEnRango(adyacente)) return 0;

        color_adyacente = partida->tablero[adyacente[0]][adyacente[1]];
        if(color_adyacente==partida->colorActual){
            terminado=TRUE;
            continue;
        }
        if (color_adyacente=='X'){
            terminado=TRUE;
            cant_de_cambios=0; // la linea no es valida
            continue;
        }
        cant_de_cambios++;
    }

    return cant_de_cambios;
}

// -------------------------------------------------------

void realizarJugada(info_tablero* partida, int* jugada, int* direcciones){
    int direccion = 0, pos_adyacente[2];
    pos_adyacente[0] = jugada[0];
    pos_adyacente[1] = jugada[1];

    partida->tablero[jugada[0]][jugada[1]] = partida->colorActual;
    for (int i = -1; i < 2; i++){
        for (int j = -1; j < 2; j++){
            if (i==0 && j==0) continue;
            for (int cambios = 0; cambios < direcciones[direccion]; cambios++){
                pos_adyacente[0] += i;
                pos_adyacente[1] += j;
                partida->tablero[pos_adyacente[0]][pos_adyacente[1]] = partida->colorActual;
            }
            pos_adyacente[0] = jugada[0];
            pos_adyacente[1] = jugada[1];
            direccion++;
        }
    }
    
}

// -------------------------------------------------------

int casillaEnRango(int* jugada){
    if (jugada[0]>7 || jugada[0]<0 || jugada[1]>7 || jugada[1]<0){
        return FALSE;
    }
    return TRUE;
}

// -------------------------------------------------------

void traducirJugada(char* jugada, int* jugadaTraducida){
    jugadaTraducida[1] = (int)(toupper(jugada[0]))-'A';
    jugadaTraducida[0] = (int)((jugada[1])-'0')-1;
}

// -------------------------------------------------------

int movimientoDisponible(info_tablero* partida){
    int jugada[2], jugada_valida=FALSE, direcciones[8];

    for(int fila=0; fila<CANT_FILAS && !jugada_valida; fila++){
        for(int columna=0; columna<CANT_COLUMNAS && !jugada_valida; columna++){
            if (partida->tablero[fila][columna]=='X'){
                jugada[0] = fila;
                jugada[1] = columna;
                printf("jugada: %d %d\n", jugada[0], jugada[1]);
                jugada_valida = validarJugada(partida, jugada, direcciones);
            }
        }
    }

    return jugada_valida;
}

// -------------------------------------------------------

void mostrarTablero(info_tablero* partida){
    printf("    A | B | C | D | E | F | G | H |\n");
    for(int fila=0; fila<CANT_FILAS; fila++){
        printf("%d | ", fila+1);
        for(int columna=0; columna<CANT_COLUMNAS; columna++){
            if (partida->tablero[fila][columna]=='X'){
                printf(" ");
            }
            else{
                printf("%c", partida->tablero[fila][columna]);
            }
            printf(" | ");
        }
        printf("\n");
    }
}

// -------------------------------------------------------

void exportarPartida(info_tablero* partida, char* destino){
    FILE* fp;
    fp = fopen(destino, "w");

    for(int fila=0; fila<CANT_FILAS; fila++){
        for(int columna=0; columna<CANT_COLUMNAS; columna++){
            fprintf(fp, "%c", partida->tablero[fila][columna]);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "%c\n", partida->colorActual);

    fclose(fp);
}

// -------------------------------------------------------

void indicarGanador(info_tablero* partida){
    int puntos_blancas = 0, puntos_negras = 0;
    char color;
    for(int fila=0; fila<CANT_FILAS; fila++){
        for(int columna=0; columna<CANT_COLUMNAS; columna++){
            color = partida->tablero[fila][columna];
            if (color=='B') puntos_blancas++;
            else if (color=='N') puntos_negras++;
        }
    }
    if (puntos_blancas>puntos_negras){
        printf("El ganador es %s, con las fichas blancas (%d-%d)\n", partida->jugadorBlanco, puntos_blancas, puntos_negras);
    }
    else if(puntos_blancas<puntos_negras){
        printf("El ganador es %s, con las fichas negras (%d-%d)\n", partida->jugadorNegro, puntos_blancas, puntos_negras);
    }
    else{
        printf("La partida termino en empate. (%d-%d)\n", puntos_blancas, puntos_negras);
    }
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

int tests(){
    FILE* fp;
    info_tablero partida;
    int jugada[2], direcciones[8];

    // tests de analizarPartida
    assert(analizarPartida("./recursos/prueba.txt", "./recursos/tablero.txt")==0);
    assert(analizarPartida("./recursos/prueba2.txt", "./recursos/tablero.txt")==-1);


    // tests de procesarArchivo
    initPartida(&partida);
    assert(procesarArchivo("./recursos/prueba.txt", &partida)==0);
    liberarPartida(&partida);
    initPartida(&partida);
    assert(procesarArchivo("./recursos/prueba2.txt", &partida)==-1);
    liberarPartida(&partida);

    // tests de leerNombre
    initPartida(&partida);
    
    fp = fopen("./recursos/prueba.txt", "r");
    assert(leerNombre(fp, &partida)==TRUE);
    fclose(fp);
    liberarPartida(&partida);

    // tests de colorInicial
    initPartida(&partida);

    fp = fopen("./recursos/prueba.txt", "r");
    for (char i; (i=fgetc(fp)!='\n');); // salteo la linea 1
    for (char i; (i=fgetc(fp)!='\n');); // y la linea 2
    assert(colorInicial(fp, &partida)==TRUE);
    fclose(fp);

    // tests de validarJugada
    initPartida(&partida);
    partida.jugadorBlanco = "Santiago";
    partida.jugadorNegro = "Pepe";
    partida.colorActual = 'B';

    traducirJugada("C5", jugada);
    assert(validarJugada(&partida, jugada, direcciones)==TRUE);
    traducirJugada("A1", jugada);
    assert(validarJugada(&partida, jugada, direcciones)==FALSE);

    // tests de validarLinea
    initPartida(&partida);
    partida.jugadorBlanco = "Santiago";
    partida.jugadorNegro = "Pepe";
    partida.colorActual = 'B';

    traducirJugada("C5", jugada);
    assert(validarLinea(&partida, jugada, 0, 1)==TRUE);
    assert(validarLinea(&partida, jugada, -1, 1)==FALSE);

    // tests de casillaEnRango
    traducirJugada("A8", jugada);
    assert(casillaEnRango(jugada)==TRUE);
    // por como funciona el programa, solo se toman 2 caracteres
    // del archivo por linea y tira un error de formato si hay mas,
    // por lo que traducirJugada solo toma los primeros 2 caracteres del string
    // y por lo tanto "A10"="A1", que está en el tablero.
    traducirJugada("A10", jugada);
    assert(casillaEnRango(jugada)==TRUE);
    traducirJugada("PA", jugada);
    assert(casillaEnRango(jugada)==FALSE);
    traducirJugada("66", jugada);
    assert(casillaEnRango(jugada)==FALSE);

    // tests de movimientoDisponible
    initPartida(&partida);
    partida.colorActual = 'B';
    assert(movimientoDisponible(&partida)==TRUE);

    initPartida(&partida);
    procesarArchivo("./recursos/terminada.txt", &partida);
    analizarJugadas(&partida, "./recursos/tablero.txt");
    assert(movimientoDisponible(&partida)==FALSE);
    liberarPartida(&partida);

    return 0;
}

// -------------------------------------------------------
