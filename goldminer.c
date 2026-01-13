//  ____       _     _ __  __ _                 
// / ___| ___ | | __| |  \/  (_)_ __   ___ _ __ 
//| |  _ / _ \| |/ _` | |\/| | | '_ \ / _ \ '__|
//| |_| | (_) | | (_| | |  | | | | | |  __/ |   
// \____|\___/|_|\__,_|_|  |_|_|_| |_|\___|_|   
   
//última atualização: 03/11/25

//NOTA:desenvolvimento em progresso, as notas podem conter erros e ambiguidades

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
//inclui  a biblioteca conio para o windos e input para o linux
//essas duas bibliotecas, respectivas a cada sistema, possuem as 
//funções kbhit() e getch() usadas no código para receber o input das teclas
#ifdef _WIN32
    #include <conio.h>  
    #include <windows.h>
    #define sleep_ms(ms) Sleep(ms)  // Sleep usa milissegundos no Windows
#else
    #include "input.c"
    #define sleep_ms(ms) usleep((ms) * 1000)  // usleep usa microssegundos no Linux
#endif
//LARGURA e ALTURA do mapa, podem ser mudadas para um novo enquadramento sem problemas
//padrão: LARGURA:90 blocos  ALTURA:40 blocos
#define LARGURA 190
#define ALTURA 50

#define TEMPO 60

#define PULO_MAXIMO 25  //define quantos frames vão se passar até o player poder pular de novo

// Códigos ANSI para cores
#define RESET   "\x1b[0m" //Cor padrão
#define VERDE   "\x1b[32m"
#define CINZA   "\x1b[90m"
#define AZUL    "\x1b[34m"
#define AMARELO "\x1b[33m"
#define CIANO   "\x1b[36m"

#define TERRA VERDE "█"
#define PEDRA CINZA "█"
#define CEU AZUL "█"
#define OURO AMARELO "█"
//MADEIRA é cor ciano por falta de código ANSI para marrom
#define MADEIRA CIANO "█"
#define PERSONAGEM AMARELO "@"


//superficie é onde acaba o céu e começa a terra
//
//     ###      ###
//    #   ### ##   #  ##      ####
//## #       #      ##  #    #    ##
//  #                    ####
//
int* superficie_generate(int superficie[LARGURA]){
    for (int x=1; x < LARGURA; x++){
        //rand() % 3 gera um número entre 0 e 2, intervalo de 3 números, subtraindo por 1, dá
        //um intervalo de -1 e 1
        int variacao = (rand() % 3) - 1;

        //a superficie x vai ser igual a superficie anterior(x-1)
        //somando a variação, entre -1 e 1
        //ou seja, subindo, descendo, ou se mantendo o mesmo
        superficie[x] = superficie[x - 1]+ variacao;

        //mantem a superficie entre 3 blocos acima da altura mínima e 3 blocos abaixo da altura máxima;
        if (superficie[x] < 3) superficie[x]=3;
        if (superficie[x] > ALTURA - 3) superficie[x] = ALTURA - 3;
    }
    return superficie;
}

//gera o mapa
void map_generate(int superficie[LARGURA], char *map[ALTURA][LARGURA]){
    int profundidade;
    int chance;
    //gera o mapa, de y até a ALTURA e entre x até a LARGURA, isso feito por meio de loops com o for
    for (int y = 0; y < ALTURA; y++){
        for (int x = 0; x < LARGURA; x++){
          //gera o céu acima da superficie
          //NOTA: o código está renderizando de cima para baixo, então y menor que superficie é a parte de cima do mapa
          if (y < superficie[x]){
                map[y][x] = CEU;
            } else {
              //profundidade é a quantidade de blocos a baixo da superficie
              profundidade = y - superficie[x];
              //gera um número de 0 a 99, se for menor que profundidade vezes 5, então dá pedra ou ouro, senão terra
              chance=rand() % 100;
              //a chance de ser pedra ou ouro é 5 possibilidades entre 100, contando de 0 a 99, que cresce de acordo com o aumento da profundidade
              //5 chance vezes a profundidade
              if (chance < 5 * profundidade){
                //se chance tiver um numero maior que 90, então será ouro
                //chance de 8 por cento, visto que entre 91 e 99 há 8 números
                if(chance>90){
                  map[y][x] = OURO;
                }else{
                  map[y][x] = PEDRA;
                }
              } else {
                map[y][x] = TERRA;
              }
            }
        }
    }
}

int iniciar_jogo(const char *jogador){
  int pulo=0, pontos=0, modo=0, tempo_limite=TEMPO;
  char continuar[100];
  char *map[ALTURA][LARGURA];
  //superficie_base é onde o primeiro bloco será colocado, a partir dele os outros serão gerados aleatoriamente
  //tendo como base a posição do primeiro bloco. Ele é a base para a criação das outras supericies.
  //ele será colocado em ALTURA/3, ou seja, dois terços de céu acima, um terço terra.
  //o número pode ser aleatorizado com algumas pequenas alterações não inclusas aqui ainda
  int superficie_base[LARGURA];
  superficie_base[0] = ALTURA / 3;
  srand(time(NULL));  //apenas configuração da biblioteca de tempo
  int* superficie = superficie_generate(superficie_base);
  //gera o mapa(map), com base na superficie, ou seja, com base em qual altura(y) acaba o céu e começa a terra
  map_generate(superficie, map);

  // Posição inicial do personagem
  //meio do mapa
  int px = LARGURA / 2;
  int py = superficie[px] - 1; // Fica no topo da superfície
                               //
  time_t inicio = time(NULL); //horario de inicio do jogo
  while(1){
    time_t agora = time(NULL);  //horario de agora
    int tempo_passado = (int)(agora - inicio);
    printf("\x1b[2J\x1b[H");  //limpa a tela
    printf("pontos: %d  tempo: %d\n", pontos, tempo_limite-tempo_passado);
    for (int y = 0; y < ALTURA; y++){
      for (int x = 0; x < LARGURA; x++){
        //renderiza o player
        //px e py são as coordenadas em que o player está, x e y respectivamente à largura e à altura
        if (x == px && y == py){
          printf(PERSONAGEM);
        }else{
          //renderiza o mapa
          //o mapa só é renderizando o onde o player não está, pois senão ficaria um bloco dentro do player
          printf("%s",map[y][x]);
        }
      }
      //pula para a próxima linha
      //renderizando o próximo y
      printf("\n");
    }

    if (tempo_passado >= tempo_limite) {
      printf("Tempo esgotado!\n");
      printf("parabéns %s você conquistou %d pontos!\n", jogador, pontos);
      printf("pressione enter para continuar!");
      getchar();
      // limpa o buffer
      int c;
      while ((c = getchar()) != '\n' && c != EOF) {}

      break;
    }

    sleep_ms(10); // delay entre cada frame em milissegundos

    // Movimentação
    //kbhit verifica se alguma tecla foi apertada
    if (kbhit()){
      // getch pega a tecla que foi apertada e coloca na variável c
      char c = getch();
      switch(c){
        case 'a':
          if (px>0 && strcmp(map[py][px-1], CEU)==0) px--; // anda para esquerda
          break;
      
        case 'd':
          if (px < LARGURA-1 && strcmp(map[py][px+1], CEU)==0) px++; // anda para direita
          break;
        case 'w':
          if (strcmp(map[py-1][px], CEU)==0){ 
          //define que o player pulou
          //mais informações abaixo
            if(pulo==0){
              pulo=PULO_MAXIMO;
            }    
          } // pula
          break;
        case 's':
          if (py < ALTURA-1 && strcmp(map[py+1][px], CEU)==0) py++; // desce
          break;
        //teclas de ação
        //o intuito das teclas de ação é quebrar os blocos no modo 0 e colocar blocos no modo 1
      
        // ação para esquerda
        case 'l':
          if (modo==1){
            if (strcmp(map[py][px+1], CEU)==0){
              map[py][px+1]=MADEIRA;
            }
          }else{
            if(strcmp(map[py][px+1], CEU)!=0){
              if (strcmp(map[py][px+1], OURO)==0) pontos+=10;
              map[py][px+1]=CEU;
            }
          }
          break;

        // ação para direita
        case 'j': 
          if (modo==1){
            if (strcmp(map[py][px-1], CEU)==0){
                map[py][px-1]=MADEIRA;
            }
          }else{
            if(strcmp(map[py][px-1], CEU)!=0){
              if (strcmp(map[py][px-1], OURO)==0) pontos+=10;
              map[py][px-1]=CEU;
            }
          }    
          break;

        // ação para cima
        case 'i': 
          if (modo==1){
            if (strcmp(map[py-1][px], CEU)==0){
              map[py-1][px]=MADEIRA;
            }
          }else{
            if(strcmp(map[py-1][px], CEU)!=0){
              if (strcmp(map[py-1][px], OURO)==0) pontos+=10;
              map[py-1][px]=CEU;
            }
          }
          break;

        // ação para baixo 
        case 'k':  
          //checa se o jogador não está na altura limite
          if (py < ALTURA-1){
            if (modo==1){
              if (strcmp(map[py+1][px], CEU)==0){
                map[py+1][px]=MADEIRA;
              }
            }else{
              //quebra o que está debaixo do jogador
              if (strcmp(map[py+1][px], OURO)==0) pontos+=10;
              map[py+1][px]=CEU;
            }
          }
          break;

        //muda o modo entre construir e destruir sempre que aperta o botão "m"
        case 'm':
          switch (modo){
            case 0:
              modo=1;
              break;
            case 1:
              modo=0;
              break;
          }
      }
      if (c == 'q') break; // sair
    }
      //código para a física do pulo
      //verifica se o player não esta no limite inferior do mapa antes
      //de comparar o bloco de baixo com o céu
      if(py<ALTURA-1){
        if (strcmp(map[py+1][px], CEU)==0 && pulo<PULO_MAXIMO/5){
          //volta o personagem para o chão
          py++;
        }
      }
      //pulo
      if(pulo>0){
        //pula apenas uma vez, quando paulo é igual a PULO_MAXIMO
        if (pulo==PULO_MAXIMO){
          py--;
        }
        //diminui o pulo até 0
        //enquanto pulo for maior que 0 o player não pode pular outra vez, isso ocasionaria
        //a situação do jogador sair voando ao invés de pular
        //a cada vez que o código passa aqui o pulo diminui de 1 em 1
        //por exemplo: se o código passar aqui 1 vez por segundo(1 frame por segundo), irá demorar 200 segundos para o jogador poder pular de novo
        //mas esse número de exemplo é hipotético e irreal!
        pulo--;
      }
    }
  return pontos;
}

