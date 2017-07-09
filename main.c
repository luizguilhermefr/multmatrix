/*
 * multmatrix.c
 * 
 * Copyright 2017 Edmar André Bellorini, Elixandre Baldi e Luiz Rosa
 * <edmar.bellorini@gmail.com>, <elixandre_michael@hotmail.com>, <luizguilhermefr@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

int main (int argc, char **argv);

int print_argumentos_invalidos ();

double **gera_matriz (int l, int c);

void transpoe_matriz (double **M, int l, int c);

double **multiplica_matrizes (double **M1, double **M2, int l, int c, bool otimizar);

void print_matriz (double **M1, int l, int c, char *msg);

/**
 * Ponto de entrada da aplicação. Uso esperado:
 * ./multmatrix l c o|t
 * Onde:
 * l => número de linhas
 * c => número de colunas
 * o|t => o = matriz M2 original, t = matriz M2 transposta
 * --verbose => escrever matrizes na tela (padrao: desativado)
 * @param int argc Número de argumentos + 1
 * @param char** argv Nome do programa + argumentos
 */
int main (int argc, char **argv) {
    int linhas, colunas;
    bool otimizar, verbose = FALSE;
    double **M1, **M2, **Mf;

    if (argc < 4) {
        return print_argumentos_invalidos();
    }

    linhas = atoi(argv[1]);
    colunas = atoi(argv[2]);

    if ((linhas == 0) || (colunas == 0)) {
        return print_argumentos_invalidos();
    }

    if (strcmp(argv[3], "o") == 0) {
        otimizar = FALSE;
    } else if (strcmp(argv[3], "t") == 0) {
        otimizar = TRUE;
    } else {
        return print_argumentos_invalidos();
    }

    if (argc > 4) {
        if (strcmp(argv[4], "--verbose") == 0) {
            verbose = TRUE;
        }
    }

    M1 = gera_matriz(linhas, colunas);
    if (verbose) print_matriz(M1, linhas, colunas, "M1:");
    M2 = gera_matriz(linhas, colunas);
    if (verbose) print_matriz(M2, linhas, colunas, "M2:");

    Mf = multiplica_matrizes(M1, M2, linhas, colunas, otimizar);
    if (verbose) print_matriz(Mf, linhas, colunas, "Mf: ");

    return 0;
}

/**
 * Escreve o erro de argumentos inválidos.
 * @return codigo de erro
 */
int print_argumentos_invalidos () {
    printf("Argumentos inválidos.\nUso: ./multmatrix l c o|t [--verbose]\nOnde:\nl: número de linhas\nc: número de colunas\no|t: matriz M2 original (o) ou transposta (t)\n--verbose: escrever matrizes durante o processo (desativado por padrão)\n");
    return 1;
}

/**
 * Escreve uma mensagem, acrescida da matriz na tela.
 * @param M matriz de entrada
 * @param l quantia de linhas
 * @param c quantia de colunas
 * @param msg mensagem
 */
void print_matriz (double **M, int l, int c, char *msg) {
    int i, j;
    printf("%s\n", msg);
    for (i = 0; i < l; i++) {
        for (j = 0; j < c; j++) {
            printf("%.2lf\t", M[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * Aloca uma matriz e retorna seu ponteiro.
 * @param l quantidade de linhas
 * @param c quantidade de colunas
 * @return matriz alocada aleatoriamente
 */
double **gera_matriz (int l, int c) {
    int i, j;
    double **M;
    M = (double **) calloc(l, sizeof(double *));
    for (i = 0; i < l; i++) {
        M[i] = (double *) calloc(c, sizeof(double));
    }
    srand((unsigned) time(NULL));
    for (i = 0; i < l; i++) {
        for (j = 0; j < c; j++) {
            M[i][j] = (double) (rand() % 1000 + 1) / 100; //(0~100 double)
        }
    }

    return M;
}

/**
 * Transpoe uma matriz para otimizaçao do uso de cache.
 * @param M matriz
 * @param l quantidade de linhas
 * @param c quantidade de colunas
 */
void transpoe_matriz (double **M, int l, int c) {
    int i, j;
    double aux;
    for (i = 0; i < l; i++) {
        for (j = i + 1; j < c; j++) {
            aux = M[i][j];
            M[i][j] = M[j][i];
            M[j][i] = aux;
        }
    }

    return;
}

/**
 * Multiplica as matrizes.
 * @param M1 matriz da esquerda
 * @param M2 matriz da direita
 * @param l quantidade de linhas
 * @param c quantidade de colunas
 * @param otimizar otimizar a multiplicaçao por meio de transposiçao
 * @return matriz resultado
 */
double **multiplica_matrizes (double **M1, double **M2, int l, int c, bool otimizar) {
    int i, j, k;
    double temp = 0, **Mf;

    Mf = (double **) calloc(l, sizeof(double *));
    for (i = 0; i < l; i++) {
        Mf[i] = (double *) calloc(c, sizeof(double));
    }

    if (otimizar) {
        transpoe_matriz(M2, l, c);
    }

    for (i = 0; i < l; i++) {
        for (j = 0; j < c; j++) {
            temp = 0;
            for (k = 0; k < l; k++) {
                temp += M1[i][k] * M2[j][k];
            }
            Mf[i][j] = temp;
        }
    }

    return Mf;
}