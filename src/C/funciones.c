#include "funciones.h"

/* Analiza una partida de Othello
    args:
        origen: path al archivo con jugadas que analizar
    returns:
        0 si se analizó correctamente, -1 si hubo un error de formateo. */
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

/* Obtiene la información de la partida almacenada en un archivo 
    args:
        origen: path a un archivo de partida de othello
        partida: estructura en la que almaceno la informacion de la partida
    returns:
        0 si funcionó correctamente
        -1 si hubo errores de formato */
int procesarArchivo(char* origen, info_tablero* partida){
    FILE* fp;
    int formatoValido = TRUE;

    fp = fopen(origen, "r");
    if(fp == NULL) {
      printf("Error!! no se pudo abrir el archivo\n");
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

/* Lee una linea del archivo con formato 'nombre,color'
    args:
        fp: puntero al archivo previamente abierto
        partida: estructura en la que almaceno la informacion de la partida
    returns:
        TRUE si logró almacenar el nombre
        FALSE si falló */
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
            printf("Error!! cantidad de argumentos erronea, se necesita nombre y color\n");
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

        printf("Error!! formato del color de jugador invalido\n");
        return FALSE;
    }
    printf("Error!! no se pudo obtener el nombre\n");
    return FALSE;
}

// -------------------------------------------------------

/* Lee una linea del archivo con formato 'color'
    args:
        fp: puntero al archivo previamente abierto
        partida: estructura en la que almaceno la informacion de la partida
    returns:
        TRUE si logró almacenar el color
        FALSE si falló */
int colorInicial(FILE* fp, info_tablero* partida){
    char fbuffer[100];
    char c;

    if (fgets(fbuffer, 100, fp)!=NULL){
        for(int i = 0; (c = fbuffer[i])==' '; i++);
        partida->colorActual = c;

        if (partida->colorActual != 'B' && partida->colorActual != 'N'){
            printf("Error!! color inicial invalido\n");
            return FALSE;
        }
        return TRUE;
    }
    printf("Error!! no se pudo obtener el color\n");
    return FALSE;
}

// -------------------------------------------------------

/* lee desde el puntero fp hasta el final del archivo
    jugadas de la forma 'columna-fila' o ' '
    args:
        fp: puntero al archivo previamente abierto
        partida: estructura en la que almaceno la informacion de la partida
    returns:
        TRUE si logró almacenar las jugadas
        FALSE si falló */
int obtenerJugadas(FILE* fp, info_tablero* partida){
    char letra; // buffer[10];
    int terminado=FALSE, error=FALSE;

    for (int i = 0; !terminado && !error;){
        letra = fgetc(fp);
        if (letra=='\r'){
            letra = fgetc(fp);
        }

        if (i==0 || i==2){
            if (letra==EOF){
                partida->jugadas[partida->cant_jugadas++][i]='\0';
                terminado=TRUE;
                continue;
            }
            if (letra=='\n'){
                partida->jugadas[partida->cant_jugadas++][i]='\0';
                i=0;
                continue;
            }
        }
        if (i==1 && (letra==EOF || letra=='\n')){
            error=TRUE;
            continue;
        }
        partida->jugadas[partida->cant_jugadas][i]=letra;
        i++;
    }

    return !error;
}

/* analiza la lista de jugadas guardadas en la estructura partida
    args:
        partida: estructura en la que almaceno la informacion de la partida
        destino: path al archivo en el que se imprime el tablero si la partida está incompleta */
void analizarJugadas(info_tablero* partida, char* destino){
    int terminado=FALSE, direcciones[8], jugada_valida=0, jugada[2], previoSaltoDeTurno=FALSE;
    // cada posicion en direcciones determina la cantidad
    // de fichas que se modificarían en cierta direccion
    // la direccion se determina por la posición en 'direcciones'
    // 0 = (-1, -1) = NorOeste; 5 = (-1, 1) = SurOeste

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
                printf("Error!! no se puede saltear el turno con movimientos disponibles\n");
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
                printf("El jugador %s realizo una jugada invalida (%s)\n", partida->jugadorBlanco, partida->jugadas[i]);
            }
            else{
                printf("El jugador %s realizo una jugada invalida (%s)\n", partida->jugadorNegro, partida->jugadas[i]);
            }
            mostrarTablero(partida);
        }
        partida->colorActual = (partida->colorActual=='B') ? 'N' : 'B';
    }
    if (jugada_valida){
        // terminado = checkearEspaciosVacios(partida);
        terminado = !movimientoDisponible(partida);
        if (terminado){
            partida->colorActual = (partida->colorActual=='B') ? 'N' : 'B';
            terminado = !movimientoDisponible(partida);
            partida->colorActual = (partida->colorActual=='B') ? 'N' : 'B';
        }
        if (!terminado){
            exportarPartida(partida, destino);
        }
        else{
            indicarGanador(partida);
        }
    }
}

// -------------------------------------------------------

/* analiza si una jugada es valida
    args:
        partida: estructura en la que almaceno la informacion de la partida
        jugada: lista de 2 posiciones con el (x, y) de la jugada
        direcciones: lista de 8 posiciones, cada una determina
            una direccion específica, y se guarda 
            cuántos cambios hay en esa dirección
    returns:
        jugada_valida: 'booleano' verdadero o falso dependiendo de la validez de la jugada */
int validarJugada(info_tablero* partida, int* jugada, int* direcciones){
    int direccion=0, jugada_valida=FALSE, cant_de_cambios=FALSE, adyacente[2];
    char color_adyacente;

    if (!casillaEnRango(jugada)){
        // printf("Error!! la jugada no esta en el tablero\n");
        return FALSE;
    }
    if (partida->tablero[jugada[1]][jugada[0]]!='X'){
        // printf("Error!! la casilla no esta vacia\n");
        return FALSE;
    }
    // recorro todas las direcciones
    for (int vecY = -1; vecY < 2; vecY++){
        for (int vecX = -1; vecX < 2; vecX++){
            if (vecX==0 && vecY==0) continue;
            adyacente[0] = jugada[0]+vecX;
            adyacente[1] = jugada[1]+vecY;
            // si el adyacente está fuera del tablero:
            if (!casillaEnRango(adyacente)){
                direcciones[direccion] = 0; // no vamos a considerar esta direccion
                direccion++;
                continue;
            }

            color_adyacente = partida->tablero[adyacente[1]][adyacente[0]];
            if (color_adyacente=='X' || color_adyacente==partida->colorActual){
                direcciones[direccion] = 0; // no vamos a considerar esta direccion
                direccion++;
                continue;
            }
            else{
                cant_de_cambios = validarLinea(partida, jugada, vecX, vecY);
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
    }
    return jugada_valida;
}

// -------------------------------------------------------

/* valida si al recorrer el tablero en cierta dirección se logra una jugada valida
    args:
        partida: estructura en la que almaceno la informacion de la partida
        jugada: lista de 2 posiciones con el (x, y) de la jugada
        direccion_x: el modificador del x que determina la direccion (-1, 0, 1)
        direccion_y: el modificador del y que determina la direccion (-1, 0, 1)
    returns:
        cant_de_cambios: cantidad de casillas modificadas en la direccion validada */
int validarLinea(info_tablero* partida, int* jugada, int direccion_x, int direccion_y){
    int adyacente[2];
    int cant_de_cambios=0, terminado=FALSE;
    char color_adyacente;

    adyacente[1] = jugada[1];
    adyacente[0] = jugada[0];
    while(!terminado){
        adyacente[0] += direccion_x;
        adyacente[1] += direccion_y;
        if (!casillaEnRango(adyacente)) return 0;

        color_adyacente = partida->tablero[adyacente[1]][adyacente[0]];
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

/* modifica el tablero en la casilla indicada por jugada
    args:
        partida: estructura en la que almaceno la informacion de la partida
        jugada: lista de 2 posiciones con el (x, y) de la jugada
        direcciones: lista de 8 posiciones, cada una determina
            una direccion específica, y se guarda 
            cuántos cambios hay en esa dirección */
void realizarJugada(info_tablero* partida, int* jugada, int* direcciones){
    int direccion = 0, pos_adyacente[2];
    pos_adyacente[0] = jugada[0];
    pos_adyacente[1] = jugada[1];

    partida->tablero[jugada[1]][jugada[0]] = partida->colorActual;
    for (int vecY = -1; vecY < 2; vecY++){
        for (int vecX = -1; vecX < 2; vecX++){
            if (vecX==0 && vecY==0) continue;
            for (int cambios = 0; cambios < direcciones[direccion]; cambios++){
                pos_adyacente[0] += vecX;
                pos_adyacente[1] += vecY;
                partida->tablero[pos_adyacente[1]][pos_adyacente[0]] = partida->colorActual;
            }
            pos_adyacente[0] = jugada[0];
            pos_adyacente[1] = jugada[1];
            direccion++;
        }
    }
    
}

// -------------------------------------------------------

/* valida si una casilla se encuentra en el tablero
    args:
        jugada: lista de 2 posiciones con el (x, y) de la jugada
    returns:
        TRUE o FALSE dependiendo de la validez */
int casillaEnRango(int* jugada){
    if (jugada[0]>7 || jugada[0]<0 || jugada[1]>7 || jugada[1]<0){
        return FALSE;
    }
    return TRUE;
}

// -------------------------------------------------------

/* traduce una jugada en formato 'Columna-Fila' al formato '(x, y)'
    args:
        jugada: texto en formato 'Columna-Fila'
        jugadaTraducida: lista de 2 posiciones con el (x, y) de la jugada */
void traducirJugada(char* jugada, int* jugadaTraducida){
    jugadaTraducida[0] = (int)(toupper(jugada[0]))-'A';
    jugadaTraducida[1] = (int)((jugada[1])-'0')-1;
}

// -------------------------------------------------------

/* determina si hay al menos 1 movimiento disponible
    args:
        partida: estructura en la que almaceno la informacion de la partida
    returns:
        jugada_valida: TRUE o FALSE dependiendo de si hay o no movimientos disponibles */
int movimientoDisponible(info_tablero* partida){
    int jugada[2], jugada_valida=FALSE, direcciones[8];

    for(int fila=0; fila<CANT_FILAS && !jugada_valida; fila++){
        for(int columna=0; columna<CANT_COLUMNAS && !jugada_valida; columna++){
            if (partida->tablero[fila][columna]=='X'){
                jugada[0] = columna;
                jugada[1] = fila;
                jugada_valida = validarJugada(partida, jugada, direcciones);
            }
        }
    }

    return jugada_valida;
}

// -------------------------------------------------------

/* imprime el tablero en pantalla
    args:
        partida: estructura en la que almaceno la informacion de la partida */
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

/* imprime la partida en un archivo
    args:
        partida: estructura en la que almaceno la informacion de la partida
        destino: path al archivo */
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

/* determina quién ganó e imprime el resultado
    args:
        partida: estructura en la que almaceno la informacion de la partida */
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

/* inicializa algunas variables de la estructura partida
    args:
        partida: estructura en la que almaceno la informacion de la partida*/
void initPartida(info_tablero* partida){
    int centroF, centroC;
    for(int fila=0; fila<CANT_FILAS; fila++){
        for(int columna=0; columna<CANT_COLUMNAS; columna++){
            partida->tablero[fila][columna] = 'X';
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

/* libera la memoria pedida cuando se lee el archivo con la partida
    args:
        partida: estructura en la que almaceno la informacion de la partida*/
void liberarPartida(info_tablero* partida){
    free(partida->jugadorBlanco);
    free(partida->jugadorNegro);
}