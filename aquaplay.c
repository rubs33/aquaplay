/*
 * Descricao: Programa que simula um AquaPlay
 * Autor: Rubens Takiguti Ribeiro
 * Versao: 1.1.0.0
 * Plataforma: GNU/Linux e Microsoft Windows
 * Data Criacao: 20/06/2006
 * Modificacao:  21/06/2006
 */

#include <allegro.h>
#include <math.h>

/* Constantes Configuraveis */
#define W           800               /* Largura da tela         */
#define H           600               /* Altura da tela          */
#define TAM_SETA    80                /* Tamanho da seta         */
#define MAX_GRAU    (90 + 35)         /* Maior grau              */
#define MIN_GRAU    (90 - 35)         /* Menor grau              */
#define MAX_FORCA   100               /* Maior forca             */
#define MIN_FORCA   0                 /* Menor forca             */
#define ALCANCE     30                /* Grau de alcance do jato */

/* Constantes Nao Configuraveis */
#define PI          3.14
#define DIREITA     1
#define ESQUERDA    2
#define CIMA        1
#define BAIXO       2

/* Discos do AquaPlay */
struct Disco {
    int x;         /* Coordenada X do disco */
    int y;         /* Coordenada Y do disco */
    int raiox;     /* Raio x do disco       */
    int raioy;     /* Raio y do disco       */
    int cor;       /* Cor do disco          */
};


/* Pinos do AquaPlay */
struct Pino {
    int x;         /* Coordenada X do pino */
    int y;         /* Coordenada Y do pino */
    int h;         /* Altura do pino       */
    int cor;       /* Cor do pino          */
};


/* Variaveis globais: coordenada da seta */
int xseta = (int)(W * 0.5);
int yseta = H;


/*
 * disco_no_pino: retorna se o disco esta' no pino
 */
char disco_no_pino(const struct Disco *disco, const struct Pino *pino) {

    /* Se o disco esta' na altura do pino */
    if (disco->y >= (H - pino->h)) {

        /* Se o disco esta' nas limitacoes de seu raio sobre o pino */
        if (abs(pino->x - disco->x) < disco->raiox) {
            return 1;
        }
    }
    return 0;
}


/*
 * alcance_jato: retorna se o disco esta na regiao de alcance do jato
 */
char alcance_jato(const struct Disco *disco, const int grau) {
    if (disco->y == (H - disco->raioy)) {
        return 1;
    }

    int dx = abs(xseta - disco->x);
    int dy = abs(yseta - disco->y);

    /* Raiz do quadrada dos catetos de um triangulo */
    int distancia = (int)(sqrt((dx * dx) + (dy * dy)));

    int MAX_X = xseta - (int)(distancia * cos((180 - grau + ALCANCE) * PI / 180));
    int MIN_X = xseta - (int)(distancia * cos((180 - grau - ALCANCE) * PI / 180));

    if ((disco->x <= MAX_X) && (disco->x >= MIN_X)) {
        return 1;
    }
    return 0;
}


/*
 * gravidade: faz um disco descer ate' o chao
 */
void gravidade(struct Disco *disco, const struct Pino *pino) {
    int MAX_Y = (H - disco->raioy);
    int randx = 1 - (rand() % 3);

    /* Se o disco nao chegou ao chao */
    if (disco->y < MAX_Y) {

        /* Descer o disco verticalmente */
        disco->y += 1;

        /* Se o disco esta' no pino */
        if (disco_no_pino(disco, pino)) {

            /* Mover o disco horizontalmente */
            disco->x += randx;

            /* Se o disco saiu do pino, entao trombou */
            if (!disco_no_pino(disco, pino)) {
                disco->x -= randx;
            }

        /* Se o disco nao esta' no pino */
        } else {

            /* Mover o disco horizontalmente */
            disco->x += randx;

            /* Se o disco entrou no pino, entao trombou */
            if (disco_no_pino(disco, pino)) {
                disco->x -= randx;
            }
        }

        /* Se passou da parede esquerda: ficar na parede esquerda*/
        if (disco->x < disco->raiox) {
            disco->x = disco->raiox;

        /* Se passou da parede direita: ficar na parede direita */
        } else if (disco->x > (W - disco->raiox)) {
            disco->x = (W - disco->raiox);
        }

    /* Se o disco esta' no chao ou passou dele */
    } else {
        disco->y = MAX_Y;
    }
}


/*
 * jato_dagua: faz uma forca sobre o disco
 */
void jato_dagua(struct Disco *disco, const struct Pino *pino, int *f, int *grau) {

    /* Verificar se o disco esta no angulo de alcance do jato d'agua */
    if ((!alcance_jato(disco, *grau)) && (disco->y < (H - disco->raioy))) {

        *f = (int)(*f * 0.9); /* Se nao esta', perde 10% de forca */
    }

    /* Decrementar altura do disco conforme forca aplicada */
    disco->y -= (int)(*f * sin(*grau * PI / 180));

    /* Se passou do teto: ficar no teto */
    if (disco->y < 0) {
        disco->y = 0;
    }

    /* Se passou da esquerda: ficar na esquerda e mudar o sentido da forca */
    if (disco->x < disco->raiox) {
        disco->x = disco->raiox;
        *grau = (int)((180 - *grau) * 1.1);

    /* Se passou da direita: ficar na direita e mudar o sentido da forca */
    } else if (disco->x > (W - disco->raiox)) {
        disco->x = (W - disco->raiox);
        *grau = (int)((180 - *grau) * 1.1);
    }

    /* Aplicar forca na componente X de acordo com a forca aplicada */
    if (disco_no_pino(disco, pino)) {
        disco->x += (int)(*f * cos(*grau * PI / 180));

        /* Se saiu do pino, entao trombou */
        if (!disco_no_pino(disco, pino)) {
            disco->x -= (int)(*f * cos(*grau * PI / 180));
        }

    /* Se nao estava no pino */
    } else {
        disco->x += (int)(*f * cos(*grau * PI / 180));

        /* Se entrou no pino, entao trombou */
        if (disco_no_pino(disco, pino)) {
            disco->x -= (int)(*f * cos(*grau * PI / 180));
        }
    }

    /* Perder 10% da forca aplicada */
    if (*f == (int)(*f * 0.9)) {
        *f -= 1;
    } else {
        *f = (int)(*f * 0.9);
    }
    if (*f < 0) {
        *f = 0;
    }
}


/*
 * desenhar_status: desenha o status do jogo
 */
void desenhar_status(BITMAP *buffer, const int grau, const int forca,
                     const char entrou, const int pontos) {
    textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255),
                  -1, "Grau: %d°", grau);
    textprintf_ex(buffer, font, 10, 10 + 5 + text_height(font),
                  makecol(255, 255, 255),
                  -1, "Forca: %d", forca);
    textprintf_ex(buffer, font, 10, 10 + 2 * (5 + text_height(font)),
                  makecol(255, 255, 255),
                  -1, "Entrou: %s", (entrou ? "Sim" : "Nao"));
    textprintf_ex(buffer, font, 10, 10 + 3 * (5 + text_height(font)),
                  makecol(255, 255, 255),
                  -1, "Pontos: %d", pontos);
    textout_ex(buffer, font, "Pressione Espaco para jogar",
               10, 10 + 5 * (5 + text_height(font)),
               makecol(255, 255, 255), -1);
    textout_ex(buffer, font, "Pressione ESC para sair",
               10, 10 + 6 * (5 + text_height(font)),
               makecol(255, 255, 255), -1);
}


/*
 * desenhar_seta: desenha o sentido que a argola sera' lancada
 */
void desenhar_seta(BITMAP *buffer, const int grau, const struct Disco *disco) {
    int cor = (key[KEY_SPACE]) ? makecol(255, 255, 0) : makecol(255, 255, 255);

    line(buffer, xseta, yseta, xseta + (int)(TAM_SETA * cos(grau * PI / 180)),
         yseta - (int)(TAM_SETA * (sin(grau * PI / 180))), cor);

    /* Se apertar ENTER, mostra o alcance do jato d'agua */
    if (key[KEY_ENTER]) {
        int dx = abs(xseta - disco->x);
        int dy = abs(yseta - disco->y);

        /* Raiz do quadrada dos catetos de um triangulo */
        int distancia = (int)(sqrt((dx * dx) + (dy * dy)));
        int MAX_X = xseta - (int)(distancia * cos((180 - grau + ALCANCE) * PI / 180));
        int MAX_Y = yseta - (int)(distancia * sin((180 - grau + ALCANCE) * PI / 180));
        int MIN_X = xseta - (int)(distancia * cos((180 - grau - ALCANCE) * PI / 180));
        int MIN_Y = yseta - (int)(distancia * sin((180 - grau - ALCANCE) * PI / 180));

        line(buffer, xseta, yseta, MAX_X, MAX_Y, cor);
        line(buffer, xseta, yseta, MIN_X, MIN_Y, cor);
    }
}


/*
 * desenhar_disco: desenha um disco
 */
void desenhar_disco(BITMAP *buffer, const struct Disco *disco) {
    ellipse(buffer, disco->x, disco->y, disco->raiox, disco->raioy , disco->cor);
}


/*
 * desenhar_pino: desenha um pino
 */
void desenhar_pino(BITMAP *buffer, const struct Pino *pino) {
    line(buffer, pino->x, pino->y, pino->x, pino->y - pino->h, pino->cor);
}


/*
 * main: Funcao principal
 */
int main() {
    enum {OK, ERRO_VIDEO};

    /* Inicializando a Allegro */
    set_uformat(U_ASCII);
    allegro_init();
    install_keyboard();
    set_window_title("Aqua");

    /* Definindo modo grafico */
    if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, W, H, 0, 0)) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Nao foi possivel iniciar um modo grafico.");
        exit(ERRO_VIDEO);
    }

    /* Criando um buffer para desenho solido */
    BITMAP *buffer = create_bitmap(W, H);
    drawing_mode(DRAW_MODE_SOLID, buffer, 0, 0);

    /* Inicializar variaveis do jogo */
    struct Disco disco;
    struct Pino pino;
    int grau           = MIN_GRAU;      /* Controle do angulo        */
    int forca          = MIN_FORCA;     /* Controle da forca         */
    int grau_aplicado  = 0;             /* Grau do jato d'agua       */
    int forca_aplicada = 0;             /* Forca do jato d'agua      */
    int pontos         = 0;             /* Pontuacao do jogador      */
    char sentido       = DIREITA;       /* Angulo: Direita/Esquerda  */
    char crescimento   = CIMA;          /* Forca: Cima/Baixo         */
    char segurando     = FALSE;         /* Segurando Barra de espaco */
    char entrou        = 0;             /* Disco entrou no pino      */
    char saiu          = 1;             /* Disco saiu do pino        */
    int last_clock;

    /* Definindo valores iniciais do disco */
    disco.x     = (int)(W / 2);
    disco.y     = 0;
    disco.raiox = 30;
    disco.raioy = 5;
    disco.cor   = makecol(0, 200, 255);

    /* Definindo valores iniciais do pino */
    pino.x   = (int)(W * 0.8);
    pino.y   = H;
    pino.h   = 200;
    pino.cor = makecol(255, 255, 0);

    /* JOGAR */
    while (!key[KEY_ESC]) {

        /* Desenhar na tela */
        clear_bitmap(buffer);
        acquire_screen();
        desenhar_status(buffer, grau, forca, entrou, pontos);
        desenhar_seta(buffer, grau, &disco);
        desenhar_disco(buffer, &disco);
        desenhar_pino(buffer, &pino);
        blit(buffer, screen, 0, 0, 0, 0, W, H);
        release_screen();

        /* Tratar eventos do teclado */

        /* Vericiar se o disco saiu do pino */
        if (entrou && (disco.y < (H - pino.h))) {
            saiu = 1;
        }

        /* Verificar se o disco entrou no pino */
        entrou = disco_no_pino(&disco, &pino);

        /* Se esta' segurando a barra de espaco */
        if (key[KEY_SPACE]) {
            segurando = TRUE;
            forca += (crescimento == CIMA) ? 1 : -1;
            if (forca == MAX_FORCA) {
                crescimento = BAIXO;
            } else if (forca == MIN_FORCA) {
                crescimento = CIMA;
            }
            gravidade(&disco, &pino);

        /* Se nao esta' segurando a barra de espaco */
        } else {

            /* Se soltou a barra */
            if (segurando) {
                forca_aplicada = forca;
                grau_aplicado  = grau;
                forca          = MIN_FORCA;
                crescimento    = CIMA;
                segurando      = FALSE;
            }

            /* Se o disco esta' sendo movimentado por uma forca aplicada */
            if (forca_aplicada) {
                jato_dagua(&disco, &pino, &forca_aplicada, &grau_aplicado);
            } else {
                gravidade(&disco, &pino);
            }

            /* Controlar sentido do jato d'agua */
            grau += (sentido == DIREITA) ? 1 : -1;
            if (grau == MAX_GRAU) {
                sentido = ESQUERDA;
            } else if (grau == MIN_GRAU) {
                sentido = DIREITA;
            }
        }

        /* Verificar se marcou ponto */
        if (saiu && entrou && (disco.y == (H - disco.raioy))) {
            pontos += 1;
            saiu = 0;
        }

        last_clock = clock();
        while (clock() - last_clock < 10000);
    }/*JOGO*/

    /* Encerrando o aquaplay */
    destroy_bitmap(buffer);
    allegro_exit();
    return OK;
}
END_OF_MAIN();
