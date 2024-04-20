#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include "timer.h"

#define nthreads 8

//a matriz final tem o número de linhas da primeira, e o número de colunas da segunda
float *matriz_final;

typedef struct
{
   float *matriz; //matriz que será carregada do arquivo
   int linhas, colunas; //dimensoes da matriz
   long long int tam; //qtde de elementos na matriz
} Matriz;

typedef struct 
{
   Matriz matriz_1;
   Matriz matriz_2;
   int inicio; //indice das linhas 'cobertas' pela thread
   int fim;//indice das colunas 'cobertas' pela thread
} tArgs;


void * multiplica (void *arg){
   
   //passando os argumentos da thread
   tArgs args = *(tArgs *) arg; 
   Matriz matriz_1 = args.matriz_1;
   Matriz matriz_2 = args.matriz_2;

   // Loop para percorrer as linhas da matriz 1
    for (int i = args.inicio; i < args.fim; i++) {
        // Loop para percorrer as colunas da matriz 2
        for (int j = 0; j < matriz_2.colunas; j++) {
            // Inicializa o elemento na matriz final como zero
            matriz_final[i * matriz_2.colunas + j] = 0;
            // Loop para multiplicar os elementos da linha da matriz 1 pela coluna da matriz 2
            for (int k = 0; k < matriz_1.colunas; k++) {
                // Multiplica os elementos correspondentes e acumula na matriz final
                matriz_final[i * matriz_2.colunas + j] += matriz_1.matriz[i * matriz_1.colunas + k] * matriz_2.matriz[k * matriz_2.colunas + j];
            }
        }
      }

   pthread_exit(NULL);
}

//função para escrever a matriz num arquivo binário
void escreverMatriz(Matriz *matriz, char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao criar o arquivo %s\n", nomeArquivo);
        exit(1);
    }

    fwrite(&(matriz->linhas), sizeof(int), 1, arquivo);
    fwrite(&(matriz->colunas), sizeof(int), 1, arquivo);
    fwrite(matriz->matriz, sizeof(float), matriz->linhas * matriz->colunas, arquivo);
    fclose(arquivo);
}

Matriz lerMatriz(char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nomeArquivo);
        exit(1);
    }

    Matriz matriz;
    fread(&matriz.linhas, sizeof(int), 1, arquivo);
    fread(&matriz.colunas, sizeof(int), 1, arquivo);

    matriz.matriz = (float *)malloc(matriz.linhas * matriz.colunas * sizeof(float));
    if (matriz.matriz == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a matriz\n");
        exit(1);
    }

    fread(matriz.matriz, sizeof(float), matriz.linhas * matriz.colunas, arquivo);
    fclose(arquivo);

    return matriz;
}


//Função Principal
int main(int argc, char*argv[]) {
   //variáveis para medir o tempo
   double inicio, fim, delta;

   GET_TIME(inicio);

   //recebe os argumentos de entrada
   if(argc != 4) {
      fprintf(stderr, "Digite: %s <arquivo matriz 1> <arquivo matriz 2> <arquivo de saída>\n", argv[0]);
      return 1;
   }
   //structs para as threads
   Matriz matriz_1;
   Matriz matriz_2;
   tArgs *args;

   pthread_t id_tds_sys[nthreads]; //criando lista com os identificadores das threads a serem criadas
   //Recebendo e lendo as matrizes binárias geradas anteriormente

   matriz_1 = lerMatriz(argv[1]);
   matriz_2 = lerMatriz(argv[2]);

   //alocando memória para a matriz final
   matriz_final = (float *) malloc(sizeof(float)*matriz_1.colunas * matriz_2.linhas);
   //verificando se é possível fazer a multiplicação
   if (matriz_1.colunas != matriz_2.linhas){
      printf("Número de linhas não é igual ao de colunas!");
      return 4;
   }
   int linhas_por_thread = matriz_1.linhas/nthreads;
   int resto = matriz_1.linhas % nthreads;
   /*Para fins ilustrativos
   printf("Matriz 1:\n");
   for(int i=0; i<matriz_1->linhas; i++) { 
      for(int j=0; j<matriz_1->colunas; j++)
        fprintf(stdout, "%.6f ", matriz_1->matriz[i*matriz_1->colunas+j]);
      fprintf(stdout, "\n");
   }
   printf("Matriz 2: \n");
   for(int i=0; i<matriz_2->linhas; i++) { 
      for(int j=0; j<matriz_2->colunas; j++)
        fprintf(stdout, "%.6f ", matriz_2->matriz[i*matriz_2->colunas+j]);
      fprintf(stdout, "\n");
   }*/
   
   for(int i =0; i < nthreads; i++ ){ //loop para criação e incialização das threads
        printf("--Alocando e preenchendo argumentos para a thread %d\n", i);
        args = malloc(sizeof(tArgs));
        printf("##Criando thread %d\n", i);
        if (args == NULL){
            printf("Erro na alocação de memória para as threads\n");
            exit(-1);
         }
         //passando os argumentos
         args->matriz_1 = matriz_1;
         args->matriz_2 = matriz_2;
         args->inicio = i * linhas_por_thread;
         args->fim = (i + 1) * (linhas_por_thread);

         //verificando se é a última thread, para passar o resto pra ela
         if (i == nthreads-1){
            args->fim += resto;
         }
         

        if(pthread_create(&id_tds_sys[i], NULL, multiplica, (void *) args)){
            printf("Erro na criação da thread %d\n", i);
            exit(-1);
            }
    }
    //--espera todas as threads terminarem
  	for (int i=0; i<nthreads; i++) {
      if (pthread_join(id_tds_sys[i],NULL)){
         printf("--ERRO: pthread_join()\n");
      }
   }

   //salvando os dados da matriz final
   Matriz *matriz_resultante;
   matriz_resultante = malloc(sizeof(Matriz));
   matriz_resultante->linhas = matriz_1.linhas;
   matriz_resultante->colunas = matriz_2.colunas;
   matriz_resultante->matriz = matriz_final;

   //escrevendo no arquivo
   escreverMatriz(matriz_resultante, argv[3]);

   GET_TIME(fim);
   delta = fim - inicio;
   printf("Tempo de execução: %.4lf segundos\n", delta);


   return 0;
}