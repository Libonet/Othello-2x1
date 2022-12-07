/*Esto pregunta si está definida la librería, en caso de no
estarlo la define*/
#ifndef __FUNCIONES_H__
#define __FUNCIONES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CANT_FILAS 8
#define CANT_COLUMNAS 8
#define TRUE 1
#define FALSE 0

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

#endif /* __FUNCIONES_H__ */