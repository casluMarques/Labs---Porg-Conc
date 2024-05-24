#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Função para verificar se um número é primo
int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main() {
    FILE *file;
    int item;
    int contador_primos = 0;

    // Abrir o arquivo binário para leitura
    file = fopen("numeros.bin", "rb");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Ler os números inteiros do arquivo e contar quantos são primos
    while (fread(&item, sizeof(int), 1, file) == 1) {
        if (ehPrimo(item)) {
            contador_primos++;
        }
    }

    // Fechar o arquivo
    fclose(file);

    // Exibir o total de números primos encontrados
    printf("Total de números primos encontrados: %d\n", contador_primos);

    return 0;
}
