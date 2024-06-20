#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

#define LARGURA 70
#define ALTURA 20
#define LARGURA_RAQUETE 6

int bola_x, bola_y, bola_dx, bola_dy;
int raquete_x;
int tijolo[LARGURA][ALTURA];
int pontuacao, vidas;

void irParaXY(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void definir_cor(int cor)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cor);
}

void iniciar_jogo()
{
    bola_x = LARGURA / 2;
    bola_y = ALTURA / 2;
    bola_dx = 1;
    bola_dy = -1;
    raquete_x = LARGURA / 2 - LARGURA_RAQUETE / 2;
    pontuacao = 0;
    vidas = 3;
    for (int i = 0; i < LARGURA; i++)
    {
        for (int j = 0; j < ALTURA / 2; j++)
        {
            tijolo[i][j] = 1;
        }
    }
}

void desenhar_bordas()
{
    for (int i = 0; i < LARGURA + 2; i++)
    {
        irParaXY(i, 0);
        printf("*");
        irParaXY(i, ALTURA + 1);
        printf("*");
    }
    for (int i = 0; i < ALTURA + 2; i++)
    {
        irParaXY(0, i);
        printf("*");
        irParaXY(LARGURA + 1, i);
        printf("*");
    }
}

void desenhar_bola()
{
    irParaXY(bola_x, bola_y);
    definir_cor(12);
    printf("O");
    definir_cor(7);
}

void desenhar_raquete()
{
    for (int i = 0; i < LARGURA_RAQUETE; i++)
    {
        irParaXY(raquete_x + i, ALTURA);
        printf("=");
    }
}

void limpar_raquete()
{
    for (int i = 0; i < LARGURA_RAQUETE; i++)
    {
        irParaXY(raquete_x + i, ALTURA);
        printf(" ");
    }
}

void desenhar_tijolos()
{
    for (int i = 0; i < LARGURA; i++)
    {
        for (int j = 0; j < ALTURA / 2; j++)
        {
            if (tijolo[i][j])
            {
                irParaXY(i + 1, j + 1);
                definir_cor(10);
                printf("#");
                definir_cor(7);
            }
        }
    }
}

void desenhar_estatisticas()
{
    irParaXY(1, ALTURA + 3);
    printf("Pontuacao: %d", pontuacao);
    irParaXY(LARGURA - 9, ALTURA + 3);
    printf("Vidas: %d", vidas);
}

void mover_bola()
{
    irParaXY(bola_x, bola_y);
    printf(" ");
    bola_x += bola_dx;
    bola_y += bola_dy;
    if (bola_x < 1 || bola_x > LARGURA)
    {
        bola_dx = -bola_dx;
    }
    if (bola_y < 1)
    {
        bola_dy = -bola_dy;
    }
    if (bola_y >= ALTURA)
    {
        vidas--;
        if (vidas == 0)
        {
            irParaXY(LARGURA / 2 - 5, ALTURA / 2);
            printf("Fim de Jogo");
            getch();
            exit(0);
        }
        else
        {
            bola_x = LARGURA / 2;
            bola_y = ALTURA / 2;
            bola_dx = 1;
            bola_dy = -1;
        }
    }
}

void mover_raquete()
{
    if (_kbhit())
    {
        char tecla = _getch();
        limpar_raquete();
        if (tecla == 'a' && raquete_x > 1)
        {
            raquete_x--;
        }
        if (tecla == 'd' && raquete_x < LARGURA - LARGURA_RAQUETE)
        {
            raquete_x++;
        }
        desenhar_raquete();
    }
}

void verificar_colisao()
{
    // Verificar colisão com a raquete
    if (bola_y == ALTURA - 1 && bola_x >= raquete_x && bola_x < raquete_x + LARGURA_RAQUETE)
    {
        bola_dy = -bola_dy;
    }

    // Verificar colisão com os tijolos
    if (tijolo[bola_x][bola_y])
    {
        tijolo[bola_x][bola_y] = 0;
        bola_dy = -bola_dy;
        pontuacao++;
    }
}

void exibir_como_jogar()
{
    system("cls");
    irParaXY(LARGURA / 2 - 7, ALTURA / 2 - 2);
    printf("COMO JOGAR:");
    irParaXY(LARGURA / 2 - 20, ALTURA / 2);
    printf("Use as teclas 'A' e 'D' para mover a raquete para esquerda e direita.");
    irParaXY(LARGURA / 2 - 20, ALTURA / 2 + 1);
    printf("Quebre todos os tijolos para ganhar o jogo.");
    irParaXY(LARGURA / 2 - 20, ALTURA / 2 + 2);
    printf("Nao deixe a bola cair abaixo da raquete.");
    irParaXY(LARGURA / 2 - 10, ALTURA / 2 + 4);
    printf("Pressione qualquer tecla para voltar ao menu.");
    getch();
}

void exibir_titulo()
{
    const char *titulo = "BREAKOUT JOGO";
    const char *cores[] = {
        "\x1b[31m", //Vermelho
        "\x1b[33m", //Amarelo
        "\x1b[32m", //Verde
        "\x1b[36m", //Ciano
        "\x1b[34m", //Azul
        "\x1b[35m", //Rosa
        "\x1b[37m"  //Branco
    };
    const int numCores = 7;

    for (int i = 0; titulo[i] != '\0'; i++)
    {
        printf("%s%c", cores[i % numCores], titulo[i]);
    }
    printf("\x1b[0m\n");
}

void exibir_menu()
{
    system("cls");
    irParaXY(LARGURA / 2 - 7, ALTURA / 2 - 2);
    exibir_titulo();
    irParaXY(LARGURA / 2 - 7, ALTURA / 2);
    printf("1. Jogar");
    irParaXY(LARGURA / 2 - 7, ALTURA / 2 + 1);
    printf("2. Como Jogar?");
    irParaXY(LARGURA / 2 - 7, ALTURA / 2 + 2);
    printf("3. Sair");
    irParaXY(LARGURA / 2 - 7, ALTURA / 2 + 4);
    printf("Escolha uma opcao: ");
}

int main()
{
    int escolha;
    while (1)
    {
        exibir_menu();
        escolha = _getch() - '0';
        if (escolha == 1)
        {
            system("cls");
            iniciar_jogo();
            desenhar_bordas();
            while (1)
            {
                desenhar_bola();
                desenhar_raquete();
                desenhar_tijolos();
                desenhar_estatisticas();
                mover_bola();
                mover_raquete();
                verificar_colisao();
                Sleep(100); // Controla a velocidade do jogo
            }
        }
        else if (escolha == 2)
        {
            exibir_como_jogar();
        }
        else if (escolha == 3)
        {
            exit(0);
        }
    }
    return 0;
}
