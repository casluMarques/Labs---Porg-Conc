#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <math.h>

#define M 10
#define CONSUMIDORES 4
#define MAX 5

//struct para verificação da thread com mais achados, e quantos foram
typedef struct {
    int id;
    int contagem_primos;
} ThreadInfo;

// Variáveis globais
sem_t bufferCheio;  // Semáforos para sincronização por condição
sem_t bufferVazio;
sem_t mutexGeral;   // Semáforo único para sincronização entre produtores e consumidores e para log

int BUFFER[M];
int contador_primos = 0;
int in = 0;
int out = 0;

ThreadInfo consumidores_info[CONSUMIDORES];

int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i <= sqrt(n); i += 2)
        if (n % i == 0) return 0;
    return 1;
}

void *função_inserir() {
    FILE *file;
    int item;
    file = fopen("numeros.bin", "rb");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        pthread_exit(NULL);
    }
    
    while (fread(&item, sizeof(int), 1, file) == 1) {
        sem_wait(&bufferVazio);
        sem_wait(&mutexGeral);
        
        BUFFER[in] = item;
        in = (in + 1) % M;

        sem_post(&mutexGeral);
        sem_post(&bufferCheio);
    }

    fclose(file);
    pthread_exit(NULL);
}

void *função_retirar(void * args) {
    ThreadInfo *info = (ThreadInfo *)args;
    while (1) {
        sem_wait(&bufferCheio);
        sem_wait(&mutexGeral);

        int item = BUFFER[out];
        out = (out + 1) % M;
        
        contador_primos += ehPrimo(item);
        info->contagem_primos++;

        sem_post(&mutexGeral);
        sem_post(&bufferVazio);
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    pthread_t tid[CONSUMIDORES];
    int i;

    // Inicializa a struct dos consumidores
    for (i = 0; i < CONSUMIDORES; i++) {
        consumidores_info[i].id = i;
        consumidores_info[i].contagem_primos = 0;
    }

    // Inicializa os semáforos
    sem_init(&mutexGeral, 0, 1);
    sem_init(&bufferCheio, 0, 0);
    sem_init(&bufferVazio, 0, M);

    // Inicia o thread produtor
    if (pthread_create(&tid[0], NULL, função_inserir, NULL)) {
        printf("Erro na criação do thread produtor\n");
        exit(1);
    }

    // Inicia os threads consumidores
    for (i = 1; i <= CONSUMIDORES; i++) {
        if (pthread_create(&tid[i], NULL, função_retirar, &consumidores_info[i])) {
            printf("Erro na criação do thread consumidor\n");
            exit(1);
        }
    }

    // Aguarda o término do thread produtor
    pthread_join(tid[0], NULL);

    // Destrói os semáforos
    sem_destroy(&mutexGeral);
    sem_destroy(&bufferCheio);
    sem_destroy(&bufferVazio);

    // Determina qual thread encontrou mais números primos
    int max_primos = 0;
    int thread_vencedor = -1;
    for (i = 0; i < CONSUMIDORES; i++) {
        if (consumidores_info[i].contagem_primos > max_primos) {
            max_primos = consumidores_info[i].contagem_primos;
            thread_vencedor = consumidores_info[i].id;
        }
    }

    printf("Total de números primos encontrados: %d\n", contador_primos);
    printf("Thread %d encontrou o maior número de primos: %d\n", thread_vencedor, max_primos);

    return 0;
}
