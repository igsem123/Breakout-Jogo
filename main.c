#include "raylib.h"
#define LINHAS_DE_TIJOLOS 7
#define TIJOLOS_POR_LINHA 10
#define ESPACAMENTO_TIJOLO 5
#define NUM_FRAMES 49

typedef enum TelaDoJogo { LOGO = 0, TITULO, JOGANDO, FIM } TelaDoJogo;

typedef struct Tijolo
{
    Vector2 posicao;
    bool ativo;
    Color cor;
} Tijolo;

typedef struct Jogador
{
    Vector2 posicao;
    Vector2 tamanho;
    int vida;
} Jogador;

typedef struct Bola
{
    Vector2 posicao;
    Vector2 velocidade;
    float raio;
} Bola;

const int larguraDaTela = 1280;
const int alturaDaTela = 720;
const float velocidadeInicialBola = 300.0f; //Variável para a velocidade constante da bola

TelaDoJogo telaAtual = LOGO;
Jogador jogador;
Bola bola;
Tijolo tijolo[LINHAS_DE_TIJOLOS][TIJOLOS_POR_LINHA];
Vector2 tamanhoDoTijolo;
bool jogoTerminado = false;
bool pausa = false;
int contadorDeQuadros = 0;
Color coresTijolos[LINHAS_DE_TIJOLOS] = { PINK, RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };
Texture2D gameoverTexture;
Texture2D frames[NUM_FRAMES];
Texture2D titleLogoTexture;
Texture2D backgroundTexture;
int currentFrame = 0;
float frameTime = 0.1f;
float frameTimer = 0.0f;
float logoScreenTime = 0.0f;

float inicioTimer = 3.0f; //Timer de início de jogo
int score = 0; //Variável de pontuação
int combo = 0; //Combo para calcular o bônus

Music trilhaSonora; //Variável para a música

void IniciarJogo(void);
void AtualizarJogo(void);
void DesenharJogo(void);
void DescarregarJogo(void);
void AtualizarDesenharQuadro(void);
void AtualizarTelaInicial(void);
void DesenharTelaInicial(void);
void AtualizarTelaTitulo(void);
void DesenharTelaTitulo(void);
void AtualizarTelaFim(void);
void DesenharTelaFim(void);

int main(void)
{
    InitWindow(larguraDaTela, alturaDaTela, "Breakout Jogo"); //Inicializa a janela com seu tamanho e o nome do programa
    InitAudioDevice(); //Inicializando o módulo de áudio

    trilhaSonora = LoadMusicStream("assets/trilha_sonora.mp3"); //Carregando a música de trilha sonora

    IniciarJogo();

    SetTargetFPS(60); //Define os frames por segundo que o programa mostrará

    while (!WindowShouldClose())
    {
        AtualizarDesenharQuadro();
    }

    UnloadMusicStream(trilhaSonora);
    CloseAudioDevice(); //Fecha o módulo de áudio
    CarregarJogo();

    CloseWindow();

    return 0;
}

void IniciarJogo(void)
{
    for (int i = 0; i < NUM_FRAMES; i++) //Lógica para carregar os frames do GIF
    {
        char filename[20];
        sprintf(filename, "assets/frame-%d.gif", i); //Carregando o GIF da tela de carregamento
        frames[i] = LoadTexture(filename);
    }

    titleLogoTexture = LoadTexture("assets/title_logo.png"); // Carregando a textura da logo
    backgroundTexture = LoadTexture("assets/background.png"); //Carregando a textura do background
    gameoverTexture = LoadTexture("assets/gameover.png"); //Carregando a textura do game over

    jogador.posicao = (Vector2){larguraDaTela / 2, alturaDaTela - 30};
    jogador.tamanho = (Vector2){200, 20};
    jogador.vida = 3;

    bola.posicao = (Vector2){larguraDaTela / 2, alturaDaTela / 2 + 50};
    bola.velocidade = (Vector2){velocidadeInicialBola, -velocidadeInicialBola};
    bola.raio = 15;

    tamanhoDoTijolo = (Vector2){(larguraDaTela - (TIJOLOS_POR_LINHA + 1) * ESPACAMENTO_TIJOLO) / TIJOLOS_POR_LINHA, 30};

    for (int i = 0; i < LINHAS_DE_TIJOLOS; i++)
    {
        for (int j = 0; j < TIJOLOS_POR_LINHA; j++)
        {
            tijolo[i][j].posicao = (Vector2){j * (tamanhoDoTijolo.x + ESPACAMENTO_TIJOLO) + ESPACAMENTO_TIJOLO, i * (tamanhoDoTijolo.y + ESPACAMENTO_TIJOLO) + 50};
            tijolo[i][j].ativo = true;
            tijolo[i][j].cor = coresTijolos[i];
        }
    }

    inicioTimer = 3.0f; //Inicia o timer de início
    score = 0; //Inicia a pontuação
    combo = 0; //Inicia o combo
}

void AtualizarJogo(void)
{
    if (!jogoTerminado)
    {
        if (IsKeyPressed(KEY_P)) pausa = !pausa;

        if (!pausa)
        {
            if (inicioTimer > 0)
            {
                inicioTimer -= GetFrameTime();
            }
            else
            {
                bola.posicao.x += bola.velocidade.x * GetFrameTime();
                bola.posicao.y += bola.velocidade.y * GetFrameTime();

                if (((bola.posicao.x + bola.raio) >= GetScreenWidth()) || ((bola.posicao.x - bola.raio) <= 0)) bola.velocidade.x *= -1;
                if ((bola.posicao.y - bola.raio) <= 0) bola.velocidade.y *= -1;
                if ((bola.posicao.y + bola.raio) >= GetScreenHeight())
                {
                    jogador.vida--;
                    bola.posicao = (Vector2){larguraDaTela / 2, alturaDaTela / 2};
                    bola.velocidade = (Vector2){velocidadeInicialBola, -velocidadeInicialBola};
                    combo = 0; //Reseta o combo ao perder a bola
                }

                if (IsKeyDown(KEY_LEFT) && ((jogador.posicao.x - jogador.tamanho.x / 2) > 0)) jogador.posicao.x -= 400 * GetFrameTime();
                if (IsKeyDown(KEY_RIGHT) && ((jogador.posicao.x + jogador.tamanho.x / 2) < larguraDaTela)) jogador.posicao.x += 400 * GetFrameTime();

                if (CheckCollisionCircleRec(bola.posicao, bola.raio, (Rectangle){jogador.posicao.x - jogador.tamanho.x / 2, jogador.posicao.y - jogador.tamanho.y / 2, jogador.tamanho.x, jogador.tamanho.y}))
                {
                    if (bola.velocidade.y > 0)
                    {
                        bola.velocidade.y = -velocidadeInicialBola; //Mantendo a velocidade constante ao rebater com o jogador
                    }
                }

                for (int i = 0; i < LINHAS_DE_TIJOLOS; i++)
                {
                    for (int j = 0; j < TIJOLOS_POR_LINHA; j++)
                    {
                        if (tijolo[i][j].ativo)
                        {
                            if (CheckCollisionCircleRec(bola.posicao, bola.raio, (Rectangle){tijolo[i][j].posicao.x, tijolo[i][j].posicao.y, tamanhoDoTijolo.x, tamanhoDoTijolo.y}))
                            {
                                tijolo[i][j].ativo = false;
                                bola.velocidade.y *= -1;
                                bola.velocidade.x = (bola.velocidade.x < 0) ? -velocidadeInicialBola : velocidadeInicialBola; //Mantendo a velocidade constante ao rebater nos tijolos
                                score += 50; //Incrementando a pontuação caso bata no tijolo, inicialmente de 50
                                combo++; //Incrementa o combo caso seja continuo, pontuando 70 a cada tijolo quebrado
                                if (combo > 1) score += 20;
                            }
                        }
                    }
                }

                if (jogador.vida <= 0)
                {
                    jogoTerminado = true;
                    telaAtual = FIM;  //Atualiza a tela atual para FIM quando as vidas acabam
                }
                else
                {
                    bool todosTijolosQuebrados = true;

                    for (int i = 0; i < LINHAS_DE_TIJOLOS; i++)
                    {
                        for (int j = 0; j < TIJOLOS_POR_LINHA; j++)
                        {
                            if (tijolo[i][j].ativo) todosTijolosQuebrados = false;
                        }
                    }

                    if (todosTijolosQuebrados)
                    {
                        jogoTerminado = true;
                        telaAtual = FIM;  //Atualiza a tela atual para FIM quando todos os tijolos são quebrados
                    }
                }
            }
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            IniciarJogo();
            jogoTerminado = false;
            telaAtual = JOGANDO;  //Verificando se a tela atual está sendo definida corretamente quando reiniciar o jogo
        }
    }

    UpdateMusicStream(trilhaSonora); //Atualiza a música a cada frame
}


void DesenharJogo(void)
{
    ClearBackground(BLACK);

    if (!jogoTerminado)
    {
        //Desenhar o background
        DrawTexture(backgroundTexture, 0, 0, WHITE);

        //Desenhar o jogador
        DrawRectangle(jogador.posicao.x - jogador.tamanho.x / 2, jogador.posicao.y - jogador.tamanho.y / 2, jogador.tamanho.x, jogador.tamanho.y, WHITE);

        //Desenhar os tijolos
        for (int i = 0; i < LINHAS_DE_TIJOLOS; i++)
        {
            for (int j = 0; j < TIJOLOS_POR_LINHA; j++)
            {
                if (tijolo[i][j].ativo)
                {
                    DrawRectangle(tijolo[i][j].posicao.x, tijolo[i][j].posicao.y, tamanhoDoTijolo.x, tamanhoDoTijolo.y, tijolo[i][j].cor);
                }
            }
        }

        DrawCircleV(bola.posicao, bola.raio, WHITE); //Desenhar a bola

        DrawText(TextFormat("VIDAS: %01i", jogador.vida), 10, 10, 20, WHITE); //Escrever no canto a vida
        DrawText(TextFormat("PONTUAÇÃO: %04i", score), 10, 30, 20, WHITE); //Escrever no canto a pontuação

        if (inicioTimer > 0) //Escrever o timer de inicio do jogo
        {
            DrawText(TextFormat("COMEÇA EM %.1f", inicioTimer), larguraDaTela / 2 - MeasureText(TextFormat("COMEÇA EM %.1f", inicioTimer), 40) / 2, alturaDaTela / 2 - 40, 40, WHITE);
        }

        if (pausa) DrawText("JOGO PAUSADO", larguraDaTela / 2 - MeasureText("JOGO PAUSADO", 40) / 2, alturaDaTela / 2 - 40, 40, WHITE); //Escrever a pausa no centro da tela
    }
    else DrawText("PREPARADO?! PRESSIONE [ENTER] PARA COMEÇAR O JOGO", larguraDaTela / 2 - MeasureText("PREPARADO?! PRESSIONE [ENTER] PARA COMEÇAR O JOGO", 20) / 2, alturaDaTela / 2 - 10, 20, WHITE);
}

void CarregarJogo(void) //Função para carregar as texturas do jogo
{
    for (int i = 0; i < NUM_FRAMES; i++)
    {
        UnloadTexture(frames[i]);
    }
    UnloadTexture(gameoverTexture);
    UnloadTexture(titleLogoTexture);
    UnloadTexture(backgroundTexture);
}

void AtualizarDesenharQuadro(void)
{
    switch (telaAtual)
    {
    case LOGO:
        AtualizarTelaInicial();
        DesenharTelaInicial();
        break;
    case TITULO:
        AtualizarTelaTitulo();
        DesenharTelaTitulo();
        break;
    case JOGANDO:
        AtualizarJogo();
        BeginDrawing();
        DesenharJogo();
        EndDrawing();
        break;
    case FIM:
        AtualizarTelaFim();
        DesenharTelaFim();
        break;
    default:
        break;
    }

    UpdateMusicStream(trilhaSonora); //Atualiza a música em todas as telas
}

void AtualizarTelaInicial(void)
{
    frameTimer += GetFrameTime();
    if (frameTimer >= frameTime)
    {
        frameTimer = 0.0f;
        currentFrame++;
        if (currentFrame >= NUM_FRAMES)
        {
            currentFrame = 0;
        }
    }

    logoScreenTime += GetFrameTime();
    if (logoScreenTime >= 5.0f)
    {
        telaAtual = TITULO;
    }
}

void DesenharTelaInicial(void)
{
    BeginDrawing();
    ClearBackground(WHITE);
    DrawTexture(frames[currentFrame], larguraDaTela / 2 - frames[currentFrame].width / 2, alturaDaTela / 2 - frames[currentFrame].height / 2, WHITE);
    DrawText("ESPERE POR 5 SEGUNDOS...", larguraDaTela / 2 - MeasureText("ESPERE POR 5 SEGUNDOS...", 20) / 2, alturaDaTela / 2 + frames[currentFrame].height / 2 + 20, 20, PURPLE);
    EndDrawing();
}

void AtualizarTelaTitulo(void)
{
    PlayMusicStream(trilhaSonora); //Inicia a música na tela de título
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        telaAtual = JOGANDO;
    }
}

void DesenharTelaTitulo(void)
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(titleLogoTexture, larguraDaTela / 2 - titleLogoTexture.width / 2, alturaDaTela / 2 - titleLogoTexture.height / 2, WHITE);
    DrawText("PRESSIONE [ENTER] PARA JOGAR", larguraDaTela / 2 - MeasureText("PRESSIONE [ENTER] PARA JOGAR", 20) / 2, alturaDaTela / 2 + titleLogoTexture.height / 2 + 20, 20, WHITE);
    EndDrawing();
}

void AtualizarTelaFim(void)
{
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        telaAtual = JOGANDO;
        IniciarJogo();
    }
}

void DesenharTelaFim(void)
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(gameoverTexture, larguraDaTela / 2 - gameoverTexture.width / 2, alturaDaTela / 2 - gameoverTexture.height / 2, WHITE);
    //DrawText("GAME OVER", larguraDaTela / 2 - MeasureText("GAME OVER", 40) / 2, alturaDaTela / 2 - 40, 40, WHITE);
    DrawText("PRESSIONE [ENTER] PARA JOGAR NOVAMENTE ou [ESC] PARA SAIR", larguraDaTela / 2 - MeasureText("PRESSIONE [ENTER] PARA JOGAR NOVAMENTE ou [ESC] PARA SAIR", 20) / 2, alturaDaTela / 2 + gameoverTexture.height / 2 + 20, 20, PURPLE);
    EndDrawing();
}
