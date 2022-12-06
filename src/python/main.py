import sys

def clear():
    print("\033[H\033[J", end='')

def main():
    if len(sys.argv)==4:
        jugar_Othello(sys.argv[1], sys.argv[2], sys.argv[3])
    else:
        print("Error!! cantidad de argumentos erronea. Ingrese origen y destino")

def jugar_Othello(origen: str, color: str, nivel: str):
    tablero, color_actual = importar_partida(origen)
    if not (color=="B" or color=="N") or not (color_actual=="B" or color_actual=="N"):
        return
    
    comenzar_partida(tablero, color_actual, color, nivel)

def comenzar_partida(tablero: list[str], color_actual: str, color: str, nivel: str):
    frontera_blanca = []
    frontera_negra = []
    fronteras = (frontera_blanca, frontera_negra)
    terminada = False
    turno_previo_saltado = False
    saltar_turno = False
    while not terminada:
        if color_actual==color:
            saltar_turno = turno_jugador(tablero, color)
        else:
            saltar_turno = turno_maquina(tablero, color_actual, nivel)
        if saltar_turno:
            if turno_previo_saltado:
                terminada = True
            else:
                turno_previo_saltado = True
        if color_actual=="B":
            color_actual="N"
        else:
            color_actual="B"
    
    terminar_partida(tablero, color)
        
def terminar_partida(tablero: list[str], color: str):
    pass

def turno_jugador(tablero: list[str], color: str, jugadas_validas: list[tuple[int, int]])->bool:
    jugada = recibir_jugada()
    while jugada not in jugadas_validas:
        clear()
        print("Jugada invalida...")
        jugada = recibir_jugada()
    realizar_jugada(tablero, jugada)

    if jugada==(10, 10):
        return True
    return False

def realizar_jugada(tablero: list[str], jugada: tuple[int, int]):
    pass

def recibir_jugada()->tuple[int, int]:
    ingreso = input("ingrese su jugada: ")
    if ingreso=='':
        return (10, 10) # codigo de saltear turno
    if len(ingreso)!=2:
        return (-1, -1) # error
    # ejemplo: "A1" = (0, 0), "F5" = (4, 5)
    jugada = (int(ingreso[1])-1, ord(ingreso[0])-ord('A'))

    return jugada

def turno_maquina(tablero: list[str], color: str, nivel: str):
    pass

def importar_partida(origen: str) -> tuple[list[str], str]:
    tablero: list[str] = []
    color_actual: str = ""
    try:
        with open(origen, 'r') as archivo:
            for linea in range(8):
                tablero[linea] = archivo.readline()
            color_actual = archivo.readline()
    except OSError:
        print("Error!! no se pudo abrir el archivo")  

    return tablero, color_actual

if __name__ == "__main__":
    main()