#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int linhas;
    int colunas;
    float *dados;
} Matriz;

// Função para ler uma matriz de um arquivo binário
Matriz lerMatriz(char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nomeArquivo);
        exit(1);
    }

    Matriz matriz;
    fread(&matriz.linhas, sizeof(int), 1, arquivo);
    fread(&matriz.colunas, sizeof(int), 1, arquivo);

    matriz.dados = (float *)malloc(matriz.linhas * matriz.colunas * sizeof(float));
    if (matriz.dados == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a matriz\n");
        exit(1);
    }

    fread(matriz.dados, sizeof(float), matriz.linhas * matriz.colunas, arquivo);
    fclose(arquivo);

    return matriz;
}

// Função para liberar a memória alocada para uma matriz
void liberarMatriz(Matriz matriz) {
    free(matriz.dados);
}

// Função para multiplicar duas matrizes
Matriz multiplicarMatrizes(Matriz matriz1, Matriz matriz2) {
    if (matriz1.colunas != matriz2.linhas) {
        fprintf(stderr, "Não é possível multiplicar as matrizes: número de colunas da primeira matriz não é igual ao número de linhas da segunda matriz\n");
        exit(1);
    }

    Matriz resultado;
    resultado.linhas = matriz1.linhas;
    resultado.colunas = matriz2.colunas;
    resultado.dados = (float *)malloc(resultado.linhas * resultado.colunas * sizeof(float));
    if (resultado.dados == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a matriz resultado\n");
        exit(1);
    }

    for (int i = 0; i < resultado.linhas; i++) {
        for (int j = 0; j < resultado.colunas; j++) {
            float soma = 0.0;
            for (int k = 0; k < matriz1.colunas; k++) {
                soma += matriz1.dados[i * matriz1.colunas + k] * matriz2.dados[k * matriz2.colunas + j];
            }
            resultado.dados[i * resultado.colunas + j] = soma;
        }
    }

    return resultado;
}

// Função para escrever uma matriz em um arquivo binário
void escreverMatriz(Matriz matriz, char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao criar o arquivo %s\n", nomeArquivo);
        exit(1);
    }

    fwrite(&matriz.linhas, sizeof(int), 1, arquivo);
    fwrite(&matriz.colunas, sizeof(int), 1, arquivo);
    fwrite(matriz.dados, sizeof(float), matriz.linhas * matriz.colunas, arquivo);
    fclose(arquivo);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <arquivo_matriz1> <arquivo_matriz2> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    // Ler as duas matrizes dos arquivos binários
    Matriz matriz1 = lerMatriz(argv[1]);
    Matriz matriz2 = lerMatriz(argv[2]);

    // Multiplicar as duas matrizes
    Matriz resultado = multiplicarMatrizes(matriz1, matriz2);

    // Escrever o resultado em um arquivo binário
    escreverMatriz(resultado, argv[3]);

    // Liberar a memória alocada para as matrizes
    liberarMatriz(matriz1);
    liberarMatriz(matriz2);
    liberarMatriz(resultado);

    printf("Multiplicação de matrizes concluída com sucesso. O resultado foi salvo em %s\n", argv[3]);

    return 0;
}
