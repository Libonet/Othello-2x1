import sys
from random import choice

def clear():
    print("\033[H\033[J", end='')

def main():
    if len(sys.argv)==4:
        jugar_Othello(sys.argv[1], sys.argv[2], sys.argv[3])
    else:
        print("Error!! cantidad de argumentos erronea. Ingrese origen, color, y nivel")

def jugar_Othello(origen: str, color: str, nivel: str):
    tablero, color_actual = importar_partida(origen)
    if not (color=="B" or color=="N") or not (color_actual=="B" or color_actual=="N"):
        print("Error!! colores invalidos")
        return
    
    comenzar_partida(tablero, color_actual, color, nivel)

def obtener_frontera(tablero: list[list[str]], color: str)->set[tuple[int, int]]:
    frontera: set[tuple[int, int]] = set()
    for pos_x, fila in enumerate(tablero):
        for pos_y, casilla in enumerate(fila):
            if casilla!="X":
                continue

            encontrado=False
            vec_y = -1
            while vec_y<2 and not encontrado:
                vec_x = -1
                while vec_x<2 and not encontrado:
                    if vec_x==0 and vec_y==0:
                        vec_x += 1
                        continue

                    ady_x = pos_x + vec_x
                    ady_y = pos_y + vec_y
                    if not casilla_en_rango((ady_x, ady_y)):
                        vec_x += 1
                        continue

                    if tablero[ady_x][ady_y]=="X":
                        vec_x += 1
                        continue

                    if tablero[ady_x][ady_y]==color:
                        encontrado=True
                        frontera.add((pos_x, pos_y))
                    
                    vec_x += 1
                vec_y+=1

    return frontera
                
def comenzar_partida(tablero: list[list[str]], color_actual: str, color_de_jugador: str, nivel: str):
    # conjunto de casillas adyacentes a las negras
    frontera_negra = obtener_frontera(tablero, "N")
    # conjunto de casillas adyacentes a las blancas
    frontera_blanca = obtener_frontera(tablero, "B")
    
    fronteras = (frontera_blanca, frontera_negra)
    terminada = False
    turno_previo_saltado = False
    saltar_turno = False
    
    while not terminada:
        jugadas_validas = buscar_jugadas(tablero, color_actual, fronteras)
        casillas_validas = [casilla[0] for casilla in jugadas_validas]

        if color_actual==color_de_jugador:
            saltar_turno = turno_jugador(tablero, color_actual, jugadas_validas, casillas_validas, fronteras)
        else:
            print("Turno de maquina:")
            saltar_turno = turno_maquina(tablero, color_actual, nivel, jugadas_validas, casillas_validas, fronteras)
        
        if saltar_turno:
            if turno_previo_saltado:
                terminada = True
            else:
                turno_previo_saltado = True
        else:
            turno_previo_saltado = False
            
        color_actual = "B" if color_actual=="N" else "N"
        
    mostrar_tablero(tablero, [])
    terminar_partida(tablero, color_de_jugador)

def mostrar_tablero(tablero: list[list[str]], casillas_validas: list[tuple[int, int]]):
    print("Tablero:")
    print("   | A | B | C | D | E | F | G | H |")
    print("---|---|---|---|---|---|---|---|---|")
    for indice, fila in enumerate(tablero):
        print(f" {indice+1} |", end="")
        for columna, letra in enumerate(fila):
            if letra!="X": print(f" {letra} |", end="")
            else:
                if (indice, columna) in casillas_validas: print(f" O |", end="")
                else: print(f"   |", end="")
        print("")

def terminar_partida(tablero: list[list[str]], color: str):
    clear()
    determinar_ganador(tablero)
    mostrar_tablero(tablero, [])

def determinar_ganador(tablero: list[list[str]]):
    cant_blancas = 0
    cant_negras = 0
    for fila in tablero:
        for casilla in fila:
            if casilla=="B":
                cant_blancas += 1
            elif casilla=="N":
                cant_negras += 1

    if cant_blancas > cant_negras:
        print(f"Ganó el jugador con las fichas blancas ({cant_blancas}-{cant_negras})")
    elif cant_negras > cant_blancas:
        print(f"Ganó el jugador con las fichas negras ({cant_blancas}-{cant_negras})")
    else:
        print(f"Empataron... ({cant_blancas}-{cant_negras})")

def traducir_jugada_inversa(jugada: tuple[int, int]):
   return chr(jugada[1]+ord("A")) + str(jugada[0]+1)

def turno_jugador(tablero: list[list[str]], color: str, jugadas_validas, \
    casillas_validas, fronteras: tuple[set[tuple[int, int]], set[tuple[int, int]]])->bool:

    casillas_validas_legibles = sorted(list(map(traducir_jugada_inversa, casillas_validas)))
    clear()
    mostrar_tablero(tablero, casillas_validas)
    print(f"jugadas validas: {casillas_validas_legibles}")
    
    jugada = recibir_jugada()
    if len(jugadas_validas)!=0:
        valida = False
        while not valida:
            for casilla, direcciones in jugadas_validas:
                if jugada==casilla:
                    valida = True
                    realizar_jugada(tablero, color, casilla, direcciones, fronteras)
            if valida==False: 
                clear()
                print("Jugada invalida...")
                mostrar_tablero(tablero, casillas_validas)
                print(f"jugadas validas: {casillas_validas_legibles}")
                jugada = recibir_jugada()
        return False
    else:
        while jugada!=(10, 10):
            clear()
            print("Jugada invalida...")
            mostrar_tablero(tablero, casillas_validas)
            print(f"jugadas validas: {casillas_validas_legibles}")
            jugada = recibir_jugada()
        return True

def actualizar_fronteras(tablero: list[list[str]], color_jugador: str, casilla: tuple[int, int], \
    fronteras: tuple[set[tuple[int, int]], set[tuple[int, int]]]):

    frontera_blanca, frontera_negra = fronteras
    f_aumentada, f_disminuida = (frontera_blanca, frontera_negra) if color_jugador=="B" else (frontera_negra, frontera_blanca)

    if casilla in f_disminuida: f_disminuida.remove(casilla)
    pos_x, pos_y = casilla
    color_anterior = "B" if color_jugador=="N" else "N"
    # la casilla cambio de color, asi que checkeamos que
    # sus adyacentes sigan perteneciendo a la frontera del color anterior
    # y agregamos estos adyacentes a la frontera del color actual
    for vec_y in range(-1, 2):
        for vec_x in range(-1, 2):
            if vec_x==0 and vec_y==0:
                continue
            
            ady_x = pos_x + vec_x
            ady_y = pos_y + vec_y
            if (not casilla_en_rango((ady_x, ady_y))):
                continue

            color_adyacente = tablero[ady_x][ady_y]
            if color_adyacente=="X":
                f_aumentada.add((ady_x, ady_y))

                # esta casilla tiene de adyacente a una del color anterior?
                # si no tiene, la eliminamos de la frontera del otro color
                encontrada=False
                vec2_y = -1
                while vec2_y < 2 and not encontrada:
                    vec2_x = -1
                    while vec2_x < 2 and not encontrada:
                        if vec_x==0 and vec_y==0:
                            continue

                        ady2_x = ady_x + vec2_x
                        ady2_y = ady_y + vec2_y
                        if (not casilla_en_rango((ady_x, ady_y))):
                            continue
            
                        color_adyacente = tablero[ady_x][ady_y]
                        if color_adyacente==color_anterior:
                            encontrada=True

                        vec2_x += 1
                    vec2_y += 1

                if not encontrada:
                    if (ady_x, ady_y) in f_disminuida: f_disminuida.remove((ady_x, ady_y))


def turno_maquina(tablero: list[list[str]], color: str, nivel: str, jugadas_validas: list[tuple[tuple[int, int], list[tuple[int, int]]]],\
    casillas_validas: list[tuple[int, int]], fronteras: tuple[set[tuple[int, int]], set[tuple[int, int]]])->bool:

    if len(jugadas_validas)==0:
        print("jugada de la maquina: ninguna, turno pasado")
        return True

    if nivel=="0":
        jugada, direcciones = choice(jugadas_validas)
        clear()
        mostrar_tablero(tablero, casillas_validas)
        print(f"jugada de la maquina: {traducir_jugada_inversa(jugada)}")
        realizar_jugada(tablero, color, jugada, direcciones, fronteras)
        input("Presione enter para continuar")
    elif nivel=="1":
        pass

    return False

def realizar_jugada(tablero: list[list[str]], color: str, jugada: tuple[int, int], direcciones: list[tuple[int, int]],\
    fronteras: tuple[set[tuple[int, int]], set[tuple[int, int]]]):
    pos_x, pos_y = jugada
    tablero[pos_x][pos_y] = color
    actualizar_fronteras(tablero, color, jugada, fronteras)

    for vec_x, vec_y in direcciones:
        ady_x = pos_x + vec_x
        ady_y = pos_y + vec_y

        while tablero[ady_x][ady_y]!=color:
            tablero[ady_x][ady_y] = color
            actualizar_fronteras(tablero, color, (ady_x, ady_y), fronteras)
            ady_x += vec_x
            ady_y += vec_y

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

def buscar_jugadas(tablero: list[list[str]], color: str, \
    fronteras: tuple[set[tuple[int, int]], set[tuple[int, int]]])->list[tuple[tuple[int, int], list[tuple[int, int]]]]:

    frontera_blanca, frontera_negra = fronteras
    jugadas_validas: list[tuple[tuple[int, int], list[tuple[int, int]]]] = []

    frontera = frontera_blanca if color=="N" else frontera_negra

    for casilla in frontera:
            valida, direcciones = jugada_valida(tablero, color, casilla)
            if valida:
                jugadas_validas.append((casilla, direcciones))

    return jugadas_validas

def jugada_valida(tablero: list[list[str]], color: str, casilla: tuple[int, int])->tuple[bool, list[tuple[int, int]]]:
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

def direccion_valida(tablero: list[list[str]], color: str, casilla: tuple[int, int], direccion: tuple[int, int]):
    pos_x, pos_y = casilla
    vec_x, vec_y = direccion

    ady_x = pos_x + vec_x
    ady_y = pos_y + vec_y
    while casilla_en_rango((ady_x, ady_y)) and tablero[ady_x][ady_y]!=color and tablero[ady_x][ady_y]!="X":
        ady_x += vec_x
        ady_y += vec_y

    if not casilla_en_rango((ady_x, ady_y)) or tablero[ady_x][ady_y]=="X":
        return False
    return True

def importar_partida(origen: str) -> tuple[list[list[str]], str]:
    tablero: list[list[str]] = [[], [], [], [], [], [], [], []]
    color_actual: str = ""
    char = ' '
    fila = 0
    try:
        with open(origen, 'r') as archivo:
            for linea in archivo.readlines():
                if fila!=8:
                    for char in linea:
                        if char!='\n': tablero[fila].append(char)
                    fila+=1
                else:
                    color_actual = linea[0]
    except OSError:
        print("Error!! no se pudo abrir el archivo")  

    return tablero, color_actual

if __name__ == "__main__":
    main()