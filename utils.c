#include "utils.h"
#include "renderer.h"
#include "constants.h"


#include <allegro5/allegro5.h>
#include <stdio.h>

void must_init(_Bool test, const char* description) {
  if (test) return;

  fprintf(stderr, "couldn't initialize %s\n", description);
  exit(1);
}

int NumToDigits(int n) {
  if (n < 0) return 1 + NumToDigits(-n);
  if (n < 10) return 1;
  if (n < 100) return 2;
  if (n < 1000) return 3;
  if (n < 10000) return 4;
  if (n < 100000) return 5;
  if (n < 1000000) return 6;
  if (n < 10000000) return 7;
  if (n < 100000000) return 8;
  if (n < 1000000000) return 9;
  /*      2147483647 is 2^31-1 - add more ifs as needed
     and adjust this final return as well. */
  return 10;
}

void ClearKeyboardKeys(unsigned char* keyboard_keys) {
  memset(keyboard_keys, 0, ALLEGRO_KEY_MAX * sizeof(unsigned char));
}

void ShuffleArray(int* array, int size) {
  if (size > 1) {
    int i;
    for (i = 0; i < size - 1; i++) {
      int j = i + rand() / (RAND_MAX / (size - i) + 1);
      if (j == i) {
        continue;
      }
      int int_j = array[j];

      array[j] = array[i];

      array[i] = int_j;
    }
  }
}

void ShuffleCardGroup(CardGroup* group) {
    if (group->count > 1) {
        int i;
        // Percorre o array de cartas
        for (i = 0; i < group->count - 1; i++) {
            // Gera um índice aleatório 'j' entre 'i' e 'group->count - 1'
            // A lógica de randomização abaixo garante que a carta só é trocada com uma carta que ainda não foi "finalizada" (da posição i em diante).

            int j = i + rand() / (RAND_MAX / (group->count - i) + 1);
            if (j == i) {
                continue;
            }

            // Realiza a troca dos elementos 
            Card temp = group->cards[j];
            group->cards[j] = group->cards[i];
            group->cards[i] = temp;
        }
    }
}

int GetRandomInt (int min, int max){
  if (min > max) return min;
  return rand() % (max - min + 1) + min;
}

