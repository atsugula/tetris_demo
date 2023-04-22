
#include "miniwin.h"
#include <cstdlib>
#include <sstream>
#include <string>
#include <ctime>

using namespace std;
using namespace miniwin;

const int TAM = 25; // Constante programa

const int FILAS = 20;
const int COLUMNAS = 10;
const int TIEMPO = 30;
const int MARGEN = 20;

typedef int Tablero[COLUMNAS][FILAS];

struct Coord {
    int x, y;
};

struct Pieza {
    Coord orig;     // Bloque central (Posicion absoluta)
    Coord perif[3]; // Bloque perifericos (Posicion relativa, los otros tres bloques)
    int color;      // Bloque color

    Coord posicion(int n) const; // n entre 0 y 3 (0 = central, 1-3, perifericos)
};

const Coord perifs[7][3] = {
    { { 1,0 }, { 0,1 }, { 1,1 } }, // Cuadrado
    { { 1,0 }, {-1,1 }, { 0,1 } }, // La pieza Z
    { { 0,1 }, { 1,1 }, {-1,0 } }, // La pieza Z - invertida
    { { 0,1 }, { 0,-1}, { 1,1 } }, // La pieza L
    { { 0,1 }, { 0,-1}, {-1,1 } }, // La pieza L - invertida
    { {-1,0 }, { 1,0 }, { 0,1 } }, // La rara esa que parece un pene T
    { { 0,1 }, { 0,-1}, { 0,2 } }, // La pieza l o palo
};

Coord Pieza::posicion(int n) const {
    Coord ret = { orig.x, orig.y };
    if (n != 0) {
        ret.x += perif[n-1].x; // a += 1 === a = a + 1
        ret.y += perif[n-1].y;
    }
    return ret;
}

void cuadrado(int x, int y){
    rectangulo_lleno(MARGEN + 1 + x * TAM,
                     MARGEN + 1 + y * TAM,
                     MARGEN + x * TAM + TAM,
                     MARGEN + y * TAM + TAM);
}

void pinta_pieza(const Pieza& P) {
    color(P.color);
    for (int i = 0; i < 4; i++) {
        Coord c = P.posicion(i);
        cuadrado(c.x, c.y);
    }
}

Coord rota_derecha(Coord& c) {
    Coord ret = { -c.y, c.x };
    return ret;
}

void rota_derecha(Pieza& P) {
    for (int i = 0; i < 3; i++) {
        P.perif[i] = rota_derecha(P.perif[i]);
    }
}

Coord rota_izquierda(Coord& c) {
    Coord ret = { c.y, -c.x };
    return ret;
}

void rota_izquierda(Pieza& P) {
    for (int i = 0; i < 3; i++) {
        P.perif[i] = rota_izquierda(P.perif[i]);
    }
}

void tablero_vacia(Tablero& T) {
    for (int i = 0; i < COLUMNAS; i++) {
        for (int j = 0; j < FILAS; j++) {
            T[i][j] = NEGRO; // Esto significa casilla vacia
        }
    }
}

void tablero_pinta(Tablero& T) {
    for (int i = 0; i < COLUMNAS; i++) {
        for (int j = 0; j < FILAS; j++) {
            color(T[i][j]);
            cuadrado(i, j);
        }
    }
}

void tablero_incrusta_pieza(Tablero& T, const Pieza& P) {
    for (int i = 0; i <4; i++) {
        Coord c = P.posicion(i);
        T[c.x][c.y] = P.color;
    }
}

bool tablero_colision(const Tablero& T, const Pieza& P) {
    for (int i = 0; i <4; i++) {
        Coord c = P.posicion(i);
        // Comprobar limites
        if (c.x < 0 || c.x >= COLUMNAS)
            return true;
        if (c.y < 0 || c.y >= FILAS)
            return true;
        // Mirar resto de fichas
        if (T[c.x][c.y] != NEGRO)
            return true;
    }
    return false;
}

void pieza_nueva(Pieza& P){
    P.orig.x = 12;
    P.orig.y = 2;
    P.color = 1 + (rand() % 6); // 1 ----> RAND-MAX + 1
    // Pieza al azar
    int r = (rand() % 7); // Rango aleatorio para la pieza
    for (int i = 0; i <3; i++) {
        P.perif[i] = perifs[r][i];
    }
}

bool tablero_fila_llena(const Tablero& T, int fila) {
    for (int i = 0; i < COLUMNAS; i++) {
        if (T[i][fila] == NEGRO) return false;
    }
    return true;
}

void tablero_colapsa(Tablero& T, int fila) {
    // Copiar de abajo a arriba
    for (int j = fila; j > 0; j--) {
        for (int i = 0; i < COLUMNAS; i++) {
            T[i][j] = T[i][j-1];
        }
    }
    // Vaciar la de arriba
    for (int i = 0; i < COLUMNAS; i++) {
        T[i][0] = NEGRO;
    }
}

int tablero_cuenta_lineas(Tablero& T) {
    int fila = FILAS - 1, cont = 0;
    while (fila >= 0) {
        if (tablero_fila_llena(T, fila)) {
            tablero_colapsa(T, fila);
            cont++;
        } else {
            fila--;
        }
    }
    return cont;
}

string a_string(int puntos) {
    stringstream sout;
    sout << puntos;
    return sout.str();
}

const int puntos_limite[10] = {
    30, 60, 90, 110, 220,
    230, 250, 270, 300, 500
};

const int tics_nivel[10] = {
    33, 25, 22, 20, 18,
    16, 14, 10, 8, 2
};

void repinta(Tablero& T, const Pieza& P, const Pieza& Sigue, int puntos, int nivel) {
    const int ancho = TAM * COLUMNAS;
    const int alto = TAM * FILAS;
    borra();
    tablero_pinta(T);
    // Marcamos lineas del juego
    color_rgb(128, 128, 128); // GRIS
    linea(MARGEN, MARGEN, MARGEN, MARGEN + (alto));
    linea(MARGEN, MARGEN + (alto), MARGEN + (ancho), MARGEN + (alto));
    linea(MARGEN + (ancho), MARGEN + (alto), MARGEN + (ancho), MARGEN);
    // Texto para los menu
    texto(40 + ancho, MARGEN, "Siguiente Pieza");
    texto(40 + ancho, MARGEN + 150, "Nivel");
    texto(40 + ancho, MARGEN + 250, "Puntos");
    texto(ancho - 100, MARGEN + 530, "ATSU 2022 © All Right Reserved.");
    // Parametros para el menu
    color(BLANCO);
    texto(40 + ancho, 300, a_string(puntos));
    texto(40 + ancho, 190, a_string(nivel + 1));
    //Pintamos las piezas
    pinta_pieza(P);
    pinta_pieza(Sigue);
    refresca();
}

void game_over() {
    color(BLANCO);
//  rectangulo();
    texto(140, 240, "GAME OVER!");
    refresca();
    espera(2000);
    vcierra();
}

int main() {
    vredimensiona(TAM * COLUMNAS + 220, TAM * FILAS + 100); // Tamaño de la GUI
    srand(time(0)); // Poner semilla para los numeros al azar

    int tic = 0, puntos = 0, nivel = 0;

    // Tablero
    Tablero T;
    tablero_vacia(T);

    // Pieza demo
    Pieza c, sigue;
    pieza_nueva(c);
    // Siguiente pieza
    pieza_nueva(sigue);
    c.orig.x = 5;

    repinta(T, c, sigue, puntos, nivel);

    // Movimientos de la ficha
    int t = tecla();
    while (t != ESCAPE) {
        // 0. Copiar la pieza actual
        Pieza copia = c;
        // Simular tecla de abajo, para que corra solo
        if (t == NINGUNA && tic > tics_nivel[nivel]) {
            tic = 0;
            t = ABAJO;
        }

        // 1. Mover "En el tablero"
        if (t == ARRIBA)            rota_derecha(c);
        else if (t == ABAJO)        c.orig.y++;
        else if (t == DERECHA)      c.orig.x++;
        else if (t == IZQUIERDA)    c.orig.x--;

        if(t == int('N')) nivel++;

        // 2. Mirar si hay colisicion
        if (tablero_colision(T, c)) {
            c = copia;
            if (t == ABAJO) {
                tablero_incrusta_pieza(T, c);
                int cont = tablero_cuenta_lineas(T);
                puntos += cont;
                if (puntos > puntos_limite[nivel]) {
                    nivel++;
                }
                c = sigue;
                pieza_nueva(sigue);
                c.orig.x = 5;
                if (tablero_colision(T, c)) {
                    game_over();
                }
            }
        }
        // Repintar
        if (t != NINGUNA){
            repinta(T, c, sigue, puntos, nivel);
        }
        espera(30);
        tic++;
        t = tecla();
    }
    vcierra();
    return 0;
}
