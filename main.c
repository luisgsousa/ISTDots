/*Este trabalho tem como objetivo desenvolver um jogo semelhante ao jogo Two Dots no âmbito da disciplina de programação do curso de MEEC.
**Trabalho efetuado por Luís Sousa - 90130 - 14/04/2018
*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX(a,b)    (((a)>(b))?(a):(b))
#define M_PI 3.14159265
#define STRING_SIZE 100       // max size for some strings
#define TABLE_SIZE 850        // main game space size
#define LEFT_BAR_SIZE 150     // left white bar size
#define WINDOW_POSX 200       // initial position of the window: x
#define WINDOW_POSY 200       // initial position of the window: y
#define SQUARE_SEPARATOR 8    // square separator in px
#define BOARD_SIZE_PER 0.7f   // board size in % wrt to table size
#define MAX_BOARD_POS 15      // maximum size of the board
#define MIN_BOARD_POS 5       //Tamanho mínimo do tabuleiro
#define MAX_COLORS 5
#define MARGIN 5
#define SQR(a) (a)*(a)
#define MAX_POINTS 99
#define MAX_MOVES 99
#define MAX_GAMES 1000

// declaration of the functions related to graphical issues
void InitEverything(int , int ,TTF_Font **, TTF_Font **, SDL_Surface **, SDL_Window ** , SDL_Renderer ** );
void InitSDL();
void InitFont();
SDL_Window* CreateWindow(int , int );
SDL_Renderer* CreateRenderer(int , int , SDL_Window *);
int RenderText(int, int, const char *, TTF_Font *, SDL_Color *, SDL_Renderer *);
int RenderLogo(int, int, SDL_Surface *, SDL_Renderer *);
int RenderTable(int, int, int [], TTF_Font *, SDL_Surface **, SDL_Renderer *, int _ncolors, int _npoints[], int _nmoves);
void ProcessMouseEvent(int , int , int [], int , int *, int * );
void RenderPoints(int [][MAX_BOARD_POS], int, int, int [], int, SDL_Renderer *);
void RenderStats( SDL_Renderer *_renderer, TTF_Font *_font, int _goals[], int _ncolors, int _gamemoves[MAX_GAMES], char _name[9], int _ndefeats, int _nvictories, int _game[MAX_GAMES]);
void filledCircleRGBA(SDL_Renderer * , int , int , int , int , int , int );
void printerror();

// definition of some strings: they cannot be changed when the program is executed !
const char myName[] = "Luis Sousa";
const char myNumber[] = "IST90130";
const int colors[3][MAX_COLORS] = {{91, 170, 54, 255, 253},{32, 240, 61, 174, 118},{94, 183, 105, 3, 144}};

/**
 * main function: entry point of the program
 * only to invoke other functions !
 */

int getpreferences(int variable, int max );
int randomcolor(int _ncolors);
int checkelements(int _board[MAX_BOARD_POS][MAX_BOARD_POS], int _selected[MAX_BOARD_POS][MAX_BOARD_POS], int _previouselement_x, int _previouselement_y, int _selectedcolor, int currentelement );
int checkerror(int _selected[MAX_BOARD_POS][MAX_BOARD_POS], int _i, int _j);
int replaceelement(int _board[MAX_BOARD_POS][MAX_BOARD_POS], int _selected[MAX_BOARD_POS][MAX_BOARD_POS], int _i, int _j, int _ncolors, int _board_pos_x, int _board_pos_y);
int checksquares(int _selected[MAX_BOARD_POS][MAX_BOARD_POS], int _board[MAX_BOARD_POS][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y, int _selectedcolor, int _j, int _i);
int PrintVictory(int _board_size_px[2], SDL_Renderer* _renderer, TTF_Font *_font);
int PrintDefeat(int _board_size_px[2], SDL_Renderer* _renderer, TTF_Font *_font);
void PrintShuffle(int _board_size_px[2], SDL_Renderer* _renderer, TTF_Font *_font);
int PossibleMoves(int _board[MAX_BOARD_POS][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y);

int main( void )
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *serif = NULL;
    SDL_Surface *imgs[2];
    SDL_Event event;
    int delay = 300;
    int quit = 0;
    int width = (TABLE_SIZE + LEFT_BAR_SIZE);
    int height = TABLE_SIZE;
    int square_size_px = 0, board_size_px[2] = {0};
    int board_pos_x = 0, board_pos_y = 0;
    int board[MAX_BOARD_POS][MAX_BOARD_POS] = {{-1}};
    int pt_x = 0, pt_y = 0;
    int ncolors = 0;                                        //Número de cores no tabuleiro
    int maxmoves = 0, nmoves = 0;                           //Número máximo de jogadas e número de jogadas restantes
    int maxpoints[5] = {0}, npoints[5] = {0};               //Número máximo de pontos (objetivo) e número restante de pontos a obter
    int i = 0;
    int j = 0;
    int selected[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}};     //Quadrados selecionados pelo utilizador que satisfazem as condições para serem substituídos
    int selectedcolor = 0;                                  //Cor do primeiro elemento selecionado
    int previouselement_x = 0, previouselement_y = 0;       //Coordenadas do elemento anteriormente selecionado
    int mouseclick = 0;                                     //Variável que toma o valor 1 quando o rato está a ser clicado e 0 se não
    int countpoints = 0;                                    //Variável que conta o número de elemntos substituídos na jogada presente
    char name[9] = "\0";                                    //Nome do utilizador
    int victory = 0, flag_v = 0;                            //Flag que assinala a vitória e flag usada para a verificação da vitória
    int defeat = 0;                                         //Flag que assinala a derrota
    TTF_Font *bigserif = NULL;                              //Font iguakl à declarada acima mas com tamanho maior
    int game[MAX_GAMES] = {-1};                             //Vetor em que para cada elemento correspondente a um jogo faz corresponder uma vitória se for 1 e uma derrota se for 0
    int gamemoves[MAX_GAMES] = {0};                         //Vetor que para cada elemento correspondente a um jogo guarda o número de jogadas feitas esse jogo
    int ndefeats = 0, nvictories = 0, ngames = 0;           //Número de derrotas, vitórias e jogos efetuados em relação a um jogador um jogador
    int savedboard[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}};   //Tabuleiro da jogada anterior utilizado para o undo
    int savedmoves = 0;                                     //Número de jogadas restantes da jogada anterior
    int savedpoints[5] = {0};                               //Objetivos na jogada anterior


    srand(1234);

//Lê o username que o jogardor insere
    printf("Username: ");
    fgets(name, sizeof(name), stdin);

    printf("-> Insert the following preferences: \n");

/*Lê ambas as dimensões do tabuleiro no formato nxm, e se a entrada não estiver nos parametros estabelecidos volta a imprimir a mensagem e a ler o parâmetro, repetindo isto até ser
introduzida uma entrada válida*/
    do
    {
        printf("Game tray size between 5x5 and 15x15 in format s1xs2: ");
        scanf("%dx%d", &board_pos_x, &board_pos_y);
        if(board_pos_x < MIN_BOARD_POS || board_pos_y < MIN_BOARD_POS || board_pos_x > MAX_BOARD_POS || board_pos_y > MAX_BOARD_POS)
        {   printerror();}
    }
    while(board_pos_x < MIN_BOARD_POS || board_pos_y < MIN_BOARD_POS || board_pos_x >MAX_BOARD_POS || board_pos_y > MAX_BOARD_POS);

//Lê o número máximo de cores através da função getpreferences
    printf("-> Number of colours up to 5 : ");
    ncolors = getpreferences(&ncolors, MAX_COLORS);

//Lê o objetivo respetivo a cada cor para as cores até ncolors, através da função getpreferences
    for(i=0; i<ncolors; i++)
    {
        printf("-> Maximum number of points for color %d(<99): ", i);
        maxpoints[i] = getpreferences(&maxpoints, MAX_POINTS);
    }

//Lê o número máximo de jogadas através da função getpreferences
    printf("-> Maximum number of moves (<99): ");
    maxmoves = getpreferences(&maxmoves, MAX_MOVES);

//Gera um número aleatório de 0 a ncolors que irá corresponder a uma cor, fazendo isto para todos os elementos do tabuleiro de jogo
for(i=0; i<board_pos_y; i++)
{
    for(j=0; j<board_pos_x; j++)
    {
        board[j][i] = rand() % ncolors;
    }
}
for(i=0; i<ncolors; i++)
{   npoints[i] = maxpoints[i];}
nmoves = maxmoves;

    // initialize graphics
    InitEverything(width, height,&bigserif , &serif, imgs, &window, &renderer);

    while( quit == 0 )
    {
        // while there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            if( event.type == SDL_QUIT )
            {
                if(victory == 0 && defeat == 0)                        //Quando o jogador clica na cruz para fechar o jogo e não estiver com o ecrã de vitória ou derrota(ou seja, a meio de um jogo)
                {                                                      //o jogo em que se encontrava é registado como derrota no vetpr game na posição referente a esse jogo
                    ndefeats++;                                        // e é aumentando o número de derrotas
                    game[ngames] = 0;
                }
                RenderStats(renderer, serif, npoints, ncolors, gamemoves, name, ndefeats, nvictories, game);    //Depois é escrito o ficheiro com a estatísticas dos jogos feitos pelo jogador
                quit = 1;                                                                                       //e sai do programa
            }
            else if ( event.type == SDL_KEYDOWN )
            {
                switch ( event.key.keysym.sym )
                {
                    case SDLK_n:
                        for(i=0; i<board_pos_y; i++)                         //Quando a tecla n é clicada o tabuleiro é gerado aleatoriamente e todas as posições do tabuleiro
                        {                                                    //são instantaneamente colocadas a 0 no vetor 2D selected, deixando de ser consideradas selecionadas
                            for(j=0; j<board_pos_x; j++)
                            {
                               board[j][i] = rand() % ncolors;
                               selected[j][i] = 0;
                            }
                        }
                        if(victory == 0 && defeat == 0)                      // Se não estiver no ecrã de derrota ou vitória, ou seja se estiver a meio de um jogo, é considerada uma derrota
                        {
                            ndefeats++;
                            game[ngames] = 0;
                        }
                        countpoints = 0;                                     //O número de elementos selecionados nesta jogada passa a ser 0
                        for(i=0; i<ncolors; i++)
                        {   npoints[i] = maxpoints[i];}                      //Os objetivos voltam ao seus valores iniciais
                        nmoves = maxmoves;                                   //O número de jogadas restantes volta ao seu valor inicial
                        victory = 0;                                         //Como se passa a tratar de um jogo novo tanto a flag que sinaliza a vitória como a que sinaliza a derrota
                        defeat = 0;                                          //tomam o valor 0
                        break;
                    case SDLK_q:                                             //Quando o jogador clica na tecla q acontece o mesmo que quando clica na cruz para fechar o jogo
                        if(victory == 0 && defeat == 0)
                        {
                        ndefeats++;
                        game[ngames] = 0;
                        }
                        RenderStats(renderer, serif, npoints, ncolors, gamemoves, name, ndefeats, nvictories, game);
                        quit = 1;
                        break;
                    case SDLK_u:
                        if(nmoves < maxmoves && victory == 0 && defeat == 0)  //Condição para só ocorrer undo quando já tenha sido feita pelo menos 1 jogada w
                        {
                            for(i=0; i<board_pos_y; i++)
                            {
                                for(j=0; j<board_pos_x; j++)
                                {   board[j][i] = savedboard[j][i];}
                            }

                            nmoves = savedmoves;

                            for(i=0; i<ncolors; i++)
                            {   npoints[i] = savedpoints[i];}
                        }
                            break;
                    default:
                        break;
                }
            }
/*  A seleção de elementos é feita da seguinte forma: quando o rato é pressionado, se já não houver elementos selecionados, o elemento onde se encontra é posto a 1 no vetor 2D selected,
**são guardadas as coordenadas dessa posição e a cor referente a esse elemento e a variável mouseclick é colocada a 1 para indicar que o rato está a ser clicado e que a jogada está a ser feita.
**  Quando o rato se move, se mouseclick for igual a 1 e se o rato se encontrar em cima de um elemento (dentro de um círculo) e se checkelements for igual a 1, a variável selected referente
**a essa posição é colocada a 1, e guarda as coordenadas desse elemento.
**  Isto é repetido até o botão do rato ser largado, onde acontecerá o mesmo que acontece quando o rato é movido e a variável mouseclick é colocada a 0.
*/
            else if ( event.type == SDL_MOUSEBUTTONDOWN && victory == 0 && defeat == 0 )
            {
                ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y);
                printf("Button down: %d %d\n", pt_x, pt_y);
                if(checkzero(selected, board_pos_x, board_pos_y) == 1)
                {
                    mouseclick = 1;                         //Variável colocada a 1 para sinalizar que o rato está a ser clicado, efetuando-se uma jogada
                    selected[pt_x][pt_y] = 1;               //Assinala o primeiro elemento a ser substituído
                    selectedcolor = board[pt_x][pt_y];      //O numero que representa a cor que será selecionada esta jogada é guardada na variável selectedcolor
                    previouselement_x = pt_x;               //Coordenada x deste elemento guardada para comnaração com o próximo
                    previouselement_y = pt_y;               //Coordenada y deste elemento guardada para comnaração com o próximo
                }

            }
            else if ( event.type == SDL_MOUSEBUTTONUP )
            {
                ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y);
                printf("Button up: %d %d\n", pt_x, pt_y);
                mouseclick = 0;                         //A variável é igualada a 0 para sinalizar que a jogada terminou
/*Verifica se a jogada feita resultará em substituição dos elementos, vendo se a função checkelements é 1, sendo cumpridas as condições necessárias,
e também se o elemento não foi já selecionado pois isto podia causar problemas com a contagem dos elementos selecionados*/
                if(checkelements(board, selected, previouselement_x, previouselement_y, selectedcolor, board[pt_x][pt_y]) == 1 && selected[pt_x][pt_y] == 0)
                {
                    selected[pt_x][pt_y] = 1;
                }
                previouselement_x = pt_x;
                previouselement_y = pt_y;
            }
            else if ( event.type == SDL_MOUSEMOTION )
            {
                ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y);
                printf("Moving Mouse: %d %d\n", pt_x, pt_y);

                if(mouseclick == 1 && pt_x != -1 && pt_y != -1) //Condição que verifica se o rato está a ser clicado enquanto se move e se o rato se encontra dentro do
                {
                    if(checkelements(board, selected, previouselement_x, previouselement_y, selectedcolor, board[pt_x][pt_y]) == 1 && selected[pt_x][pt_y] == 0)
                    {
                        selected[pt_x][pt_y] = 1;
                    }
                    previouselement_x = pt_x;                   //São guardadas as 2 coordenadas desta posição para comparação com o próximo elemento
                    previouselement_y = pt_y;
                }
            }
        }

//O shuffle não foi implementado pois a função PossibleMoves, que supostamente retornava 1 se houvessem jogadas possíveis, retorna sempre 1
//Mas aqui se encontra a parte do código que supostamente trocaria as cores dos elementos do tabuleiro aleatoriamente e imprimia uma mensagem de shuffle
//        int n = 0, m = 0;
//        int color = 0;
//        if(PossibleMoves(board, board_pos_x, board_pos_y) == 0)
//        {
//            for(j=0; j<board_pos_x; j++)
//            {
//                for(i=0; i<board_pos_y; i++)
//                {
//                    n = rand() % board_pos_x;
//                    m = rand() % board_pos_y;
//
//                    board[j][i] = color;
//                    board[j][i] = board[n][m];
//                    board[n][m] = color;
//                }
//            }
//            PrintShuffle(board_size_px, renderer, serif);
//            SDL_Delay(2000);
//        }


//Quando o jogador faz uma jogada, são guardados os objetivos, o número de jogadas restantes e o tabuleiro anteriores à jogada para ser usado no undo
    if(mouseclick ==1 && countselected(selected, board_pos_x, board_pos_y) > 1)
    {
        for(i=0; i<board_pos_y; i++)
        {
            for(j=0; j<board_pos_x; j++)
            {   savedboard[j][i] = board[j][i];}
        }

        savedmoves = nmoves;

        for(i=0; i<ncolors; i++)
        {   savedpoints[i] = npoints[i];}
    }
//Verifica se os elementos selecionados formam um quadrado ou um retângulo através da função checkquares e, se sim, os elementos da mesma côr e os elementos dentro da figura
//são colocados a -1 no vetor 2D board, o que fará com que os pontos não sejam rendered. Para cada um destes pontos aumenta o valor de countpoints no valor de 1 para contar os pontos removidos.
    if(mouseclick == 0)
    {
        for(i=0; i<board_pos_y; i++)
        {
            for(j=0; j<board_pos_x; j++)
            {
              if(checksquares(selected, board, board_pos_x, board_pos_y, selectedcolor, j, i) == 1)
              {
                  board[j][i] = -1;
                  countpoints++;
              }
            }
        }
    }

//Remove os pontos selecionados pelo jogador que para além de satisfazerem a função checkelements, thambém satisfazem a função checkerror, que verifica se não há elementos selecionados isolados.
//Isto só é feito enquando mouseclick=0, para não o fazer enquanto a jogada está a ser feita
    if(mouseclick == 0)
    {
        for(i=0; i<board_pos_y; i++)
        {
            for(j=0; j<board_pos_x; j++)
            {
                if(checkerror(selected, i, j) == 1)
                {
                    selected[i][j] = 0;
                }
                if(selected[j][i] == 1 && checkerror(selected, i, j) == 0)
                    {board[j][i] = -1;}
            }
        }
    }

//Parte da função que conta o número de pontos removidos e atualiza os objetivos e o número as jogadas restante, se o número de elemento a ser removidos for maior que 1
//(para só o fazer quando uma jogada foi feita)
    if(countselected(selected, board_pos_x, board_pos_y) > 1)
    {
        countpoints = countselected(selected, board_pos_x, board_pos_y);
        if(mouseclick == 0)
        {
            npoints[selectedcolor] -= countpoints;                          //Subtrai o número de pontos removidos ao objetivo da cor correspondente

            if(npoints[selectedcolor] < 0)                                  //Se o objetivo tomar um valor negativo por excesso de pontos removidos dessa cor o valor é posto a 0
            {   npoints[selectedcolor] = 0;}

            nmoves--;                                                       //O número de jogadas restantes é decremenetado no valor de 1

            if(nmoves < 0)                                                  //Se o número de jogadas restantes tomar um valor negativo por excesso de pontos removidos dessa cor o valor é posto a 0
            {   nmoves = 0;}
        }
    }

//O tabuleiro é atualizado através da função replaceelements que substituirá os elementos que devem ser substituídos e mantém os restantes iguais
    for(i=board_pos_y-1; i>=0; i--)
    {
        for(j=0; j<board_pos_x; j++)
        {   board[j][i] = replaceelements(board, i, j, ncolors, board_pos_x, board_pos_y);}
    }

    if(mouseclick == 0)
    {   for(i=0; i<board_pos_y; i++)
        {
            for(j=0; j<board_pos_x; j++)
            {
                selected[j][i] = 0;
                selectedcolor = -1;
                countpoints = 0;
            }
        }
    }

//flag que sinaliza a vitória é igualada a 1, mas se não satisfizer as condições para a vitória, que são que todos os objetivos das cores tenham sido cumpridos,
//será colocada a zero imediatamente a seguir
    flag_v = 1;
    for(i=0; i<ncolors; i++)
    {
        if(npoints[i] != 0)
        {   flag_v = 0;}
    }
    if(flag_v == 1 && victory == 0)                 //condição que verifica se a flag de vitória é igual a 1 e se a vitória já foi alcançada(para só contar 1 vitória)
    {
        gamemoves[ngames] = maxmoves - nmoves;      //guarda o número de jogadas feitas neste jogo
        victory = 1;                                //Flag que ativará a mensagem de vitória é igualada a 1
        nvictories++;                               //Incrementa o número de vitórias
    }

    if(nmoves == 0 && victory == 0 && defeat == 0)  //Se o número de jogadas for 0 e não tiver sido alcançada a vitória (pois ainda pode ocorrer vitória na última jogada) ou a derrota
    {
        gamemoves[ngames] = maxmoves - nmoves;      //Igual à secção acima
        defeat = 1;
        ndefeats++;
    }

        // render game table
        square_size_px = RenderTable( board_pos_x, board_pos_y, board_size_px, serif, imgs, renderer, ncolors, npoints, nmoves);
        // render board
        RenderPoints(board, board_pos_x, board_pos_y, board_size_px, square_size_px, renderer);
        // render in the screen all changes above

//Se a flag respetiva tiver o valor de 1, a mensagem de vitória/derrota é exibida e o jogo é registado como vitória(1)/derrota(0) no vetor game na posição do jogo corrente (ngames)
        if(defeat == 1)
        {
            game[ngames] = 0;
            PrintDefeat(board_size_px, renderer, bigserif);
        }
        if(victory == 1)
        {
            game[ngames] = 1;
            PrintVictory(board_size_px, renderer, bigserif);
        }

        ngames = nvictories + ndefeats;

        SDL_RenderPresent(renderer);
        // add a delay
        SDL_Delay( delay );

    }

    // free memory allocated for images and textures and closes everything including fonts
    TTF_CloseFont(serif);
    SDL_FreeSurface(imgs[0]);
    SDL_FreeSurface(imgs[1]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

//Função que faz a leitura das variável até ser introduzida uma entrada válida, sendo o requisito que ela seja maior que 0, menor que a entrada max e que seja um inteiro
int getpreferences(int variable, int max)
{
    int k = 0;
    do
    {
        k = scanf("%d", &variable);
        if(variable < 1 || variable > max)
        {   printerror();}
    }
    while (variable < 1 || variable > max || k != 1);
    return variable;
}

//Função que imprime mensagem de erro
void printerror()
{   printf("Error: Invalid input\n");}

//Função que retorna 1 se houver jogadas possíveis e 0 se não
int PossibleMoves(int _board[MAX_BOARD_POS][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y)
{
    int a = 0, b = 0;

    for(a=0; a < _board_pos_x; a++)
    {
        for(b=0; b<_board_pos_y; b++)
        {
            if(a != 0 && b != 0 && (_board[a][b] == _board[a+1][b] || _board[a][b] == _board[a][b+1] || _board[a][b] == _board[a-1][b] || _board[a][b] == _board[a][b-1]))
            {   return 1;}
            else if( a == 0 && b == 0 && (_board[a][b] == _board[a+1][b] || _board[a][b] == _board[a][b+1]))
            {   return 1;}
            else if(a == 0 &&(_board[a][b] == _board[a+1][b] || _board[a][b] == _board[a][b+1] || _board[a][b] == _board[a][b-1]))
            {   return 1;}
            else if(b == 0 && (_board[a][b] == _board[a+1][b] || _board[a][b] == _board[a][b+1] || _board[a][b] == _board[a-1][b]))
            {   return 1;}
        }
    }

    return 0;
}

//Função que conta o número de elementos selecionados
int countselected(int _selected[MAX_BOARD_POS][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y)
{
    int nselected = 0;
    int i = 0, j = 0;
    for(i=0; i<_board_pos_y; i++)
    {
        for(j=0; j<_board_pos_x; j++)
        {
            if(_selected[j][i] == 1)
            {   nselected++;}
        }
    }
    return nselected;
}

/*Função que verifica se o elemento respetivo à posição atual do rato e o elemento anterior  são compatíveis para serem selecionados, ou seja, se têm a mesma cor
e se o elemento anterior foi selecionado(pois só pode ser selecionado um elemento se um, adjacente tenha sido selecionado)*/
int checkelements(int _board[MAX_BOARD_POS][MAX_BOARD_POS], int _selected[MAX_BOARD_POS][MAX_BOARD_POS], int _previouselement_x, int _previouselement_y, int _selectedcolor, int currentelement )
{
    if((currentelement == _selectedcolor) && (_selected[_previouselement_x][_previouselement_y] == 1))
    {   return 1;}
    else
    {   return 0;}
}

/*Função que verifica se um elemento selecionado está rodeado de elementos não selecionados, e se estiver retorna 1, que corresponde a erro o que fará com que não seja selecionado.
Isto impede que aconteçam erros como elementos na diagonal ou elementos sozinhos serem destruídos*/
int checkerror(int _selected[MAX_BOARD_POS][MAX_BOARD_POS], int _i, int _j)
{
    if(_j != 0 && _i != 0 && _selected[_j][_i] == 1 && _selected[_j+1][_i] == 0 && _selected[_j][_i+1] == 0 && _selected[_j-1][_i] == 0 && _selected[_j][_i-1] == 0)
    {   return 1;}
    else if(_j == 0 && (_selected[_j][_i] == 1 && _selected[_j+1][_i] == 0 && _selected[_j][_i+1] == 0 && _selected[_j][_i-1] == 0))
    {   return 1;}
    else if(_i == 0 && (_selected[_j][_i] == 1 && _selected[_j+1][_i] == 0 && _selected[_j][_i+1] == 0 && _selected[_j-1][_i] == 0))
    {   return 1;}
    else
    {   return 0;}
}

//Função que verifica não há nenhum elemento selecionado, returnando o valor 0 se houver pelo menos 1
int checkzero(int _selected[MAX_BOARD_POS][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y)
{
    int i = 0, j = 0;

    for(i=0; i<_board_pos_y; i++)
    {
        for(j=0; j<_board_pos_x; j++)
        {
            if( _selected[j][i] == 1)
            {   return 0;}
        }
    }
    return 1;
}

//Função que substitui os elementos do tabuleiro de jogo, fazendo os elementos que assim devem "cair"
int replaceelements(int _board[MAX_BOARD_POS][MAX_BOARD_POS], int _i, int _j, int _ncolors, int _board_pos_x, int _board_pos_y)
{
    int a = 0, b = 0;
    int flag1 = 0, flag2 = 0;
    int nempty = 0;

//conta o número de elementos sem bola
    for(a=0; a<_board_pos_y; a++)
    {
        for(b=0; b<_board_pos_x; b++)
        {
            if(_board[b][a] == -1)
            {   nempty++;}
        }
    }

/*
Se não houver nenhum elemento vazio(sem bola), nenhum elemento é substituído, mantendo-se todas as cores iguais.
Se houver, a substituição é feita da seguinte forma:
-Se um elemento estiver vazio e estiver na linha 0, é-lhe atribuída uma cor aleatória
-Se um elemento estiver vazio, não pertencer à linha 0 e tiver um elemento não vazio em cima dele, a sua cor passa a ser a do elemento a cima e o elemento de cima passa
 a estar vazio, trocando-se os elementos. Isto vai ser feito de cima para baixo
 -Se um elemento estiver vazio, não pertencer à linah 0 e tiver um elemento vazio acima dele, mantém-se vazio
 -Se um elemento não estiver vazio, a sua cor mantém-se. Isto acontece, pois, visto que a função vai ser aplicada à matriz de baixo para cima, nunca vai haver um
elemento não vazio com um elemento vazio por baixo, pois já terá sido trocado com o vazio.
Como esta função é chamada repetidamente e esta função só faz os elementos descer uma "casa", isto fará com que os elementos parecam cair
*/
    if(nempty > 0)
    {
        if(_board[_j][_i] == -1)
        {
            if(_i == 0)
                {   _board[_j][_i] = rand() % _ncolors;}
            else
            {
                if(_board[_j][_i-1] != -1)
                {
                    _board[_j][_i] = _board[_j][_i-1];
                    _board[_j][_i-1] = -1;
                }
                else
                {   _board[_j][_i] = -1;}
            }

        }
        else
        {   _board[_j][_i] = _board[_j][_i];}
    }

    return _board[_j][_i];
}

/*
    Função que verifica a existência de quadrados/retângulos selecionados pelo jogador da seguinte forma:
    -Repetindo este processo para todos os elementos do tabuleiro à exceção dos limites baixo e direito do tabuleiro, verifica se o elemento em questão foi selecionado.
    -Se sim, mantém a coordenada y e vai aumentando a coordenada x 1 a 1, verificando se o elemento a seguir foi selecionado, quando chegar a um elemento onde o seguinte não o foi ou à ponta do
 tabuleiro, pára o ciclo,significando que se houver um quadrado/retângulo, esta aresta acaba nesse elemento.
    -Repete este processo com coordenada y a variar e obtém a outra aresta
    -Depois, começando no elemento final da aresta que se extende no eixo do y, faz o mesmo processo, variando o x, até chegar a um elemento onde o seguinte não foi selecionado ou até a coordenada x
ser igual à da aresta paralela a esta , se chegar a esta coordenada significa que pode haver um quadradp/retângulo
    -Repete este processo começando no fim da aresta que se extende no eixo do y
    -Calcula o comprimento das arestas no eixo do x e do y
    -Se ambos os comprimentos forem maiores que 1 significa que há quadrado/retângulo, logo coloca-se uma condição para verificar se isto se verifica e se o elemento correspondente às
coordenadas de entrada _j e _i é da mesma cor ou está dentro da figura
*/
int checksquares(int _selected[MAX_BOARD_POS][MAX_BOARD_POS], int _board[MAX_BOARD_POS][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y, int _selectedcolor, int _j, int _i)
{
    int a = 0, b = 0, c = 0, d = 0;
    int x_start = 0, y_start = 0, x_end = 0, y_end = 0;
    int x_lenght = 0, y_lenght = 0;

    for(d=0; d<_board_pos_y; d++)
    {
        for(c=0; c<_board_pos_x; c++)
        {
            x_start = c;
            y_start = d;

            if(_selected[c][d] == 1 && x_start != _board_pos_x - 1 && d != _board_pos_y - 1)
            {
                for(a=x_start; a<_board_pos_x;a++)
                {
                    if(_selected[a+1][y_start] == 0)
                    {   break;}
                }
                for(b=y_start; b<_board_pos_y; b++)
                {
                    if(_selected[x_start][b+1] == 0)
                    {   break;}
                }

                for(x_end=x_start; x_end<_board_pos_x; x_end++)
                {
                    if(_selected[x_end+1][b] == 0 || x_end == a || x_end == _board_pos_x-1)
                    {   break;}
                }
                for(y_end=y_start; y_end<_board_pos_y; y_end++)
                {
                    if(_selected[a][y_end+1] == 0 || y_end == b || y_end == _board_pos_y - 1)
                    {   break;}
                }

                x_lenght = x_end - x_start + 1;
                y_lenght = y_end - y_start + 1;

                if(x_lenght > 1 && y_lenght > 1 && (_j > x_start && _j < x_end && _i > x_start && _i < y_end || _selectedcolor == _board[_j][_i]))
                {
                    return 1;
                }
            }
        }
    }


    return 0;
}

//Função que imprime mensagem de vitória
int PrintVictory(int _board_size_px[2], SDL_Renderer* _renderer, TTF_Font *_font)
{
    SDL_Rect background;
    SDL_Color white = {255, 255, 255};
    char message[9] = "VICTORY!";

    background.x = ((TABLE_SIZE - _board_size_px[0]) >> 1) + 55;
    background.y = TABLE_SIZE / 2;
    background.w = 485;
    background.h = 120;

    SDL_SetRenderDrawColor(_renderer, 0, 200, 0, 255);
    SDL_RenderFillRect(_renderer, &background);
    RenderText(background.x + 5, background.y - 15, message, _font, &white, _renderer);

    return 1;
}

//Função que imprime mensagem de derrota
int PrintDefeat(int _board_size_px[2], SDL_Renderer* _renderer, TTF_Font *_font)
{
    SDL_Rect background;
    SDL_Color white = {255, 255, 255};
    char message[8] = "DEFEAT!";

    background.x = ((TABLE_SIZE - _board_size_px[0]) >> 1) + 85;
    background.y = TABLE_SIZE / 2;
    background.w = 420;
    background.h = 120;

    SDL_SetRenderDrawColor(_renderer, 200, 0, 0, 255);
    SDL_RenderFillRect(_renderer, &background);
    RenderText(background.x + 5, background.y - 15, message, _font, &white, _renderer);

    return 1;
}

//Função que imprime mensagem de shuffle
void PrintShuffle(int _board_size_px[2], SDL_Renderer* _renderer, TTF_Font *_font)
{
    SDL_Rect background;
    SDL_Color white = {255, 255, 255};
    char message[10] = "SHUFFLING";

    background.x = ((TABLE_SIZE - _board_size_px[0]) >> 1) + 85;
    background.y = TABLE_SIZE / 2;
    background.w = 460;
    background.h = 120;

    SDL_SetRenderDrawColor(_renderer, 0, 0, 150, 255);
    SDL_RenderFillRect(_renderer, &background);
    RenderText(background.x + 5, background.y - 15, message, _font, &white, _renderer);
}

/**
 * ProcessMouseEvent: gets the square pos based on the click positions !
 * \param _mouse_pos_x position of the click on pixel coordinates
 * \param _mouse_pos_y position of the click on pixel coordinates
 * \param _board_size_px size of the board !
 * \param _square_size_px size of each square
 * \param _pt_x square nr
 * \param _pt_y square nr
 */
void ProcessMouseEvent(int _mouse_pos_x, int _mouse_pos_y, int _board_size_px[], int _square_size_px,
        int *_pt_x, int *_pt_y )
{
    int circleX = 0, circleY = 0, circleR = 0;
    int sq_x = 0, sq_y = 0;
    int dist = 0;

    // corner of the board
    int x_corner = (TABLE_SIZE - _board_size_px[0]) >> 1;
    int y_corner = (TABLE_SIZE - _board_size_px[1] - 15);



    // verify if valid cordinates
    if (_mouse_pos_x < x_corner || _mouse_pos_y < y_corner || _mouse_pos_x > (x_corner + _board_size_px[0])
        || _mouse_pos_y > (y_corner + _board_size_px[1]) )
    {
        *_pt_x = -1;
        *_pt_y = -1;
        return;
    }

    // computes the square where the mouse position is
//    _mouse_pos_x = _mouse_pos_x - x_corner;
//    _mouse_pos_y = _mouse_pos_y - y_corner;

    sq_x = (_mouse_pos_x - x_corner) / (_square_size_px + SQUARE_SEPARATOR);
    sq_y = (_mouse_pos_y - y_corner) / (_square_size_px + SQUARE_SEPARATOR);

    circleX = x_corner + (sq_x+1)*SQUARE_SEPARATOR + sq_x*(_square_size_px)+(_square_size_px>>1);
    circleY = y_corner + (sq_y+1)*SQUARE_SEPARATOR + sq_y*(_square_size_px)+(_square_size_px>>1);
    circleR = (int)(_square_size_px*0.4f);

    dist = (int)floor(sqrt(SQR(_mouse_pos_x - circleX) + SQR(_mouse_pos_y - circleY)));

    if(dist < circleR)
    {
        *_pt_x = sq_x;
        *_pt_y = sq_y;
    }
    else
    {
        *_pt_x = -1;
        *_pt_y = -1;
    }
}

/**
 * RenderPoints: renders the board
 * \param _board 2D array with integers representing board colors
 * \param _board_pos_x number of positions in the board (x axis)
 * \param _board_pos_y number of positions in the board (y axis)
 * \param _square_size_px size of each square
 * \param _board_size_px size of the board
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderPoints(int _board[][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y,
        int _board_size_px[], int _square_size_px, SDL_Renderer *_renderer )
{
    int clr, x_corner, y_corner, circleX, circleY, circleR;

    // corner of the board
    x_corner = (TABLE_SIZE - _board_size_px[0]) >> 1;
    y_corner = (TABLE_SIZE - _board_size_px[1] - 15);

    // renders the squares where the dots will appear
    for ( int i = 0; i < _board_pos_x; i++ )
    {
        for ( int j = 0; j < _board_pos_y; j++ )
        {
                // define the size and copy the image to display
                circleX = x_corner + (i+1)*SQUARE_SEPARATOR + i*(_square_size_px)+(_square_size_px>>1);
                circleY = y_corner + (j+1)*SQUARE_SEPARATOR + j*(_square_size_px)+(_square_size_px>>1);
                circleR = (int)(_square_size_px*0.4f);
                // draw a circle (except if the board has the number -1 associated with it)
                clr = _board[i][j];
                if(clr != -1)
                    {filledCircleRGBA(_renderer, circleX, circleY, circleR, colors[0][clr], colors[1][clr], colors[2][clr]);}
        }
    }
}
/**
 * filledCircleRGBA: renders a filled circle
 * \param _circleX x pos
 * \param _circleY y pos
 * \param _circleR radius
 * \param _r red
 * \param _g gree
 * \param _b blue
 */
void filledCircleRGBA(SDL_Renderer * _renderer, int _circleX, int _circleY, int _circleR, int _r, int _g, int _b)
{
    int off_x = 0;
    int off_y = 0;
    float degree = 0.0;
    float step = M_PI / (_circleR*8);

    SDL_SetRenderDrawColor(_renderer, _r, _g, _b, 255);

    while (_circleR > 0)
    {
        for (degree = 0.0; degree < M_PI/2; degree+=step)
        {
            off_x = (int)(_circleR * cos(degree));
            off_y = (int)(_circleR * sin(degree));
            SDL_RenderDrawPoint(_renderer, _circleX+off_x, _circleY+off_y);
            SDL_RenderDrawPoint(_renderer, _circleX-off_y, _circleY+off_x);
            SDL_RenderDrawPoint(_renderer, _circleX-off_x, _circleY-off_y);
            SDL_RenderDrawPoint(_renderer, _circleX+off_y, _circleY-off_x);
        }
        _circleR--;
    }
}


/*
 * Shows some information about the game:
 * - Goals of the game
 * - Number of moves
 * \param _renderer renderer to handle all rendering in a window
 * \param _font font to display the scores
 * \param _goals goals of the game
 * \param _ncolors number of colors
 * \param _moves number of moves remaining
 */

void RenderStats( SDL_Renderer *_renderer, TTF_Font *_font, int _goals[], int _ncolors, int _gamemoves[MAX_GAMES], char _name[9], int _ndefeats, int _nvictories, int _game[MAX_GAMES])
{
    int ngames = 0;
    int i = 0;
    ngames = _nvictories + _ndefeats;

    FILE *fp;
    fp = fopen(_name, "w");

    if(fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    fprintf(fp, "%s\n", _name);
    fprintf(fp, "%d games made: %d Victories, %d Defeats\n", ngames, _nvictories, _ndefeats);

    for(i=0; i<ngames; i++)
    {
        if(_game[i] == 1)
        {   fprintf(fp, "Game %d - Victory \nNumber of moves made: %d\n", i, _gamemoves[i]);}
        else
        {   fprintf(fp, "Game %d - Defeat \nNumber of moves made: %d\n", i, _gamemoves[i]);}
    }
    fclose(fp);
}

/*
 * RenderTable: Draws the table where the game will be played, namely:
 * -  some texture for the background
 * -  the right part with the IST logo and the student name and number
 * -  the grid for game board with squares and seperator lines
 * \param _board_pos_x number of positions in the board (x axis)
 * \param _board_pos_y number of positions in the board (y axis)
 * \param _board_size_px size of the board
 * \param _font font used to render the text
 * \param _img surfaces with the table background and IST logo (already loaded)
 * \param _renderer renderer to handle all rendering in a window
 */

int RenderTable( int _board_pos_x, int _board_pos_y, int _board_size_px[],
        TTF_Font *_font, SDL_Surface *_img[], SDL_Renderer* _renderer, int _ncolors, int _npoints[5], int _nmoves)
{
    SDL_Color black = { 0, 0, 0 }; // black
    SDL_Color light = { 205, 193, 181 };
    SDL_Color dark = { 120, 110, 102 };
    SDL_Texture *table_texture;
    SDL_Rect tableSrc, tableDest, board, board_square;
    SDL_Rect rectangle0, rectangle1, rectangle2, rectangle3, rectangle4, rectangle5;
    int height, board_size, square_size_px, max_pos;
    int k = 0, i = 0;
    char points[2] = "\0";

    // set color of renderer to some color
    SDL_SetRenderDrawColor( _renderer, 255, 255, 255, 255 );

    k = (TABLE_SIZE - 220) / _ncolors;

    // clear the window
    SDL_RenderClear( _renderer );

    tableDest.x = tableSrc.x = 0;
    tableDest.y = tableSrc.y = 0;
    tableSrc.w = _img[0]->w;
    tableSrc.h = _img[0]->h;
    tableDest.w = TABLE_SIZE;
    tableDest.h = TABLE_SIZE;

    // draws the table texture
    table_texture = SDL_CreateTextureFromSurface(_renderer, _img[0]);
    SDL_RenderCopy(_renderer, table_texture, &tableSrc, &tableDest);

    // render the IST Logo
    height = RenderLogo(TABLE_SIZE, 0, _img[1], _renderer);

    // render the student name
    height += RenderText(TABLE_SIZE+3*MARGIN, height, myName, _font, &black, _renderer);

    // this renders the student number
    RenderText(TABLE_SIZE+3*MARGIN, height, myNumber, _font, &black, _renderer);

    // compute and adjust the size of the table and squares
    max_pos = MAX(_board_pos_x, _board_pos_y);
    board_size = (int)(BOARD_SIZE_PER*TABLE_SIZE);
    square_size_px = (board_size - (max_pos+1)*SQUARE_SEPARATOR) / max_pos;
    _board_size_px[0] = _board_pos_x*(square_size_px+SQUARE_SEPARATOR)+SQUARE_SEPARATOR;
    _board_size_px[1] = _board_pos_y*(square_size_px+SQUARE_SEPARATOR)+SQUARE_SEPARATOR;

    // renders the entire board background
    SDL_SetRenderDrawColor(_renderer, dark.r, dark.g, dark.b, dark.a );
    board.x = (TABLE_SIZE - _board_size_px[0]) >> 1;
    board.y = (TABLE_SIZE - _board_size_px[1] - 15);
    board.w = _board_size_px[0];
    board.h = _board_size_px[1];
    SDL_RenderFillRect(_renderer, &board);

    // renders the squares where the numbers will appear
    SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a );

    // iterate over all squares
    for ( int i = 0; i < _board_pos_x; i++ )
    {
        for ( int j = 0; j < _board_pos_y; j++ )
        {
            board_square.x = board.x + (i+1)*SQUARE_SEPARATOR + i*square_size_px;
            board_square.y = board.y + (j+1)*SQUARE_SEPARATOR + j*square_size_px;
            board_square.w = square_size_px;
            board_square.h = square_size_px;
            SDL_RenderFillRect(_renderer, &board_square);
        }
    }

    rectangle5.x = 30;
    rectangle5.y = 100;
    rectangle5.w = 77;
    rectangle5.h = 40;
    SDL_RenderFillRect(_renderer, &rectangle5);
    SDL_SetRenderDrawColor(_renderer, 205, 193, 181, 255);
    sprintf(points, "Moves: %d", _nmoves);
    RenderText((rectangle5.x + 5), (rectangle5.y + 5), points, _font, &black, _renderer);

    for(i=0; i<_ncolors; i++)
    {
        switch(i)
        {
            case 0:
            {
                rectangle0.x = k/2 + 120;
                rectangle0.y = 100;
                rectangle0.w = 70;
                rectangle0.h = 40;
                SDL_SetRenderDrawColor(_renderer, 205, 193, 181, 255);
                SDL_RenderFillRect(_renderer, &rectangle0);
                filledCircleRGBA(_renderer, (rectangle0.x + 20), (rectangle0.y + 20), 15, colors[0][0], colors[1][0], colors[2][0]);
                sprintf(points, "%d", _npoints[0]);
                RenderText((rectangle0.x + 45), (rectangle0.y + 5), points, _font, &black, _renderer);
                break;
            }
            case 1:
            {
                rectangle1.x = k/2 + k + 120;
                rectangle1.y = 100;
                rectangle1.w = 70;
                rectangle1.h = 40;
                SDL_SetRenderDrawColor(_renderer, 205, 193, 181, 255);
                SDL_RenderFillRect(_renderer, &rectangle1);
                filledCircleRGBA(_renderer, (rectangle1.x + 20), (rectangle1.y  + 20), 15, colors[0][1], colors[1][1], colors[2][1]);
                sprintf(points, "%d", _npoints[1]);
                RenderText((rectangle1.x + 45), (rectangle1.y + 5), points, _font, &black, _renderer);
                break;

            }
            case 2:
            {
                rectangle2.x = k/2 + 2*k + 120;
                rectangle2.y = 100;
                rectangle2.w = 70;
                rectangle2.h = 40;
                SDL_SetRenderDrawColor(_renderer, 205, 193, 181, 255);
                SDL_RenderFillRect(_renderer, &rectangle2);
                filledCircleRGBA(_renderer, (rectangle2.x + 20), (rectangle2.y  + 20), 15, colors[0][2], colors[1][2], colors[2][2]);
                sprintf(points, "%d", _npoints[2]);
                RenderText((rectangle2.x + 45), (rectangle2.y + 5), points, _font, &black, _renderer);
                break;
            }
            case 3:
            {
                rectangle3.x = k/2 + 3*k + 120;
                rectangle3.y = 100;
                rectangle3.w = 70;
                rectangle3.h = 40;
                SDL_SetRenderDrawColor(_renderer, 205, 193, 181, 255);
                SDL_RenderFillRect(_renderer, &rectangle3);
                filledCircleRGBA(_renderer, (rectangle3.x + 20), (rectangle3.y  + 20), 15, colors[0][3], colors[1][3], colors[2][3]);
                sprintf(points, "%d", _npoints[3]);
                RenderText((rectangle3.x + 45), (rectangle3.y + 5), points, _font, &black, _renderer);
                break;
            }
            case 4:
            {
                rectangle4.x = k/2 + 4*k + 120;
                rectangle4.y = 100;
                rectangle4.w = 70;
                rectangle4.h = 40;
                SDL_SetRenderDrawColor(_renderer, 205, 193, 181, 255);
                SDL_RenderFillRect(_renderer, &rectangle4);
                filledCircleRGBA(_renderer, (rectangle4.x + 20), (rectangle4.y  + 20), 15, colors[0][4], colors[1][4], colors[2][4]);
                sprintf(points, "%d", _npoints[4]);
                RenderText((rectangle4.x + 45), (rectangle4.y + 5), points, _font, &black, _renderer);
                break;
            }
        }
    }

    // destroy everything
    SDL_DestroyTexture(table_texture);
    // return for later use
    return square_size_px;
}

/**
 * RenderLogo function: Renders the IST logo on the app window
 * \param x X coordinate of the Logo
 * \param y Y coordinate of the Logo
 * \param _logoIST surface with the IST logo image to render
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderLogo(int x, int y, SDL_Surface *_logoIST, SDL_Renderer* _renderer)
{
    SDL_Texture *text_IST;
    SDL_Rect boardPos;

    // space occupied by the logo
    boardPos.x = x;
    boardPos.y = y;
    boardPos.w = _logoIST->w;
    boardPos.h = _logoIST->h;

    // render it
    text_IST = SDL_CreateTextureFromSurface(_renderer, _logoIST);
    SDL_RenderCopy(_renderer, text_IST, NULL, &boardPos);

    // destroy associated texture !
    SDL_DestroyTexture(text_IST);
    return _logoIST->h;
}

/**
 * RenderText function: Renders some text on a position inside the app window
 * \param x X coordinate of the text
 * \param y Y coordinate of the text
 * \param text string with the text to be written
 * \param _font TTF font used to render the text
 * \param _color color of the text
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderText(int x, int y, const char *text, TTF_Font *_font, SDL_Color *_color, SDL_Renderer* _renderer)
{
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect solidRect;

    solidRect.x = x;
    solidRect.y = y;
    // create a surface from the string text with a predefined font
    text_surface = TTF_RenderText_Blended(_font,text,*_color);
    if(!text_surface)
    {
        printf("TTF_RenderText_Blended: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    // create texture
    text_texture = SDL_CreateTextureFromSurface(_renderer, text_surface);
    // obtain size
    SDL_QueryTexture( text_texture, NULL, NULL, &solidRect.w, &solidRect.h );
    // render it !
    SDL_RenderCopy(_renderer, text_texture, NULL, &solidRect);
    // clear memory
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
    return solidRect.h;
}


/**
 * InitEverything: Initializes the SDL2 library and all graphical components: font, window, renderer
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _font font that will be used to render the text
 * \param _img surface to be created with the table background and IST logo
 * \param _window represents the window of the application
 * \param _renderer renderer to handle all rendering in a window
 */
void InitEverything(int width, int height,TTF_Font **_biggerfont, TTF_Font **_font, SDL_Surface *_img[], SDL_Window** _window, SDL_Renderer** _renderer)
{
    InitSDL();
    InitFont();
    *_window = CreateWindow(width, height);
    *_renderer = CreateRenderer(width, height, *_window);

    // load the table texture
    _img[0] = IMG_Load("table_texture.png");
    if (_img[0] == NULL)
    {
        printf("Unable to load image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // load IST logo
    _img[1] = SDL_LoadBMP("ist_logo.bmp");
    if (_img[1] == NULL)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // this opens (loads) a font file and sets a size
    *_font = TTF_OpenFont("FreeSerif.ttf", 16);
    if(!*_font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    *_biggerfont = TTF_OpenFont("FreeSerif.ttf", 100);
    if(!*_biggerfont)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitSDL: Initializes the SDL2 graphic library
 */
void InitSDL()
{
    // init SDL library
    if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        printf(" Failed to initialize SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitFont: Initializes the SDL2_ttf font library
 */
void InitFont()
{
    // Init font library
    if(TTF_Init()==-1)
    {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * CreateWindow: Creates a window for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \return pointer to the window created
 */
SDL_Window* CreateWindow(int width, int height)
{
    SDL_Window *window;
    // init window
    window = SDL_CreateWindow( "ISTDots", WINDOW_POSX, WINDOW_POSY, width, height, 0 );
    // check for error !
    if ( window == NULL )
    {
        printf("Failed to create window : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return window;
}

/**
 * CreateRenderer: Creates a renderer for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _window represents the window for which the renderer is associated
 * \return pointer to the renderer created
 */
SDL_Renderer* CreateRenderer(int width, int height, SDL_Window *_window)
{
    SDL_Renderer *renderer;
    // init renderer
    renderer = SDL_CreateRenderer( _window, -1, 0 );

    if ( renderer == NULL )
    {
        printf("Failed to create renderer : %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // set size of renderer to the same as window
    SDL_RenderSetLogicalSize( renderer, width, height );

    return renderer;
}
