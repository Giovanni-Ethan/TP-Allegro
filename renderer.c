#include "renderer.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "utils.h"


Card GenerateRandomCard (CardType type, int cost){
    Card c = {0};
    c.type = type;
    c.energy_cost = cost;
    c.effect_value = 0;

    if (type == SPECIAL){ // se a carta for especial ela tem custo 0
        c.energy_cost = 0;
        return c;
    }

    switch (cost) { //aqui vai definir o valor do efeito, seja de dano ou escudo
        case 0:
            // Custo 0: Efeito entre 1 e 5
            c.effect_value = GetRandomInt(1, 5);
            break;
        case 1:
            // Custo 1: Efeito entre 5 e 10
            c.effect_value = GetRandomInt(5, 10);
            break;
        case 2:
            // Custo 2: Efeito entre 10 e 15
            c.effect_value = GetRandomInt(10, 15);
            break;
        case 3:
            // Custo 3: Efeito entre 15 e 30
            c.effect_value = GetRandomInt(15, 30);
            break;
        default:
            c.effect_value = 0;
    }
    return c;
}

void InitializeDeck(Player* player) {
    int i = 0;
    int deck_index = 0; // Índice para preencher o array player->deck

    // 1. 10 Cartas de ATAQUE
    // Mínimos obrigatórios (Total 6)
    player->deck[deck_index++] = GenerateRandomCard(ATTACK, 0); // 1x Custo 0
    for (i = 0; i < 3; i++) player->deck[deck_index++] = GenerateRandomCard(ATTACK, 1); // 3x Custo 1
    player->deck[deck_index++] = GenerateRandomCard(ATTACK, 2); // 1x Custo 2
    player->deck[deck_index++] = GenerateRandomCard(ATTACK, 3); // 1x Custo 3
    // Cartas restantes (4) para completar as 10 de Ataque (colocadas como Custo 1 para simplificar)
    for (i = 0; i < 4; i++) player->deck[deck_index++] = GenerateRandomCard(ATTACK, 1);

    // 2. 8 Cartas de DEFESA
    // Mínimos obrigatórios (Total 6)
    player->deck[deck_index++] = GenerateRandomCard(DEFENSE, 0); // 1x Custo 0
    for (i = 0; i < 3; i++) player->deck[deck_index++] = GenerateRandomCard(DEFENSE, 1); // 3x Custo 1
    player->deck[deck_index++] = GenerateRandomCard(DEFENSE, 2); // 1x Custo 2
    player->deck[deck_index++] = GenerateRandomCard(DEFENSE, 3); // 1x Custo 3

    // Cartas restantes (2) para completar as 8 de Defesa (colocadas como Custo 1)
    for (i = 0; i < 2; i++) player->deck[deck_index++] = GenerateRandomCard(DEFENSE, 1);

    // 3. 2 Cartas ESPECIAIS (Custo 0)
    player->deck[deck_index++] = GenerateRandomCard(SPECIAL, 0);
    player->deck[deck_index++] = GenerateRandomCard(SPECIAL, 0);
}

void InitializePlayer(Player* player) { //configura a enegria e o deck de cartas do jogador

    // 1. Inicializa atributos básicos da crature
    player->base.max_health = 100;
    player->base.current_health = 100;
    player->base.shield = 0;
    player->base.is_alive = true; // O jogador começa vivo
    
    // 2. Inicializa energia e estado da mão
    player->max_energy = 3;
    player->hand_count = 0;

    // 3. Inicializa Baralho e Pilhas
    InitializeDeck(player); // Cria as 20 cartas iniciais

    // Copia as 20 cartas do baralho (deck) para a pilha de compras (draw_pile)
    player->draw_pile.count = INITTIAL_DECK_SIZE;
    for (int i = 0; i < INITTIAL_DECK_SIZE; i++) {
        player->draw_pile.cards[i] = player->deck[i];
    }
    player->discard_pile.count = 0; // O descarte começa vazio

    // 4. Embaralha a pilha de compras para começar
    ShuffleCardGroup(&player->draw_pile);
}

void InitializeEnemies(EnemyGroup* group) { //essa estrutura vai gerar dois inimigos, seus atributos, e suas acoes de IA

    group->count = MAX_ENEMIES;
    int strong_enemy_count = 0;
    
    for (int i = 0; i < group->count; i++) {
        Enemy* e = &group->enemies[i];
        
        // 1. Define o Tipo (5% chance de Strong, max 1)
        if (strong_enemy_count == 0 && GetRandomInt(1, 100) <= 5) {
            e->type = STRONG;
            strong_enemy_count++;
        } else {
            e->type = WEAK;
        }

        // 2. Configurações Básicas
        e->base.shield = 0;
        e->base.is_alive = true;
        e->current_action_index = 0; // Começa na primeira ação da IA
        
        // 3. Configuração de Vida e IA
        if (e->type == WEAK) {
            e->base.max_health = GetRandomInt(10, 30); // 10 a 30 PV
            e->ai_action_count = GetRandomInt(1, 2); // 1 ou 2 ações
            
            // Definição da IA Simples (Exemplo)
            e->ai_actions[0].type = ATTACK;
            e->ai_actions[0].effect_value = GetRandomInt(5, 10);
            if (e->ai_action_count == 2) {
                e->ai_actions[1].type = DEFENSE;
                e->ai_actions[1].effect_value = GetRandomInt(3, 6);
            }
            
        } else { // STRONG
            e->base.max_health = GetRandomInt(40, 100); // 40 a 100 PV
            e->ai_action_count = GetRandomInt(2, 3); // 2 ou 3 ações
            
            // Definição da IA Forte (Exemplo)
            e->ai_actions[0].type = ATTACK;
            e->ai_actions[0].effect_value = GetRandomInt(15, 20);

            e->ai_actions[1].type = DEFENSE;
            e->ai_actions[1].effect_value = GetRandomInt(10, 15);
            
            if (e->ai_action_count == 3) {
                e->ai_actions[2].type = ATTACK;
                e->ai_actions[2].effect_value = GetRandomInt(15, 20);
            }
        }
        
        // Inicializa a vida atual
        e->base.current_health = e->base.max_health;
    }
}



void DrawCards(Player* player, int n) { //remove cartas da pilha de compras e move para a mao do joagdor, para gerenciar o proximo embaralhamento do descarte

    for (int i = 0; i < n; i++) {
        // 1. VERIFICA E REEMBARALHA: Se draw_pile está vazia, usa discard_pile
        if (player->draw_pile.count == 0) {
            if (player->discard_pile.count > 0) {
                // Move o descarte para a draw_pile
                for (int k = 0; k < player->discard_pile.count; k++) {
                    player->draw_pile.cards[k] = player->discard_pile.cards[k];
                }
                player->draw_pile.count = player->discard_pile.count;
                player->discard_pile.count = 0; 
                ShuffleCardGroup(&player->draw_pile); // Reembaralha!
            } else {
                // Não há mais cartas em lugar nenhum. Para de comprar.
                break; 
            }
        }

        // 2. VERIFICA MÃO: Se a mão está cheia, para
        if (player->hand_count >= MAX_HAND_SIZE) {
            break;
        }

        // 3. COMPRA: Move a carta do topo (último elemento)
        Card drawn_card = player->draw_pile.cards[player->draw_pile.count - 1];
        player->draw_pile.count--; // Reduz o tamanho da draw_pile

        player->hand[player->hand_count] = drawn_card;
        player->hand_count++;
    }
}


void InitializeCombat(Combat* combat) {
    // 1. Inicializa o Jogador
    InitializePlayer(&combat->player);

    // 2. Inicializa os Inimigos
    InitializeEnemies(&combat->enemies);

    // 3. Configura o estado inicial do combate
    combat->state = PLAYER_TURN;
    combat->current_combat_number = 1; // Começamos no primeiro combate
    combat->card_selection_index = 0; 
    combat->target_enemy_index = 0; 

    // 4. Inicia o turno do jogador
    combat->player.current_energy = combat->player.max_energy;
    
    // 5. Compra as 5 cartas iniciais
    DrawCards(&combat->player, 5);
}


void DrawScaledText(ALLEGRO_FONT* font, ALLEGRO_COLOR color, float x, float y,
                    float xscale, float yscale, int alignment,
                    const char* text) {
  ALLEGRO_TRANSFORM transform;
  al_identity_transform(&transform);  // Start with an identity transform
  al_scale_transform(
      &transform, xscale,
      yscale);  // Apply scaling (e.g., sx=2.0, sy=2.0 for double size)
  al_use_transform(&transform);  // Use the transform for subsequent drawing

  al_draw_text(font, color, x, y, alignment, text);
  // al_draw_text(font, color, x, y, alignment, text);
  al_identity_transform(&transform);
  al_use_transform(&transform);  // Use the transform for subsequent drawing
}

void DrawCenteredScaledText(ALLEGRO_FONT* font, ALLEGRO_COLOR color, float x,
                            float y, float xscale, float yscale, char* text) {
  DrawScaledText(font, color, x, y, xscale, yscale, ALLEGRO_ALIGN_CENTRE, text);
}

void FillRenderer(Renderer* renderer) {
  al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
  al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

  renderer->display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  must_init(renderer->display, "display");

  renderer->display_buffer =
      al_create_bitmap(DISPLAY_BUFFER_WIDTH, DISPLAY_BUFFER_HEIGHT);
  must_init(renderer->display_buffer, "display buffer");

  renderer->font = al_create_builtin_font();
  must_init(renderer->font, "font");

  InitializeCombat (&renderer->combat);
}

void RenderBackground(Renderer* renderer) {
  al_clear_to_color(al_map_rgb(0, 0, 0));
}

void RenderDeck(Renderer* renderer, int x_left, int y_top) {
  ALLEGRO_BITMAP* prev_bmp_target = al_get_target_bitmap();

  ALLEGRO_BITMAP* deck_bitmap = al_create_bitmap(DECK_WIDTH, DECK_HEIGHT);
  al_set_target_bitmap(deck_bitmap);

  al_draw_filled_rounded_rectangle(0, 0, DECK_WIDTH, DECK_HEIGHT, 10, 0,
                                   al_map_rgb(255, 255, 255));
  al_set_target_bitmap(prev_bmp_target);

  al_draw_scaled_bitmap(deck_bitmap, 0, 0, DECK_WIDTH, DECK_HEIGHT, x_left,
                        y_top, DECK_WIDTH, DECK_HEIGHT, 0);
  al_destroy_bitmap(deck_bitmap);
}

void RenderHealthBar(float x_begin, float x_end, float y_down_left,
                     ALLEGRO_FONT* font) {
  float mid_y = y_down_left - (HEALTH_BAR_HEIGHT * 0.78);

  al_draw_filled_rounded_rectangle(
      x_begin - HEALTH_BAR_BACKGROUND_EXTRA,
      y_down_left - HEALTH_BAR_BACKGROUND_EXTRA,
      x_end + HEALTH_BAR_BACKGROUND_EXTRA,
      y_down_left - HEALTH_BAR_HEIGHT + HEALTH_BAR_BACKGROUND_EXTRA,
      HEALTH_BAR_RX, HEALTH_BAR_RY, al_map_rgb(255, 255, 255));
  char text[100] = "";
  sprintf(text, "Vida?");
  float x_scale = 2.0, y_scale = 2.0;
  DrawScaledText(font, al_map_rgb(0, 0, 0), (x_begin + x_end) / 2.0 / x_scale,
                 mid_y / y_scale, x_scale, y_scale, ALLEGRO_ALIGN_CENTRE, text);
}

void RenderCreature(const Renderer* renderer, int begin_x, int mid_y,
                    int width) {
  al_draw_filled_circle(begin_x + width / 2.0, mid_y, width,
                        al_map_rgb(255, 255, 255));
  float x_end = begin_x + width;

  float health_bar_y = mid_y + width + 20;
  RenderHealthBar(begin_x, x_end, health_bar_y, renderer->font);

  // Suggestion: render enemy intents here.
}


void RenderCard(const Renderer* renderer, const Card* card, int x_left, int y_top, int is_selected) {
    // Deslocamento vertical se a carta estiver selecionada
    if (is_selected) {
        y_top -= 30;
    }

    ALLEGRO_BITMAP* card_bitmap = al_create_bitmap(CARD_WIDTH, CARD_HEIGHT);
    al_set_target_bitmap(card_bitmap);

    // Variáveis para cor e texto
    ALLEGRO_COLOR border_color;
    char type_text[20];
    char effect_text[30];

    // Define a aparência baseada no tipo da carta
    switch (card->type) {
        case ATTACK:
            border_color = al_map_rgb(200, 60, 60); // Vermelho
            sprintf(type_text, "ATAQUE");
            // Mostra o dano
            sprintf(effect_text, "Dano: %d", card->effect_value);
            break;
        case DEFENSE:
            border_color = al_map_rgb(60, 60, 200); // Azul
            sprintf(type_text, "DEFESA");
            // Mostra o escudo
            sprintf(effect_text, "Escudo: %d", card->effect_value);
            break;
        case SPECIAL:
            border_color = al_map_rgb(218, 165, 32); // Dourado
            sprintf(type_text, "ESPECIAL");
            // Efeito fixo do especial
            sprintf(effect_text, "Compra 5");
            break;
        default:
             border_color = al_map_rgb(100, 100, 100);
             sprintf(type_text, "???");
             sprintf(effect_text, "-");
    }

    // Fundo da carta
    al_draw_filled_rounded_rectangle(0, 0, CARD_WIDTH, CARD_HEIGHT, 15, 15, al_map_rgb(245, 245, 245));
    // Borda colorida
    al_draw_rounded_rectangle(0, 0, CARD_WIDTH, CARD_HEIGHT, 15, 15, border_color, 8);

    // Renderiza os Textos
    float xscale = 1.4, yscale = 1.4;
    
    // Título da Carta (Topo)
    DrawCenteredScaledText(renderer->font, border_color, CARD_WIDTH / 2, 40, xscale, yscale, type_text);

    // Efeito da Carta (Centro)
    DrawCenteredScaledText(renderer->font, al_map_rgb(0, 0, 0), CARD_WIDTH / 2, CARD_HEIGHT / 2, xscale, yscale, effect_text);

    // Custo de Energia (Canto Superior Esquerdo)
    al_draw_filled_circle(40, 40, 25, al_map_rgb(20, 20, 20)); // Fundo preto
    al_draw_circle(40, 40, 25, border_color, 3); // Borda colorida
    
    char cost_text[5];
    sprintf(cost_text, "%d", card->energy_cost);
    DrawCenteredScaledText(renderer->font, al_map_rgb(255, 255, 255), 40, 28, 2.0, 2.0, cost_text);

    // Desenha o bitmap da carta na tela principal
    al_set_target_bitmap(renderer->display_buffer);
    al_draw_scaled_bitmap(card_bitmap, 0, 0, CARD_WIDTH, CARD_HEIGHT, x_left,
                          y_top, CARD_WIDTH, CARD_HEIGHT, 0);

    al_destroy_bitmap(card_bitmap);
}

void RenderPlayerHand(Renderer* renderer) {
    // Acessa o ponteiro do jogador para facilitar a escrita
    Player* player = &renderer->combat.player;
    
    // Obtém qual índice está selecionado no momento
    int selection_index = renderer->combat.card_selection_index;
    
    // Espaço extra entre as cartas
    int spacing = 15; 

    // A mão deve estar visível
    for (int i = 0; i < player->hand_count; i++) {
        // Calcula a posição X: Posição Inicial + (Largura da Carta + Espaço) * índice
        int pos_x = HAND_BEGIN_X + (i * (CARD_WIDTH + spacing));
        int pos_y = HAND_BEGIN_Y;

        // Verifica se a carta atual (i) é a carta selecionada
        int is_selected = (i == selection_index);

        // Chama a renderização passando o endereço da carta atual
        RenderCard(renderer, &player->hand[i], pos_x, pos_y, is_selected);
    }
}

void RenderEnemies(Renderer* renderer) {

}

void RenderEnergy(Renderer* renderer) {

}

void Render(Renderer* renderer) {
  al_set_target_bitmap(renderer->display_buffer);
  RenderBackground(renderer);
  RenderDeck(renderer, DRAW_DECK_X, DRAW_DECK_Y);
  RenderCreature(renderer, PLAYER_BEGIN_X, PLAYER_BEGIN_Y + PLAYER_RADIUS,
                 PLAYER_RADIUS);
  RenderEnergy(renderer);
  RenderEnemies(renderer);
  RenderPlayerHand(renderer);
  al_set_target_backbuffer(renderer->display);

  al_draw_scaled_bitmap(renderer->display_buffer, 0, 0, DISPLAY_BUFFER_WIDTH,
                        DISPLAY_BUFFER_HEIGHT, 0, 0, DISPLAY_WIDTH,
                        DISPLAY_HEIGHT, 0);

  al_flip_display();
}

void ClearRenderer(Renderer* renderer) {
  al_destroy_display(renderer->display);
  al_destroy_bitmap(renderer->display_buffer);
  al_destroy_font(renderer->font);
}