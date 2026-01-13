#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// declara a função do outro arquivo (goldminer.c)
int iniciar_jogo(const char *jogador);

typedef struct {
    char nome[100];
    int score;
} Jogador;

int compararScore(const void *a, const void *b) {
    Jogador *j1 = (Jogador *)a;
    Jogador *j2 = (Jogador *)b;
    return j2->score - j1->score; // decrescente
}

int main() {
    Jogador jogadores[100];
    Jogador jogador_de_agora;
    int numero_de_jogadores=0;
    for (int i=0; i<=100; i++){
      printf("\x1b[2J\x1b[H");
      printf("=== Bem-vindo ao GoldMiner ===\n");
      printf("Digite o nome do jogador: ");
      scanf("%s", jogadores[i].nome);
      numero_de_jogadores+=1;
      jogador_de_agora=jogadores[i];
      jogadores[i].score = iniciar_jogo(jogadores[i].nome); // chama o jogo e recebe o score
      printf("\x1b[2J\x1b[H");
      
      // ordena o vetor pelo score (maior para menor)
      qsort(jogadores, numero_de_jogadores, sizeof(Jogador), compararScore);

      printf("\n===== RESULTADO =====\n\n");
      if (strcmp(jogador_de_agora.nome,jogadores[0].nome)==0){
        printf("Parabéns jogador %s, você alcançou o primeiro lugar no ranking!!!\n\n", jogador_de_agora.nome);
      }
      for (int i=0; i<numero_de_jogadores;i++){  
        printf("Jogador: %s\n", jogadores[i].nome);
        printf("Score: %d\n\n", jogadores[i].score);
      }
      printf("pressione enter para continuar!");
      getchar();
    }
    return 0;
}

