#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>


typedef struct Combat Combat; //declaracao antecipada 


typedef enum { //aqui estao os tipos de cartas
    ATTACK,
    DEFENSE,
    SPECIAL
} CardType;

typedef struct { //aqui esta a estrutura de uma carta
    CardType type;
    int energy_cost; //custo de energia ( 0 a 3 )
    int effect_value; //valor do efeito (dano ou escuto)
} Card;

//constantes para a logica das cartas
#define INITTIAL_DECK_SIZE 20
#define MAX_CARDS 80
#define MAX_HAND_SIZE 5
#define MAX_ENEMIES 2


typedef struct CardGroup { //estrutura de um grupo de cartas
    Card cards[MAX_CARDS];
    int count;
} CardGroup;

typedef struct { //estrutura de uma criatura (vale tanto para jogadores quanto para inimigos)
  int max_health;
  int current_health;
  int shield;
  _Bool is_alive; //essa funcao bool serve pra saber se a criatura esta viva (vida > 0);
  int total_frames;          // O total de quadros 
  int current_frame;         // O quadro atual a ser desenhado (0 a 5)
  int frame_delay;           // Contador para controlar a velocidade da animação
  int frame_delay_max;       // O valor que o delay deve atingir antes de mudar o frame 
} Creature;

typedef struct { //estrutura do jogador 
    Creature base; //vida/escudo do player
    Card deck[INITTIAL_DECK_SIZE]; // baralho incial de 20 cartas
    CardGroup draw_pile;
    CardGroup discard_pile; //pilha de compras e de descarte;
    Card hand[MAX_HAND_SIZE]; //mao do jogador
    int hand_count; 
    int current_energy; //energia atual do player 
    int max_energy;
} Player;


typedef enum { //estrutura para o tipo de inimigo, podendo ser forte ou fraco
    WEAK,
    STRONG
} EnemyType;


typedef struct { //estrutra para as acoes de "IA" do inimigo
    CardType type; // Tipo de ação (ATAQUE ou DEFESA) 
    int effect_value; // Valor do efeito (dano ou escudo) 
} EnemyAction;


typedef struct {  //estrutura de um inimigo
    Creature base;
    EnemyType type;
    EnemyAction ai_actions[3]; // Ações da IA (máx 3 para um forte)
    int ai_action_count; // Quantidade de ações
    int current_action_index; // Índice da próxima ação a ser executada
} Enemy;


typedef struct { // Estrutura de um grupo de Inimigos 
    Enemy enemies[MAX_ENEMIES];
    int count;
} EnemyGroup;


//-----------ESTRUTURAS DE COMBATE--------------
typedef enum { //estrutura para o estado do jogo 
    SETUP,
    PLAYER_TURN,
    ENEMY_TURN,
    TRANSITION_TURN,
    COMBAT_END,
    GAME_OVER,
    GAME_WON // Vitória após 10 combates 
} GameState;


 struct Combat { //estrutura principal de combate
    Player player;
    EnemyGroup enemies;
    GameState state;

    int current_combat_number; // Para contar os 10 combates 
    int card_selection_index; // Índice da carta selecionada na mão 
    int target_enemy_index; // Índice do inimigo alvo selecionado 
    _Bool did_combat_end;
} ;

typedef struct  {
  ALLEGRO_DISPLAY* display;
  ALLEGRO_BITMAP* display_buffer;
  ALLEGRO_FONT* font;
  Combat combat;
} Renderer;



void InitializeDeck(Player* player);

void InitializePlayer(Player* player);

void InitializeEnemies(EnemyGroup* group);

void InitializeCombat(Combat *combat);

void DrawCards(Player* player, int n);

Card GenerateRandomCard (CardType type, int cost);

void FillRenderer(Renderer* renderer);

void Render(Renderer* renderer);

void ClearRenderer(Renderer* renderer);

void MoveCardSelection(Combat* combat, int direction); // direction: +1 (Dir), -1 (Esq)

void MoveTargetSelection(Combat* combat, int direction); // direction: +1 (Dir), -1 (Esq)

_Bool PlayCard(Combat* combat); // Retorna TRUE se a carta foi jogada com sucesso

_Bool PlayCard(Combat* combat); 

void MoveCardToDiscard(Combat* combat, int hand_index);

void ApplyCardEffect(Combat* combat, const Card* card);

void RemoveCardFromHand(Player* player, int hand_index);

void EndPlayerTurn(Combat* combat);

void StartEnemyTurn(Combat* combat);

void StartPlayerTurn(Combat* combat);

void ShuffleCardGroup(CardGroup* group);

void CheckCombatEnd(Combat* combat);

void RenderEnergy(Renderer* renderer);

void RenderDrawPile(Renderer* renderer);

void RenderDiscardPile(Renderer* renderer);

void RenderCombatNumber(Renderer* renderer);

#endif