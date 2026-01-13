#ifndef INPUT_H
#define INPUT_H
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

// Retorna 1 se uma tecla foi pressionada, 0 caso contrário
int kbhit(void) {
  struct termios oldt, newt;
  int ch;
  int oldf;

  // Salva as configurações atuais do terminal
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;

  // Desliga o modo canônico e o eco (não precisa apertar Enter)
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  // Deixa o stdin como "não bloqueante"
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  // Tenta ler um caractere
  ch = getchar();

  // Restaura as configurações originais do terminal
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  // Se leu alguma coisa, coloca de volta e retorna 1
  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }
    return 0;
  }

  // Lê uma tecla sem precisar apertar Enter
int getch(void) {
  struct termios oldt, newt;
  int ch;

  // Desliga o modo canônico e o eco
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  // Lê uma tecla
  ch = getchar();

  // Restaura o terminal
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  return ch;
}
#endif
