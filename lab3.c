#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define nthreads 1

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
   Matriz *matriz_1;
   Matriz *matriz_2;
} tArgs;


void * multiplica (void *arg){
   printf("entrei na thread\n");
   
   //passando os argumentos da thread
   tArgs args = *(tArgs *) arg; 
   Matriz matriz_1 = *(Matriz *)args.matriz_1;
   Matriz matriz_2 = *(Matriz *)args.matriz_2;

   long long int tam_mf = matriz_1.linhas * matriz_2.colunas;

   for (int c =0; c< tam_mf; c++){
      for (int i =0; i <matriz_1.linhas; i++){
         int j = i * matriz_2.colunas;
         matriz_final[c] += matriz_1.matriz[i] * matriz_2.matriz[j];
      }
   }

   printf("Matriz final:\n");
   for(int i=0; i<matriz_1.linhas; i++) { 
      for(int j=0; j<matriz_2.colunas; j++)
        fprintf(stdout, "%.6f ", matriz_final[i*matriz_2.colunas+j]);
      fprintf(stdout, "\n");
   }
   //printf("a matriz final tem o número de linhas da primeira, e o número de colunas da segunda\n");
   //printf("Matriz 1:\nNúmero de linhas: %d // Número de colunas: %d // Tamanho total:%llu\n",matriz_1.linhas,matriz_1.colunas, matriz_1.tam);
   //printf("Matriz 2:\nNúmero de linhas: %d // Número de colunas: %d // Tamanho total:%llu\n",matriz_2.linhas,matriz_2.colunas, matriz_2.tam);

   pthread_exit(NULL);
}


//Recebendo e lendo matrizes binárias geradas anteriormente
int main(int argc, char*argv[]) {
      //recebe os argumentos de entrada
   if(argc < 2) {
      fprintf(stderr, "Digite: %s <arquivo matriz 1> <arquivo matriz 2> <arquivo de saída>\n", argv[0]);
      return 1;
   }
   //structs para as threads
   Matriz *matriz_1;
   Matriz *matriz_2;
   tArgs *args;
   
   //para carregar os binários das matrizes
   float *matriz;
   int linhas, colunas;
   long long int tam;
   FILE * descritorArquivo; //descritor do arquivo de entrada
   size_t ret; //retorno da funcao de leitura no arquivo de entrada

   pthread_t id_tds_sys[nthreads]; //criando lista com os identificadores das threads a serem criadas
   //lendo e armazenando as matrizes
   for (int i=1; i < 3; i++){
      
      //abre o arquivo para leitura binaria
      descritorArquivo = fopen(argv[i], "rb");
      if(!descritorArquivo) {
         fprintf(stderr, "Erro de abertura do arquivo\n");
         return 2;
      }

      //le as dimensoes da matriz
      ret = fread(&linhas, sizeof(int), 1, descritorArquivo);
      if(!ret) {
         fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
         return 3;
      }
      ret = fread(&colunas, sizeof(int), 1, descritorArquivo);
      if(!ret) {
         fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
         return 3;
      }
      tam = linhas * colunas; //calcula a qtde de elementos da matriz

      //aloca memoria para a matriz
      matriz = (float*) malloc(sizeof(float) * tam);
      if(!matriz) {
         fprintf(stderr, "Erro de alocao da memoria da matriz\n");
         return 3;
      }
      

      //carrega a matriz de elementos do tipo float do arquivo
      ret = fread(matriz, sizeof(float), tam, descritorArquivo);
      if(ret < tam) {
         fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
         return 4;
      }


      if(i ==1){
         //aloca memoria para a struct matriz_1
         matriz_1 = malloc(sizeof(float) * tam);
         if(!matriz) {
            fprintf(stderr, "Erro de alocao da memoria da matriz\n");
            return 3;
      }
         //passando os dados para a struct
         matriz_1->linhas = linhas;
         matriz_1->colunas = colunas;
         matriz_1->tam = tam;
         matriz_1->matriz = matriz;
         }
      if(i==2){
         //aloca memoria para a struct matriz_2
         matriz_2 = malloc(sizeof(float) * tam);
         if(!matriz) {
            fprintf(stderr, "Erro de alocao da memoria da matriz\n");
            return 3;
         }
         //passando os dados para a struct
         matriz_2->linhas = linhas;
         matriz_2->colunas = colunas;
         matriz_2->tam = tam;
         matriz_2->matriz = matriz;
         }
   }
   //liberando memória depois de salvar os dados na struct
   free(matriz);
   if (matriz_1->colunas != matriz_2->linhas){
      printf("Núero de linhas não é igual ao de colunas!");
      return 4;
   }
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
   }
   //alocando memória para a matriz final
   matriz_final = malloc(matriz_1->colunas * matriz_2->linhas);

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
   return 0;
}