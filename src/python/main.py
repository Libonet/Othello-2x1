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

def comenzar_partida(tablero: list[str], color_actual: str, color_de_jugador: str, nivel: str):
    # lista de casillas adyacentes a las negras
    frontera_negra = [
        traducir_jugada("D3"),
        traducir_jugada("E3"),
        traducir_jugada("F3"),
        traducir_jugada("F4"),
        traducir_jugada("F5"),
        traducir_jugada("C4"),
        traducir_jugada("C5"),
        traducir_jugada("C6"),
        traducir_jugada("D6"),
        traducir_jugada("E6")]
    # lista de casillas adyacentes a las blancas
    frontera_blanca = [ 
        traducir_jugada("E3"),
        traducir_jugada("D3"),
        traducir_jugada("C3"),
        traducir_jugada("C4"),
        traducir_jugada("C5"),
        traducir_jugada("F4"),
        traducir_jugada("F5"),
        traducir_jugada("F6"),
        traducir_jugada("E6"),
        traducir_jugada("D6")]
    fronteras = (frontera_blanca, frontera_negra)
    terminada = False
    turno_previo_saltado = False
    saltar_turno = False
    while not terminada:
        jugadas_validas = buscar_jugadas(tablero, color_actual, fronteras)
        if color_actual==color_de_jugador:
            saltar_turno = turno_jugador(tablero, color_actual, jugadas_validas)
        else:
            saltar_turno = turno_maquina(tablero, color_actual, nivel, jugadas_validas)
        if saltar_turno:
            if turno_previo_saltado:
                terminada = True
            else:
                turno_previo_saltado = True
        color_actual = "B" if color_actual=="N" else "N"
    
    terminar_partida(tablero, color_de_jugador)
        
def terminar_partida(tablero: list[str], color: str):
    pass

def turno_jugador(tablero: list[str], color: str, jugadas_validas: list[tuple[tuple[int, int], list[tuple[int, int]]]])->bool:
    jugada = recibir_jugada()
    
    if len(jugadas_validas)!=0:
        valida = False
        while not valida:
            for casilla, direcciones in jugadas_validas:
                if jugada==casilla:
                    valida = True
                    realizar_jugada(tablero, casilla, direcciones)
        return False
    else:
        while jugada!=(10, 10):
            clear()
            print("Jugada invalida...")
            jugada = recibir_jugada()
        return True

def turno_maquina(tablero: list[str], color: str, nivel: str, jugadas_validas: list[tuple[tuple[int, int], list[tuple[int, int]]]]):
    pass

def realizar_jugada(tablero: list[str], jugada: tuple[int, int], direcciones: list[tuple[int, int]]):
    pass

def recibir_jugada()->tuple[int, int]:
    ingreso = input("ingrese su jugada: ")
    if ingreso=='':
        return (10, 10) # codigo de saltear turno
    if len(ingreso)!=2:
        return (-1, -1) # error
    # ejemplo: "A1" = (0, 0), "F5" = (4, 5)
    jugada = traducir_jugada(ingreso)

    return jugada

def traducir_jugada(ingreso: str):
    return (int(ingreso[1])-1, ord(ingreso[0])-ord('A'))

def buscar_jugadas(tablero: list[str], color: str, \
    fronteras: tuple[list[tuple[int, int]], list[tuple[int, int]]])->list[tuple[tuple[int, int], list[tuple[int, int]]]]:
    frontera_blanca, frontera_negra = fronteras
    jugadas_validas: list[tuple[tuple[int, int], list[tuple[int, int]]]] = []

    if color=="B":
        for casilla in frontera_blanca:
            valida, direcciones = jugada_valida(tablero, color, casilla)
            if valida:
                jugadas_validas.append((casilla, direcciones))
    else:
        for casilla in frontera_negra:
            valida, direcciones = jugada_valida(tablero, color, casilla)
            if valida:
                jugadas_validas.append((casilla, direcciones))

    return jugadas_validas

def jugada_valida(tablero: list[str], color: str, casilla: tuple[int, int])->tuple[bool, list[tuple[int, int]]]:
    fila_jugada, columna_jugada = casilla
    valida = False
    direcciones: list[tuple[int, int]] = []
    for fila in range(-1, 2): # recorro las distintas direcciones
        for columna in range(-1, 2):
            if fila==0 and columna==0: continue
            fila_ady = fila_jugada+fila
            columna_ady = columna_jugada+columna

            if not casilla_en_rango((fila_ady, columna_ady)):
                continue
            
            color_adyacente = tablero[fila_ady][columna_ady]
            if color_adyacente==color or color_adyacente=="X":
                continue

            if direccion_valida(tablero, color, casilla, (fila, columna)):
                valida=True
                direcciones.append((fila, columna))

    return (valida, direcciones)

def casilla_en_rango(casilla: tuple[int, int])->bool:
    x, y = casilla
    if x>7 or x<0 or y>7 or y<0:
        return False
    return True

def direccion_valida(tablero: list[str], color: str, casilla: tuple[int, int], direccion: tuple[int, int]):
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