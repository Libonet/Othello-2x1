from ..main import *

# lo testeo primero porque limpia la consola
def test_turno_maquina():
    tablero, color = (
        [['N', 'X', 'X', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'N', 'N', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'X', 'N', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'X', 'N', 'N', 'N', 'N', 'N', 'X'],
         ['X', 'X', 'X', 'N', 'N', 'B', 'X', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'N', 'X', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'B', 'N', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'X', 'X', 'X']],
         "B"
    )
    frontera_blanca = obtener_frontera(tablero, "B")
    frontera_negra = obtener_frontera(tablero, "N")
    fronteras = (frontera_blanca, frontera_negra)
    # el jugador juega C5
    realizar_jugada(tablero, "B", (2, 4), [0, 0, 0, 0, 2, 0, 0, 0], fronteras)

    jugadas_validas = buscar_jugadas(tablero, "N", fronteras)
    casillas_validas = [casilla[0] for casilla in jugadas_validas]

    assert turno_maquina(tablero, "N", "1", jugadas_validas, casillas_validas, fronteras)==False
    assert turno_maquina(tablero, "N", "1", [], [], fronteras)==True

def test_importar_partida():
    assert importar_partida("../C/tablero_ejemplo.txt")==(
        [['N', 'X', 'X', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'N', 'N', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'X', 'N', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'X', 'N', 'N', 'N', 'N', 'N', 'X'],
         ['X', 'X', 'X', 'N', 'N', 'B', 'X', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'N', 'X', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'B', 'N', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'X', 'X', 'X']],
         "B"
    )

def test_obtener_frontera():
    tablero, color = (
        [['N', 'X', 'X', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'N', 'N', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'X', 'N', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'X', 'N', 'N', 'N', 'N', 'N', 'X'],
         ['X', 'X', 'X', 'N', 'N', 'B', 'X', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'N', 'X', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'B', 'N', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'X', 'X', 'X']],
         "B"
    )
    assert obtener_frontera(tablero, "N")=={
        (3, 1), (4, 6), (5, 7), (0, 2), (1, 0), (2, 5), (1, 3), (7, 4),
        (6, 2), (7, 7), (6, 5), (4, 2), (3, 0), (4, 5), (0, 1), (2, 4),
        (1, 2), (6, 4), (7, 3), (6, 7), (7, 6), (3, 2), (3, 5), (5, 2),
        (2, 0), (1, 4), (7, 2), (7, 5)}
    assert obtener_frontera(tablero, "B")=={
        (6, 5), (4, 6), (6, 4), (5, 7), (6, 7), (4, 5), (4, 7)
    }

def test_buscar_jugadas():
    tablero, color = (
        [['N', 'X', 'X', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'N', 'N', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'X', 'N', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'X', 'N', 'N', 'N', 'N', 'N', 'X'],
         ['X', 'X', 'X', 'N', 'N', 'B', 'X', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'N', 'X', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'B', 'N', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'X', 'X', 'X']],
         "B"
    )
    frontera_blanca = obtener_frontera(tablero, "B")
    frontera_negra = obtener_frontera(tablero, "N")
    fronteras = (frontera_blanca, frontera_negra)
    assert buscar_jugadas(tablero, color, fronteras)==[
        ((2, 4), [0, 0, 0, 0, 2, 0, 0, 0]),
        ((7, 6), [0, 0, 0, 1, 0, 0, 0, 0]),
        ((3, 2), [0, 0, 0, 0, 0, 0, 0, 1]), 
        ((5, 2), [0, 0, 0, 0, 0, 0, 1, 0]), 
        ((7, 2), [0, 0, 0, 0, 0, 1, 0, 0])]

def test_jugada_valida():
    tablero, color = (
        [['N', 'X', 'X', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'N', 'N', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'X', 'N', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'X', 'N', 'N', 'N', 'N', 'N', 'X'],
         ['X', 'X', 'X', 'N', 'N', 'B', 'X', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'N', 'X', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'B', 'N', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'X', 'X', 'X']],
         "B"
    )

    assert jugada_valida(tablero, color, (2, 4))==(True, [0, 0, 0, 0, 2, 0, 0, 0])
    assert jugada_valida(tablero, color, (0, 0))==(False, [0, 0, 0, 0, 0, 0, 0, 0])
    assert jugada_valida(tablero, color, (3, 2))==(True, [0, 0, 0, 0, 0, 0, 0, 1])

def test_direccion_valida():
    tablero, color = (
        [['N', 'X', 'X', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'N', 'N', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'X', 'N', 'X', 'X', 'X', 'X', 'X'],
         ['X', 'X', 'N', 'N', 'N', 'N', 'N', 'X'],
         ['X', 'X', 'X', 'N', 'N', 'B', 'X', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'N', 'X', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'B', 'N', 'X'],
         ['X', 'X', 'X', 'X', 'X', 'X', 'X', 'X']],
         "B"
    )
    
    assert direccion_valida(tablero, color, (2, 4), (1, 0))==2
    assert direccion_valida(tablero, color, (2, 4), (-1, -1))==0

def test_casilla_en_rango():
    assert casilla_en_rango((0, 0))==True
    assert casilla_en_rango((7, 7))==True
    assert casilla_en_rango((-1, -1))==False
    assert casilla_en_rango((8, 8))==False

def test_traducir_jugada():
    assert traducir_jugada("C5")==(2, 4)
    assert traducir_jugada("A1")==(0, 0)
    assert traducir_jugada("H4")==(7, 3)
    assert traducir_jugada("D8")==(3, 7)

def test_traducir_jugada_inversa():
    assert traducir_jugada_inversa((2, 4))=="C5"
    assert traducir_jugada_inversa((0, 0))=="A1"
    assert traducir_jugada_inversa((7, 3))=="H4"
    assert traducir_jugada_inversa((3, 7))=="D8"

def test_traducir_direccion():
    assert traducir_direccion(0)==(-1, -1)
    assert traducir_direccion(3)==(-1, 0)
    assert traducir_direccion(5)==(-1, 1)
    assert traducir_direccion(7)==(1, 1)