#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "constants.h"
#include "renderer.h"
#include "utils.h"

int main() {
  must_init(al_init(), "allegro");
  must_init(al_init_image_addon(), "allegro");
  must_init(al_init_primitives_addon(), "primitives");
  must_init(al_install_keyboard(), "keyboard");

  srand(time(NULL));

  ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
  must_init(timer, "timer");

  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  must_init(queue, "queue");

  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_timer_event_source(timer));

  unsigned char keyboard_keys[ALLEGRO_KEY_MAX];
  ClearKeyboardKeys(keyboard_keys);

  ALLEGRO_EVENT event;

  Renderer renderer;
  FillRenderer(&renderer);
  al_register_event_source(queue, al_get_display_event_source(renderer.display));

  al_start_timer(timer);

      static int transition_counter = 0;
      const int TRANSITION_DELAY_FRAMES = 30; //  0.5 segundo a 60 FPS

  while (1) {
    al_wait_for_event(queue, &event);
    
    int done = 0, print_combat = 0, redraw = 0;

    switch (event.type) {
      case ALLEGRO_EVENT_TIMER:
        redraw = 1;

        if (keyboard_keys[ALLEGRO_KEY_Q]) {
          done = 1;
          break;
        }
          
        for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
          keyboard_keys[i] &= ~GAME_KEY_SEEN;
        }

        if (renderer.combat.state == TRANSITION_TURN) {
          transition_counter++;
                
          if (transition_counter >= TRANSITION_DELAY_FRAMES) {
            printf("Transição concluída! Iniciando turno do jogador...\n");
              StartPlayerTurn(&renderer.combat); 
              transition_counter = 0; // Reseta para a próxima vez
          }
         }        
        break;

      case ALLEGRO_EVENT_KEY_DOWN:
        keyboard_keys[event.keyboard.keycode] = GAME_KEY_SEEN | GAME_KEY_DOWN;
        break;
      case ALLEGRO_EVENT_KEY_UP:
        keyboard_keys[event.keyboard.keycode] &= ~GAME_KEY_DOWN;
        break;
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        done = true;
        break;
    }


    if (done) {
      break;
    }


    // You want to put your combat logic here.

    // 1. Controle de Seleção de Cartas e Alvos
    if (renderer.combat.state == PLAYER_TURN) {
    
    // Mover seleção de carta para a esquerda
    if (keyboard_keys[ALLEGRO_KEY_LEFT] & GAME_KEY_DOWN) {
        // Se a seleção de alvo estiver ativa, move o alvo. Se não, move a carta.
        MoveCardSelection(&renderer.combat, -1);
    }
    
    // Mover seleção de carta para a direita
    if (keyboard_keys[ALLEGRO_KEY_RIGHT] & GAME_KEY_DOWN) {
        // Se a seleção de alvo estiver ativa, move o alvo. Se não, move a carta.
        MoveCardSelection(&renderer.combat, 1);
    }
    
    // Para simplificar, vamos mapear o CTRL para alternar entre alvos.
    if (keyboard_keys[ALLEGRO_KEY_LCTRL] & GAME_KEY_DOWN) {
        MoveTargetSelection(&renderer.combat, 1);
    }
    
    // Ação: Jogar Carta (ENTER)
    if (keyboard_keys[ALLEGRO_KEY_ENTER] & GAME_KEY_DOWN) {
        PlayCard(&renderer.combat); // Chama a lógica principal de jogar a carta
    }
    
    // Ação: Encerrar Turno (ESC)
    if (keyboard_keys[ALLEGRO_KEY_ESCAPE] & GAME_KEY_DOWN) {
        printf("Encerrando turno do jogador....\n");
        EndPlayerTurn(&renderer.combat);
    }


  }

  ClearKeyboardKeys(keyboard_keys);

    if (redraw && al_is_event_queue_empty(queue)) {
      Render(&renderer);
      redraw = 0;
    }
  }


  al_destroy_timer(timer);
  al_destroy_event_queue(queue);
  ClearRenderer(&renderer);
  return 0;
}