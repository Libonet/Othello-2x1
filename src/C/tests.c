#include "tests.h"

int main(){
    test_analizarPartida();

    test_procesarArchivo();

    test_leerNombre();

    test_colorInicial();

    test_validarJugada();

    test_validarLinea();

    test_casillaEnRango();

    test_movimientoDisponible();

    printf("\033[H\033[J");
    printf("Todos los tests correctos\n");
    return 0;
}

void test_analizarPartida(){
    // tests de analizarPartida
    assert(analizarPartida("./recursos/prueba.txt", "./recursos/tablero.txt")==0);
    assert(analizarPartida("./recursos/prueba2.txt", "./recursos/tablero.txt")==-1);
}

void test_procesarArchivo(){
    info_tablero partida;

    // tests de procesarArchivo
    initPartida(&partida);
    assert(procesarArchivo("./recursos/prueba.txt", &partida)==0);
    liberarPartida(&partida);
    initPartida(&partida);
    assert(procesarArchivo("./recursos/prueba2.txt", &partida)==-1);
    liberarPartida(&partida);
}

void test_leerNombre(){
    info_tablero partida;
    FILE* fp;

    // tests de leerNombre
    initPartida(&partida);
    
    fp = fopen("./recursos/prueba.txt", "r");
    assert(leerNombre(fp, &partida)==TRUE);
    fclose(fp);
    liberarPartida(&partida);
}

void test_colorInicial(){
    info_tablero partida;
    FILE* fp;

    // tests de colorInicial
    initPartida(&partida);

    fp = fopen("./recursos/prueba.txt", "r");
    for (char i; (i=fgetc(fp)!='\n');); // salteo la linea 1
    for (char i; (i=fgetc(fp)!='\n');); // y la linea 2
    assert(colorInicial(fp, &partida)==TRUE);
    fclose(fp);
}

void test_validarJugada(){
    info_tablero partida;
    int jugada[2], direcciones[8];

    // tests de validarJugada
    initPartida(&partida);
    partida.jugadorBlanco = "Santiago";
    partida.jugadorNegro = "Pepe";
    partida.colorActual = 'B';

    traducirJugada("C5", jugada);
    assert(validarJugada(&partida, jugada, direcciones)==TRUE);
    traducirJugada("A1", jugada);
    assert(validarJugada(&partida, jugada, direcciones)==FALSE);
}

void test_validarLinea(){
    info_tablero partida;
    int jugada[2];

    // tests de validarLinea
    initPartida(&partida);
    partida.jugadorBlanco = "Santiago";
    partida.jugadorNegro = "Pepe";
    partida.colorActual = 'B';

    traducirJugada("C5", jugada);
    assert(validarLinea(&partida, jugada, 0, 1)==TRUE);
    assert(validarLinea(&partida, jugada, -1, 1)==FALSE);
}

void test_casillaEnRango(){
    int jugada[2];

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
}

void test_movimientoDisponible(){
    info_tablero partida;

    // tests de movimientoDisponible
    initPartida(&partida);
    partida.colorActual = 'B';
    assert(movimientoDisponible(&partida)==TRUE);

    initPartida(&partida);
    procesarArchivo("./recursos/terminada.txt", &partida);
    analizarJugadas(&partida, "./recursos/tablero.txt");
    assert(movimientoDisponible(&partida)==FALSE);
    liberarPartida(&partida);
}