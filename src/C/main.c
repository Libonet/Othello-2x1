#include "funciones.h"

int main(int argc, char* argv[]){
    if (argc >= 3){
        return analizarPartida(argv[1], argv[2]);
    }
    // printf("Error!! no se ingreso un archivo\n");
    return -1; // Error
}