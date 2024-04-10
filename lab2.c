#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX 1000 //valor maximo dos elementos do vetor
int soma; //variável global para a soma das threads
int c = 0;


typedef struct {
   int id; //id da thread
   long int size; //tamanho do bloco que a thread executa (cada thread processa um bloco)
   int *vetor; //ponteiro para o vetor
} tArgs;





//fluxo das threads
void * tarefa(void * arg) {
   tArgs args = *(tArgs *) arg; 
   int idx = args.id;
   for(long int i=idx; i< idx + args.size; i++) {
    soma += args.vetor[i];
   } 
   printf("Soma bloco %d: %d\n", c, soma);
   c++;

    pthread_exit((void *) soma);
}

int main (int argc, char *argv[]){
    //inicialziando parâmetros da função main
    int nthreads;
    int soma_total;
    int fator = 1;
    soma = 0;


       //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
   if(argc < 2) {
       fprintf(stderr, "Digite: %s <numero threads>\n", argv[0]);
       return 1; 
   }
   nthreads = atoi(argv[1]);
   printf("nthreads=%d\n", nthreads); 

    //inicializando o vetor
    int elem;
    printf("Tamanho do vetor:\n");
    int tamanho;
    scanf("%d", &tamanho);
    int tam_bloco = tamanho/nthreads;
    int soma_real = 0;
    int vetor[tamanho];
    //gera e salva no vetor os elementos, além de registrar a soma correta dos mesmos
    for (int i=0; i<tamanho; i++) {
        elem = (rand() % MAX)/3.0 * fator;
        soma_real += elem;
        vetor[i] = elem;
        printf("%d ", elem);
    }
    printf("\n");

   //cria threads, executa e retorna o valor da soma de cada thread, somando a varável 'soma_total'

   pthread_t id_tds_sys[nthreads]; //criando lista com os identificadores das threads a serem criadas

    tArgs *args; //criando ponteiro para a variável que receberá os argumentos das threads 
    //criando as threads, por um loop for

    for(int i =0; i < nthreads; i++ ){ //loop para criação e incialização das threads
        //printf("--Alocando e preenchendo argumentos para a thread %d\n", i);
        args = malloc(sizeof(tArgs));
        if (args == NULL){
            printf("Erro na alocação de memória\n");
            exit(-1);}
        args->vetor = vetor;
        args->size = tam_bloco;
        args->id = i*args->size;
        //printf("##Criando thread %d\n", i);
        printf("Thread número %d,com id %d, incializa com o valor %d\n", i,args->id, args->vetor[i*args->size]);
        if(pthread_create(&id_tds_sys[i], NULL, tarefa, (void *) args)){
            printf("Erro na criação da thread %d\n", i);
            exit(-1);
            }
    }
    //--espera todas as threads terminarem
  	for (int i=0; i<nthreads; i++) {
    	if (pthread_join(id_tds_sys[i], (void**) &soma_total)) {
         	printf("--ERRO: pthread_join() \n"); 
            exit(-1); 
    } 
  }
    printf("Fim da execução das threads\n");
	printf("Soma total real: %d\n", soma_real);
    printf("Soma total calculada: %d\n", soma_total);
	
	
    pthread_exit(NULL);

	
    
    
    
    
    
    return 0;


}
   