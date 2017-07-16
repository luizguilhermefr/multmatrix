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
#include <locale.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

int main (int argc, char **argv);

int print_argumentos_invalidos ();

double **gera_matriz (int l, int c);

double **transpoe_matriz (double **M, int l, int c);

double **multiplica_matrizes (double **M1, double **M2, int lm1cm2, int cm1lm2, bool otimizar);

void print_matriz (double **M, int l, int c, char *msg);

/**
 * Ponto de entrada da aplicação. Uso esperado:
 * ./multmatrix l c o|t
 * Onde:
 * l => número de linhas da matriz M1 (automaticamente colunas da matriz M2)
 * c => número de colunas da matriz M1 (automaticamente linhas da matriz M2)
 * o|t => o = matriz M2 original, t = matriz M2 transposta
 * --verbose => escrever matrizes na tela (padrao: desativado)
 * @param int argc Número de argumentos + 1
 * @param char** argv Nome do programa + argumentos
 */
int main (int argc, char **argv) {
    int lm1, cm1;
    bool otimizar, verbose = FALSE;
    double **M1, **M2, **Mf;
    float tempo_global = 0.0, tempo_mult = 0.0;
    clock_t t_global_inicio, t_global_fim, t_mult_inicio, t_mult_fim;

    setlocale(LC_NUMERIC, "pt_BR.utf8");

    if (argc < 4) {
        return print_argumentos_invalidos();
    }

    lm1 = atoi(argv[1]);
    cm1 = atoi(argv[2]);

    if ((lm1 == 0) || (cm1 == 0)) {
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

    t_global_inicio = clock();

    M1 = gera_matriz(lm1, cm1);
    if (verbose) print_matriz(M1, lm1, cm1, "M1:");
    M2 = gera_matriz(cm1, lm1);
    if (verbose) print_matriz(M2, cm1, lm1, "M2:");

    if (otimizar) {
        M2 = transpoe_matriz(M2, cm1, lm1);
        if (verbose) print_matriz(M2, lm1, cm1, "M2t:");
    }

    t_mult_inicio = clock();
    Mf = multiplica_matrizes(M1, M2, lm1, cm1, otimizar);
    t_mult_fim = clock();

    if (verbose) print_matriz(Mf, lm1, lm1, "Mf: ");

    t_global_fim = clock();

    printf("Tempo total (desde a a alocação da primeira matriz):\n\t%f\tsegundos\nTempo da multiplicação:\n\t%f\tsegundos\n",
           (float) (((t_global_fim - t_global_inicio) + 0.0) / CLOCKS_PER_SEC),
           (float) (((t_mult_fim - t_mult_inicio) + 0.0) / CLOCKS_PER_SEC));

    return 0;
}

/**
 * Escreve o erro de argumentos inválidos.
 * @return codigo de erro
 */
int print_argumentos_invalidos () {
    printf("Argumentos inválidos.\nUso: ./multmatrix l c o|t [--verbose]\nOnde:\nl: número de linhas da matriz M1 (colunas da matriz M2)\nc: número de colunas da matriz M1 (linhas da matriz M2)\no|t: matriz M2 original (o) ou transposta (t)\n--verbose: escrever matrizes durante o processo (desativado por padrão)\n");
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
    M = (double **) calloc((size_t) l, sizeof(double *));
    for (i = 0; i < l; i++) {
        M[i] = (double *) calloc((size_t) c, sizeof(double));
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
 * @param l quantidade de linhas da matriz de entrada, colunas da matriz de saida
 * @param c quantidade de colunas da matriz de entrada, linhas da matriz de saida
 */
double **transpoe_matriz (double **M, int l, int c) {
    int i, j;
    double **Mt;

    Mt = (double **) calloc((size_t) c, sizeof(double *));
    for (i = 0; i < c; i++) {
        Mt[i] = (double *) calloc((size_t) l, sizeof(double));
    }

    for (i = 0; i < c; i++) {
        for (j = 0; j < l; j++) {
            Mt[i][j] = M[j][i];
        }
    }

    return Mt;
}

/**
 * Multiplica as matrizes.
 * @param M1 matriz da esquerda
 * @param M2 matriz da direita
 * @param lm1cm2 quantidade de linhas da matriz M1, ordem da matriz Mf
 * @param cm1lm2 quantidade de colunas da matriz M1, linhas da matriz M2
 * @param otimizar otimizar a multiplicaçao por meio de transposiçao
 * @return matriz resultado
 */
double **multiplica_matrizes (double **M1, double **M2, int lm1cm2, int cm1lm2, bool otimizar) {
    int i, j, k;
    double temp = 0, **Mf;

    Mf = (double **) calloc((size_t) lm1cm2, sizeof(double *));
    for (i = 0; i < lm1cm2; i++) {
        Mf[i] = (double *) calloc((size_t) lm1cm2, sizeof(double));
    }

    if (otimizar) {
        for (i = 0; i < lm1cm2; i++) {
            for (j = 0; j < lm1cm2; j++) {
                temp = 0;
                for (k = 0; k < cm1lm2; k++) {
                    temp += M1[i][k] * M2[j][k];
                }
                Mf[i][j] = temp;
            }
        }
    } else {
        for (i = 0; i < lm1cm2; i++) {
            for (j = 0; j < lm1cm2; j++) {
                temp = 0;
                for (k = 0; k < cm1lm2; k++) {
                    temp += M1[i][k] * M2[k][j];
                }
                Mf[i][j] = temp;
            }
        }
    }

    return Mf;
}