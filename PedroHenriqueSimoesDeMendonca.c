#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    // VARIAVEIS RESPONSAVEIS POR GERAR O RANKING
    int comidas;
    int colisoes;
    int movimentos;
    char letra;
} tMovimento;

typedef struct
{
    // GUARDAM INFORMACOES SOBRE OS MAPAS
    int linhas;
    int colunas;
    int jogadas;
    int linha_pac;
    int coluna_pac;
    char mapa[40][100];
    int novo_mapa[40][100];
} tMapa;

typedef struct
{
    // FLAGS P/ REGULAR MOVIMENTO DOS FANTASMAS E RESTAURACAO DA COMIDA
    int flag_comida_b;
    int flag_comida_c;
    int flag_comida_p;
    int flag_comida_i;
    int flag_inverte_b;
    int flag_inverte_c;
    int flag_inverte_p;
    int flag_inverte_i;
    int qtd;
} tComida;

typedef struct
{
    // CALCULA N DE MOVIMENTOS DE CADA DIRECAO
    int cima;
    int baixo;
    int esquerda;
    int direita;
    // OUTRAS STRUCTS
    tMapa mapa;
    tComida comida;
    tMovimento movimentos[4];
    int contador;
    int qtdcomidafinal;
    char direcao;
    int linha_pac;
    int coluna_pac;
    int cruzou;
    int linha_anterior_fantasma;
    int coluna_anterior_fantasma;
    int perdeu_parado;
    int flag_portal;
} tJogo;

typedef struct
{
    // ENCONTRA LINHA E COLUNA DO PORTAL
    int linha;
    int coluna;
} tPortal;

void PrintaMapa(tJogo jogo){

    for (int i = 0; i < jogo.mapa.linhas; i++)
    {
        for (int j = 0; j < jogo.mapa.colunas; j++)
        {
            printf("%c", jogo.mapa.mapa[i][j]);
        }
        printf("\n");
    }
}

void PrintaPosJogada(tJogo jogo)
{
    printf("Estado do jogo apos o movimento '%c':\n", jogo.direcao);

    PrintaMapa(jogo);

    printf("Pontuacao: %d\n\n", jogo.qtdcomidafinal);
}

tJogo NovoMapa(tJogo jogo)
{
    // MAPA DA TRILHA ANTES DE INDICAR O MOVIMENTO DO PACMAN
    for (int i = 0; i < jogo.mapa.linhas; i++)
    {
        for (int j = 0; j < jogo.mapa.colunas; j++)
        {
            jogo.mapa.novo_mapa[i][j] = -1; // ATRIBUI ESSE VALOR A TUDO, SE NAO FOR MODIFICADO VIRA '#'
        }
    }

    jogo.mapa.novo_mapa[jogo.mapa.linha_pac - 1][jogo.mapa.coluna_pac - 1] = 0; // ATRIBUI ZERO NA TRILHA RELATIVO A POSICAO EM QUE O PACMAN NASCE

    return jogo;
}

tJogo ZeraMovimentos(tJogo jogo)
{
    // INICIALIZA AS VARIAVEIS DO RANKING
    jogo.movimentos[0].letra = 'a';
    jogo.movimentos[1].letra = 'd';
    jogo.movimentos[2].letra = 'w';
    jogo.movimentos[3].letra = 's';

    for (int i = 0; i < 4; i++)
    {
        jogo.movimentos[i].colisoes = 0;
        jogo.movimentos[i].comidas = 0;
        jogo.movimentos[i].movimentos = 0;
    }

    return jogo;
}

tJogo ReordenaRanking(tJogo jogo, int i, int j)
{
    // COLOCA AS DIRECOES 'W', 'S', 'D' E 'A' NA ORDEM REQUERIDA
    tMovimento savevalue;

    savevalue = jogo.movimentos[i];
    jogo.movimentos[i] = jogo.movimentos[j];
    jogo.movimentos[j] = savevalue;

    return jogo;
}

void ImprimeTrilha(tJogo jogo, FILE *trilha)
{
    for (int i = 0; i < jogo.mapa.linhas; i++)
    {
        for (int j = 0; j < jogo.mapa.colunas; j++)
        {
            if (jogo.mapa.novo_mapa[i][j] == -1)
            {
                fprintf(trilha, "# "); // COLOCA '#' NAS POSICOES EM QUE O PACMN NAO TOCOU
            }
            else
            {
                fprintf(trilha, "%d ", jogo.mapa.novo_mapa[i][j]); // COLOCA O NUMERO DA JOGADA RELATIVA AO MOVIMENTO DO PACMAN EM DADA CELULA
            }
        }
        fprintf(trilha, "\n");
    }
}

void ImprimeRanking(tJogo jogo, FILE *ranking)
{
    // CONTROLA OS PARAMETROS DE PRIORIDADE NA REORDENACAO DO RANKING E O IMPRIME
    for (int i = 0; i < 3; i++)
    {
        for (int j = i + 1; j < 4; j++)
        {
            if (jogo.movimentos[i].comidas < jogo.movimentos[j].comidas)
            {
                jogo = ReordenaRanking(jogo, i, j);
            }
            else if (jogo.movimentos[i].comidas == jogo.movimentos[j].comidas)
            {
                if (jogo.movimentos[i].colisoes > jogo.movimentos[j].colisoes)
                {
                    jogo = ReordenaRanking(jogo, i, j);
                }
                else if (jogo.movimentos[i].colisoes == jogo.movimentos[j].colisoes)
                {
                    if (jogo.movimentos[i].movimentos < jogo.movimentos[j].movimentos)
                    {
                        jogo = ReordenaRanking(jogo, i, j);
                    }
                    else if (jogo.movimentos[i].movimentos == jogo.movimentos[j].movimentos)
                    {
                        if (jogo.movimentos[i].letra > jogo.movimentos[j].letra)
                        {
                            jogo = ReordenaRanking(jogo, i, j);
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        fprintf(ranking, "%c,%d,%d,%d\n", jogo.movimentos[i].letra, jogo.movimentos[i].comidas, jogo.movimentos[i].colisoes, jogo.movimentos[i].movimentos);
    }
}

void ImprimeEstatisticas(tJogo jogo, FILE *estatisticas)
{

    //CALCULA O TOTAL DE COLISOES E DE MOVIMENTOS A PARTIR DO NUMERO DE COLISOES E DE NIVIMENTOS FEITAS COM CADA DIRECAO DO PACMAN
    int total_de_colisoes = jogo.movimentos[0].colisoes + jogo.movimentos[1].colisoes + jogo.movimentos[2].colisoes + jogo.movimentos[3].colisoes;
    int total_movimentos = jogo.baixo + jogo.cima + jogo.esquerda + jogo.direita;

    fprintf(estatisticas, "Numero de movimentos: %d\nNumero de movimentos sem pontuar: %d\nNumero de colisoes com parede: %d\nNumero de movimentos para baixo: %d\nNumero de movimentos para cima: %d\nNumero de movimentos para esquerda: %d\nNumero de movimentos para direita: %d\n", total_movimentos, total_movimentos - jogo.qtdcomidafinal, total_de_colisoes, jogo.baixo, jogo.cima, jogo.esquerda, jogo.direita);
}

void PrintaDerrota(tJogo jogo)
{
    printf("Game over!\nPontuacao final: %d\n", jogo.qtdcomidafinal);
}

void PerdeJogo(tJogo jogo, FILE *resumo, FILE *ranking, FILE *estatisticas, FILE *trilha)
{
    fprintf(resumo, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", jogo.contador + 1, jogo.direcao);

    //SE O PACMAN MORRER DEPOIS DE O FANTASMA CRUZAR COM ELE ESTANDO PARADO, PRINTA A COLISAO DEPOIS DE PRINTAR O FIM DE JOGO
    if (jogo.perdeu_parado == 1)
    {
        fprintf(resumo, "Movimento %d (%c) colidiu na parede\n", jogo.contador + 1, jogo.direcao);
        jogo.perdeu_parado = 0;
    }

    ImprimeRanking(jogo, ranking);
    ImprimeEstatisticas(jogo, estatisticas);
    ImprimeTrilha(jogo, trilha);
    PrintaPosJogada(jogo);
    PrintaDerrota(jogo);
    exit(0);
}

void PrintaVitoria(tJogo jogo)
{
    printf("Voce venceu!\nPontuacao final: %d", jogo.comida.qtd);
}

void VenceJogo(tJogo jogo, FILE *ranking, FILE *estatisticas, FILE *trilha){

    PrintaPosJogada(jogo);
    PrintaVitoria(jogo); // TERMINA O JOGO CASO O PACMAN COMA TODAS AS COMIDAS
    ImprimeRanking(jogo, ranking);
    ImprimeEstatisticas(jogo, estatisticas);
    ImprimeTrilha(jogo, trilha);
    exit(0); // TERMINA O PROGRAMA
}

tMapa LeMapa(int argc, char geral[1001])
{
    tMapa mapa;
    char imprimir_mapa[1001];
    FILE *file;

    // GERA MENSAGEM CASO NADA SEJA DIGITADO
    if (argc <= 1)
    {
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        exit(1);
    }

    sprintf(imprimir_mapa, "%s/mapa.txt", geral); // RECEBE CAMINHO DO DIRETORIO E O MAPA
    file = fopen(imprimir_mapa, "r");

    if (file == NULL)
    {
        printf("ERRO: O arquivo %s nao pode ser lido. O arquivo esperado e o 'mapa.txt'\n ", geral);
        exit(1);
    }

    fscanf(file, "%d %d %d%*c", &mapa.linhas, &mapa.colunas, &mapa.jogadas);

    // LE O ARQUIVO MAPA.TXT
    for (int i = 0; i < mapa.linhas; i++)
    {
        for (int j = 0; j < mapa.colunas; j++)
        {
            fscanf(file, "%c", &mapa.mapa[i][j]);
        }
        fscanf(file, "%*c");
    }

    fclose(file);
    return mapa;
}

tMapa InicializaMapa(tMapa mapa, char geral[1001])
{
    // GERA O ARQUIVO INICIALIZACAO.TXT
    FILE *file;
    char Inicializacao[1001];
    sprintf(Inicializacao, "%s/saida/inicializacao.txt", geral);
    file = fopen(Inicializacao, "w");


    for (int i = 0; i < mapa.linhas; i++)
    {
        for (int j = 0; j < mapa.colunas; j++)
        {
            fprintf(file, "%c", mapa.mapa[i][j]);
            // LOCALIZA POSICAO DO PACMAN
            if (mapa.mapa[i][j] == '>')
            {
                mapa.linha_pac = i + 1;
                mapa.coluna_pac = j + 1;
            }
        }
        fprintf(file, "\n");
    }

    fprintf(file, "Pac-Man comecara o jogo na linha %d e coluna %d", mapa.linha_pac, mapa.coluna_pac);

    fclose(file);
    return mapa;
}

tJogo InicializaVariaveis (tJogo jogo){

    // INICIALIZA AS FLAGS QUE REGULAM MOVIMENTO DOS FANTASMAS
    jogo.comida.flag_comida_b = 0;
    jogo.comida.flag_comida_p = 0;
    jogo.comida.flag_comida_c = 0;
    jogo.comida.flag_comida_i = 0;
    jogo.comida.flag_inverte_b = 0;
    jogo.comida.flag_inverte_c = 0;
    jogo.comida.flag_inverte_p = 0;
    jogo.comida.flag_inverte_i = 0;

    // INICIALIZA VARIAVEIS QUE CONTROLAM OS DADOS DAS ESTATISTICAS
    jogo.cima = 0;
    jogo.baixo = 0;
    jogo.esquerda = 0;
    jogo.direita = 0;

    // INICIALIZA OUTRAS VARIAVEIS
    jogo.perdeu_parado = 0;

    return jogo;
}

tJogo MoverFantasmas(tJogo jogo)
{
    int flag_b = 0, flag_c = 0, flag_p = 0, flag_i = 0;
    jogo.cruzou = 0;

    for (int i = 0; i < jogo.mapa.linhas; i++)
    {
        for (int j = 0; j < jogo.mapa.colunas; j++)
        {
            if (jogo.mapa.mapa[i][j] == 'B')
            { // IDENTIFICA O FANTASMA
                if (flag_b == 0)
                { // CONFERE SE JA FOI MOVIDO
                    if (jogo.mapa.mapa[i][j - 1] != '#' && jogo.comida.flag_inverte_b == 0)
                    { // CONFERE SE PRECISA MUDAR DE DIRECAO OU SE JA MUDOU
                        if (jogo.comida.flag_comida_b == 1)
                        { // CONFERE SE JA PASSOU POR CIMA DE ALGUMA COMIDA
                            jogo.mapa.mapa[i][j] = '*';
                            jogo.comida.flag_comida_b = 0;
                        }
                        else
                        {
                            jogo.mapa.mapa[i][j] = ' ';
                        }
                        if (jogo.mapa.mapa[i][j - 1] == '*')
                        { // CONFERE SE VAI PASSAR POR CIMA DE UMA COMIDA
                            jogo.comida.flag_comida_b = 1;
                        }
                        if (jogo.mapa.mapa[i][j - 1] == '>')
                        { // CONFERE SE VAI CRUZAR COM A POSICAO ANTERIOR DO PACMAN E GUARDA A POSICAO DELE, QUE SERA AGORA OCUPADA PELO FANTASMA
                            jogo.cruzou = 1;
                            jogo.linha_anterior_fantasma = i;
                            jogo.coluna_anterior_fantasma = j;
                            jogo.linha_pac = i;
                            jogo.coluna_pac = j - 1;
                        }
                        jogo.mapa.mapa[i][j - 1] = 'B'; // MOVE O FANTASMA UMA CASA
                    }
                    else
                    { // DIRECAO INVERTIDA
                        jogo.comida.flag_inverte_b = 1;
                        if (jogo.mapa.mapa[i][j + 1] != '#')
                        {
                            if (jogo.comida.flag_comida_b == 1)
                            {
                                jogo.mapa.mapa[i][j] = '*';
                                jogo.comida.flag_comida_b = 0;
                            }
                            else
                            {
                                jogo.mapa.mapa[i][j] = ' ';
                            }
                            if (jogo.mapa.mapa[i][j + 1] == '*')
                            {
                                jogo.comida.flag_comida_b = 1;
                            }
                            if (jogo.mapa.mapa[i][j + 1] == '>')
                            {
                                jogo.cruzou = 1;
                                jogo.linha_anterior_fantasma = i;
                                jogo.coluna_anterior_fantasma = j;
                                jogo.linha_pac = i;
                                jogo.coluna_pac = j + 1;
                            }
                            jogo.mapa.mapa[i][j + 1] = 'B';
                        }
                        else
                        {
                            jogo.comida.flag_inverte_b = 0;
                            // VerificaComida(jogo.comida.flag_comida_p, i, j);
                            if (jogo.comida.flag_comida_b == 1)
                            {
                                jogo.mapa.mapa[i][j] = '*';
                                jogo.comida.flag_comida_b = 0;
                            }
                            else
                            {
                                jogo.mapa.mapa[i][j] = ' ';
                            }
                            if (jogo.mapa.mapa[i][j - 1] == '*')
                            {
                                jogo.comida.flag_comida_b = 1;
                            }
                            if (jogo.mapa.mapa[i][j - 1] == '>')
                            {
                                jogo.cruzou = 1;
                                jogo.linha_anterior_fantasma = i;
                                jogo.coluna_anterior_fantasma = j;
                                jogo.linha_pac = i;
                                jogo.coluna_pac = j - 1;
                            }
                            jogo.mapa.mapa[i][j - 1] = 'B';
                        }
                    }
                    flag_b = 1;
                }
            }
            else if (jogo.mapa.mapa[i][j] == 'C')
            {
                if (flag_c == 0)
                {
                    if (jogo.mapa.mapa[i][j + 1] != '#' && jogo.comida.flag_inverte_c == 0)
                    {
                        if (jogo.comida.flag_comida_c == 1)
                        {
                            jogo.mapa.mapa[i][j] = '*';
                            jogo.comida.flag_comida_c = 0;
                        }
                        else
                        {
                            jogo.mapa.mapa[i][j] = ' ';
                        }
                        if (jogo.mapa.mapa[i][j + 1] == '*')
                        {
                            jogo.comida.flag_comida_c = 1;
                        }
                        if (jogo.mapa.mapa[i][j + 1] == '>')
                        {
                            jogo.cruzou = 1;
                            jogo.linha_anterior_fantasma = i;
                            jogo.coluna_anterior_fantasma = j;
                            jogo.linha_pac = i;
                            jogo.coluna_pac = j + 1;
                        }
                        jogo.mapa.mapa[i][j + 1] = 'C';
                    }
                    else
                    {
                        jogo.comida.flag_inverte_c = 1;
                        if (jogo.mapa.mapa[i][j - 1] != '#')
                        {
                            if (jogo.comida.flag_comida_c == 1)
                            {
                                jogo.mapa.mapa[i][j] = '*';
                                jogo.comida.flag_comida_c = 0;
                            }
                            else
                            {
                                jogo.mapa.mapa[i][j] = ' ';
                            }
                            if (jogo.mapa.mapa[i][j - 1] == '*')
                            {
                                jogo.comida.flag_comida_c = 1;
                            }
                            if (jogo.mapa.mapa[i][j - 1] == '>')
                            {
                                jogo.cruzou = 1;
                                jogo.linha_anterior_fantasma = i;
                                jogo.coluna_anterior_fantasma = j;
                                jogo.linha_pac = i;
                                jogo.coluna_pac = j - 1;
                            }
                            jogo.mapa.mapa[i][j - 1] = 'C';
                        }
                        else
                        {
                            jogo.comida.flag_inverte_c = 0;
                            if (jogo.comida.flag_comida_c == 1)
                            {
                                jogo.mapa.mapa[i][j] = '*';
                                jogo.comida.flag_comida_c = 0;
                            }
                            else
                            {
                                jogo.mapa.mapa[i][j] = ' ';
                            }
                            if (jogo.mapa.mapa[i][j + 1] == '*')
                            {
                                jogo.comida.flag_comida_c = 1;
                            }
                            if (jogo.mapa.mapa[i][j + 1] == '>')
                            {
                                jogo.cruzou = 1;
                                jogo.linha_anterior_fantasma = i;
                                jogo.coluna_anterior_fantasma = j;
                                jogo.linha_pac = i;
                                jogo.coluna_pac = j + 1;
                            }
                            jogo.mapa.mapa[i][j + 1] = 'C';
                        }
                    }
                    flag_c = 1;
                }
            }
            else if (jogo.mapa.mapa[i][j] == 'P')
            {
                if (flag_p == 0)
                {
                    if (jogo.mapa.mapa[i - 1][j] != '#' && jogo.comida.flag_inverte_p == 0)
                    {
                        if (jogo.comida.flag_comida_p == 1)
                        {
                            jogo.mapa.mapa[i][j] = '*';
                            jogo.comida.flag_comida_p = 0;
                        }
                        else
                        {
                            jogo.mapa.mapa[i][j] = ' ';
                        }
                        if (jogo.mapa.mapa[i - 1][j] == '*')
                        {
                            jogo.comida.flag_comida_p = 1;
                        }
                        if (jogo.mapa.mapa[i - 1][j] == '>')
                        {
                            jogo.cruzou = 1;
                            jogo.linha_anterior_fantasma = i;
                            jogo.coluna_anterior_fantasma = j;
                            jogo.linha_pac = i - 1;
                            jogo.coluna_pac = j;
                        }
                        jogo.mapa.mapa[i - 1][j] = 'P';
                    }
                    else
                    {
                        jogo.comida.flag_inverte_p = 1;
                        if (jogo.mapa.mapa[i + 1][j] != '#')
                        {
                            if (jogo.comida.flag_comida_p == 1)
                            {
                                jogo.mapa.mapa[i][j] = '*';
                                jogo.comida.flag_comida_p = 0;
                            }
                            else
                            {
                                jogo.mapa.mapa[i][j] = ' ';
                            }
                            if (jogo.mapa.mapa[i + 1][j] == '*')
                            {
                                jogo.comida.flag_comida_p = 1;
                            }
                            if (jogo.mapa.mapa[i + 1][j] == '>')
                            {
                                jogo.cruzou = 1;
                                jogo.linha_anterior_fantasma = i;
                                jogo.coluna_anterior_fantasma = j;
                                jogo.linha_pac = i + 1;
                                jogo.coluna_pac = j;
                            }
                            jogo.mapa.mapa[i + 1][j] = 'P';
                        }
                        else
                        {
                            jogo.comida.flag_inverte_p = 0;
                            if (jogo.comida.flag_comida_p == 1)
                            {
                                jogo.mapa.mapa[i][j] = '*';
                                jogo.comida.flag_comida_p = 0;
                            }
                            else
                            {
                                jogo.mapa.mapa[i][j] = ' ';
                            }
                            if (jogo.mapa.mapa[i - 1][j] == '*')
                            {
                                jogo.comida.flag_comida_p = 1;
                            }
                            if (jogo.mapa.mapa[i - 1][j] == '>')
                            {
                                jogo.cruzou = 1;
                                jogo.linha_anterior_fantasma = i;
                                jogo.coluna_anterior_fantasma = j;
                                jogo.linha_pac = i - 1;
                                jogo.coluna_pac = j;
                            }
                            jogo.mapa.mapa[i - 1][j] = 'P';
                        }
                    }
                    flag_p = 1;
                }
            }
            else if (jogo.mapa.mapa[i][j] == 'I')
            {
                if (flag_i == 0)
                {
                    if (jogo.mapa.mapa[i + 1][j] != '#' && jogo.comida.flag_inverte_i == 0)
                    {
                        if (jogo.comida.flag_comida_i == 1)
                        {
                            jogo.mapa.mapa[i][j] = '*';
                            jogo.comida.flag_comida_i = 0;
                        }
                        else
                        {
                            jogo.mapa.mapa[i][j] = ' ';
                        }
                        if (jogo.mapa.mapa[i + 1][j] == '*')
                        {
                            jogo.comida.flag_comida_i = 1;
                        }
                        if (jogo.mapa.mapa[i + 1][j] == '>')
                        {
                            jogo.cruzou = 1;
                            jogo.linha_anterior_fantasma = i;
                            jogo.coluna_anterior_fantasma = j;
                            jogo.linha_pac = i + 1;
                            jogo.coluna_pac = j;
                        }
                        jogo.mapa.mapa[i + 1][j] = 'I';
                    }
                    else
                    {
                        jogo.comida.flag_inverte_i = 1;
                        if (jogo.mapa.mapa[i - 1][j] != '#')
                        {
                            if (jogo.comida.flag_comida_i == 1)
                            {
                                jogo.mapa.mapa[i][j] = '*';
                                jogo.comida.flag_comida_i = 0;
                            }
                            else
                            {
                                jogo.mapa.mapa[i][j] = ' ';
                            }
                            if (jogo.mapa.mapa[i - 1][j] == '*')
                            {
                                jogo.comida.flag_comida_i = 1;
                            }
                            if (jogo.mapa.mapa[i - 1][j] == '>')
                            {
                                jogo.cruzou = 1;
                                jogo.linha_anterior_fantasma = i;
                                jogo.coluna_anterior_fantasma = j;
                                jogo.linha_pac = i - 1;
                                jogo.coluna_pac = j;
                            }
                            jogo.mapa.mapa[i - 1][j] = 'I';
                        }
                        else
                        {
                            jogo.comida.flag_inverte_i = 0;
                            if (jogo.comida.flag_comida_i == 1)
                            {
                                jogo.mapa.mapa[i][j] = '*';
                                jogo.comida.flag_comida_i = 0;
                            }
                            else
                            {
                                jogo.mapa.mapa[i][j] = ' ';
                            }
                            if (jogo.mapa.mapa[i + 1][j] == '*')
                            {
                                jogo.comida.flag_comida_i = 1;
                            }
                            if (jogo.mapa.mapa[i + 1][j] == '>')
                            {
                                jogo.cruzou = 1;
                                jogo.linha_anterior_fantasma = i;
                                jogo.coluna_anterior_fantasma = j;
                                jogo.linha_pac = i + 1;
                                jogo.coluna_pac = j;
                            }
                            jogo.mapa.mapa[i + 1][j] = 'I';
                        }
                    }
                    flag_i = 1;
                }
            }
        }
    }

    return jogo;
}

int HouverFantasma(tJogo jogo, int linha, int coluna)
{
    // VERIFICA SE UM FANTASMA VAI PARA O LUGAR DE ONDE O PACMAN SAIU
    if (jogo.mapa.mapa[linha][coluna] != 'B' && jogo.mapa.mapa[linha][coluna] != 'P' && jogo.mapa.mapa[linha][coluna] != 'I' && jogo.mapa.mapa[linha][coluna] != 'C')
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int CruzouComFantasma(tJogo jogo, int linha, int coluna)
{
    if ((jogo.cruzou == 1) && (linha == jogo.linha_anterior_fantasma) && (coluna == jogo.coluna_anterior_fantasma))
    { // SE O FANTASMA TIVER CRUZADO COM A POSICAO INICIAL DO PACMAN E DEPOIS O PACMAN SE MOVER PARA A DIRECAO DA POSICAO ANTERIOR DO FANTASMA, QUER DIZER QUE ELES BATERAM
        return 1;
    }
    else
    {
        return 0;
    }
}

tPortal EncontraPortal(tJogo jogo, tPortal portal)
{ // VARRE MAPA A PROCURA DO PORTAL E GUARDA SUA POSICAO
    for (int i = 0; i < jogo.mapa.linhas; i++)
    {
        for (int j = 0; j < jogo.mapa.colunas; j++)
        {
            if (jogo.mapa.mapa[i][j] == '@')
            {
                portal.linha = i;
                portal.coluna = j;
            }
        }
    }

    return portal;
}

tJogo MoverPacMan(tJogo jogo, FILE *resumo, FILE *ranking, FILE *estatisticas, FILE *trilha)
{
    int pac_linha, pac_coluna, flag = 0;
    tPortal portal;

    // ENCONTRA A POSICAO DO PACMAN
    for (int i = 0; i < jogo.mapa.linhas; i++)
    {
        for (int j = 0; j < jogo.mapa.colunas; j++)
        {
            if (jogo.mapa.mapa[i][j] == '>')
            {
                pac_linha = i;
                pac_coluna = j;
                flag = 1; // SINALIZA CASO AINDA HAJA PACMAN NO MAPA DEPOIS DA MOVIMENTACAO DO FANTASMA
            }
        }
    }

    // SE NAO HOUVER PACMAN DEPOIS DA MOVIMENTACAO DO FANTASMA, SUA POSICAO E GUARDADA
    if (flag == 0)
    {
        pac_linha = jogo.linha_pac;
        pac_coluna = jogo.coluna_pac;
    }

    if (jogo.direcao == 'w')
    { // VERIFICA A DIRECAO
        jogo.movimentos[2].movimentos++;
        jogo.cima++;
        if (jogo.mapa.mapa[pac_linha - 1][pac_coluna] != '#')
        { // VERIFICA SE NAO HA MURO A FRENTE
            if (jogo.flag_portal == 1)
            { // VERIFICA SE PASSOU POR UM PORTAL
                jogo.mapa.mapa[pac_linha][pac_coluna] = '@';
                jogo.flag_portal = 0;
            }
            else if (!(HouverFantasma(jogo, pac_linha, pac_coluna)))
            { // SE NAO HOUVER FANSTAMA DE ONDE O PACMAN SAIU, IMPRIME UM ESPACO EM BRANCO
                jogo.mapa.mapa[pac_linha][pac_coluna] = ' ';
            }
            if (HouverFantasma(jogo, pac_linha - 1, pac_coluna) || CruzouComFantasma(jogo, pac_linha - 1, pac_coluna))
            { // SE ELE BATER EM UM FANTASMA O JOGO ACABA
                PerdeJogo(jogo, resumo, ranking, estatisticas, trilha);
            }
            if (jogo.mapa.mapa[pac_linha - 1][pac_coluna] == '*')
            { // VERIFICA SE HA COMIDA A FRENTE E ANOTA O DADO
                jogo.qtdcomidafinal++;
                jogo.movimentos[2].comidas++;
                fprintf(resumo, "Movimento %d (%c) pegou comida\n", jogo.contador + 1, jogo.direcao);
            }
            if (jogo.mapa.mapa[pac_linha - 1][pac_coluna] == '@')
            { // SE ENTRAR EM UM PORTAL, ANOTA E TELEPORTA ELE PARA O OUTRO PORTAL
                jogo.flag_portal = 1;
                portal = EncontraPortal(jogo, portal);
                jogo.mapa.mapa[portal.linha][portal.coluna] = '>';
                jogo.mapa.novo_mapa[pac_linha - 1][pac_coluna] = jogo.contador + 1;
                jogo.mapa.novo_mapa[portal.linha][portal.coluna] = jogo.contador + 1;
            }
            else
            {
                jogo.mapa.mapa[pac_linha - 1][pac_coluna] = '>'; // MOVE O PACMAN
                jogo.mapa.novo_mapa[pac_linha - 1][pac_coluna] = jogo.contador + 1;
            }
        }
        else
        { // CASO DE BATER EM UM MURO
            if (jogo.flag_portal == 1)
            { // CASO DE ELE ENTRAR NO PORTAL NOVAMENTE POR PERMANCER EM CIMA DELE
                jogo.flag_portal = 1;
                portal = EncontraPortal(jogo, portal);
                jogo.mapa.mapa[portal.linha][portal.coluna] = '>';
                jogo.mapa.mapa[pac_linha][pac_coluna] = '@';
                jogo.mapa.novo_mapa[pac_linha][pac_coluna] = jogo.contador + 1;
                jogo.mapa.novo_mapa[portal.linha][portal.coluna] = jogo.contador + 1;
            }
            jogo.movimentos[2].colisoes++;
            if (HouverFantasma(jogo, pac_linha, pac_coluna))
            { // CASO DE UM FANTASMA BATER NELE ENQUANTO ESTIVER PARADO
                jogo.perdeu_parado = 1;
                PerdeJogo(jogo, resumo, ranking, estatisticas, trilha);
            }
            fprintf(resumo, "Movimento %d (%c) colidiu na parede\n", jogo.contador + 1, jogo.direcao);
            jogo.mapa.novo_mapa[pac_linha][pac_coluna] = jogo.contador + 1;
        }
    }
    else if (jogo.direcao == 'a')
    {
        jogo.movimentos[0].movimentos++;
        jogo.esquerda++;
        if (jogo.mapa.mapa[pac_linha][pac_coluna - 1] != '#')
        {
            if (jogo.flag_portal == 1)
            {
                jogo.mapa.mapa[pac_linha][pac_coluna] = '@';
                jogo.flag_portal = 0;
            }
            else if (!(HouverFantasma(jogo, pac_linha, pac_coluna)))
            {
                jogo.mapa.mapa[pac_linha][pac_coluna] = ' ';
            }
            if (HouverFantasma(jogo, pac_linha, pac_coluna - 1) || CruzouComFantasma(jogo, pac_linha, pac_coluna - 1))
            {
                PerdeJogo(jogo, resumo, ranking, estatisticas, trilha);
            }
            if (jogo.mapa.mapa[pac_linha][pac_coluna - 1] == '*')
            {
                jogo.qtdcomidafinal++;
                jogo.movimentos[0].comidas++;
                fprintf(resumo, "Movimento %d (%c) pegou comida\n", jogo.contador + 1, jogo.direcao);
            }
            if (jogo.mapa.mapa[pac_linha][pac_coluna - 1] == '@')
            {
                jogo.flag_portal = 1;
                portal = EncontraPortal(jogo, portal);
                jogo.mapa.mapa[portal.linha][portal.coluna] = '>';
                jogo.mapa.novo_mapa[pac_linha][pac_coluna - 1] = jogo.contador + 1;
                jogo.mapa.novo_mapa[portal.linha][portal.coluna] = jogo.contador + 1;
            }
            else
            {
                jogo.mapa.mapa[pac_linha][pac_coluna - 1] = '>';
                jogo.mapa.novo_mapa[pac_linha][pac_coluna - 1] = jogo.contador + 1;
            }
        }
        else
        {
            if (jogo.flag_portal == 1)
            {
                jogo.flag_portal = 1;
                portal = EncontraPortal(jogo, portal);
                jogo.mapa.mapa[portal.linha][portal.coluna] = '>';
                jogo.mapa.mapa[pac_linha][pac_coluna] = '@';
                jogo.mapa.novo_mapa[pac_linha][pac_coluna] = jogo.contador + 1;
                jogo.mapa.novo_mapa[portal.linha][portal.coluna] = jogo.contador + 1;
            }
            jogo.movimentos[0].colisoes++;
            if (HouverFantasma(jogo, pac_linha, pac_coluna))
            {
                jogo.perdeu_parado = 1;
                PerdeJogo(jogo, resumo, ranking, estatisticas, trilha);
            }
            fprintf(resumo, "Movimento %d (%c) colidiu na parede\n", jogo.contador + 1, jogo.direcao);
            jogo.mapa.novo_mapa[pac_linha][pac_coluna] = jogo.contador + 1;
        }
    }
    else if (jogo.direcao == 'd')
    {
        jogo.movimentos[1].movimentos++;
        jogo.direita++;
        if (jogo.mapa.mapa[pac_linha][pac_coluna + 1] != '#')
        {
            if (jogo.flag_portal == 1)
            {
                jogo.mapa.mapa[pac_linha][pac_coluna] = '@';
                jogo.flag_portal = 0;
            }
            else if (!(HouverFantasma(jogo, pac_linha, pac_coluna)))
            {
                jogo.mapa.mapa[pac_linha][pac_coluna] = ' ';
            }
            if (HouverFantasma(jogo, pac_linha, pac_coluna + 1) || CruzouComFantasma(jogo, pac_linha, pac_coluna + 1))
            {
                PerdeJogo(jogo, resumo, ranking, estatisticas, trilha);
            }
            if (jogo.mapa.mapa[pac_linha][pac_coluna + 1] == '*')
            {
                jogo.qtdcomidafinal++;
                jogo.movimentos[1].comidas++;
                fprintf(resumo, "Movimento %d (%c) pegou comida\n", jogo.contador + 1, jogo.direcao);
            }
            if (jogo.mapa.mapa[pac_linha][pac_coluna + 1] == '@')
            {
                jogo.flag_portal = 1;
                portal = EncontraPortal(jogo, portal);
                jogo.mapa.mapa[portal.linha][portal.coluna] = '>';
                jogo.mapa.novo_mapa[pac_linha][pac_coluna + 1] = jogo.contador + 1;
                jogo.mapa.novo_mapa[portal.linha][portal.coluna] = jogo.contador + 1;
            }
            else
            {
                jogo.mapa.mapa[pac_linha][pac_coluna + 1] = '>';
                jogo.mapa.novo_mapa[pac_linha][pac_coluna + 1] = jogo.contador + 1;
            }
        }
        else
        {
            if (jogo.flag_portal == 1)
            {
                jogo.flag_portal = 1;
                portal = EncontraPortal(jogo, portal);
                jogo.mapa.mapa[portal.linha][portal.coluna] = '>';
                jogo.mapa.mapa[pac_linha][pac_coluna] = '@';
                jogo.mapa.novo_mapa[pac_linha][pac_coluna] = jogo.contador + 1;
                jogo.mapa.novo_mapa[portal.linha][portal.coluna] = jogo.contador + 1;
            }
            jogo.movimentos[1].colisoes++;
            if (HouverFantasma(jogo, pac_linha, pac_coluna))
            {
                jogo.perdeu_parado = 1;
                PerdeJogo(jogo, resumo, ranking, estatisticas, trilha);
            }
            fprintf(resumo, "Movimento %d (%c) colidiu na parede\n", jogo.contador + 1, jogo.direcao);
            jogo.mapa.novo_mapa[pac_linha][pac_coluna] = jogo.contador + 1;
        }
    }
    else if (jogo.direcao == 's')
    {
        jogo.baixo++;
        jogo.movimentos[3].movimentos++;
        if (jogo.mapa.mapa[pac_linha + 1][pac_coluna] != '#')
        {
            if (jogo.flag_portal == 1)
            {
                jogo.mapa.mapa[pac_linha][pac_coluna] = '@';
                jogo.flag_portal = 0;
            }
            else if (!(HouverFantasma(jogo, pac_linha, pac_coluna)))
            {
                jogo.mapa.mapa[pac_linha][pac_coluna] = ' ';
            }
            if (HouverFantasma(jogo, pac_linha + 1, pac_coluna) || CruzouComFantasma(jogo, pac_linha + 1, pac_coluna))
            {
                PerdeJogo(jogo, resumo, ranking, estatisticas, trilha);
            }
            if (jogo.mapa.mapa[pac_linha + 1][pac_coluna] == '*')
            {
                jogo.qtdcomidafinal++;
                jogo.movimentos[3].comidas++;
                fprintf(resumo, "Movimento %d (%c) pegou comida\n", jogo.contador + 1, jogo.direcao);
            }
            ////////////////////////////////////////////////////////////
            if (jogo.mapa.mapa[pac_linha + 1][pac_coluna] == '@')
            {
                jogo.flag_portal = 1;
                portal = EncontraPortal(jogo, portal);
                jogo.mapa.mapa[portal.linha][portal.coluna] = '>';
                jogo.mapa.novo_mapa[pac_linha + 1][pac_coluna] = jogo.contador + 1;
                jogo.mapa.novo_mapa[portal.linha][portal.coluna] = jogo.contador + 1;
            }
            /////////////////////////////////////////////////////////////
            else
            {
                jogo.mapa.novo_mapa[pac_linha + 1][pac_coluna] = jogo.contador + 1;
                jogo.mapa.mapa[pac_linha + 1][pac_coluna] = '>';
            }
        }
        else
        {
            if (jogo.flag_portal == 1)
            {
                jogo.flag_portal = 1;
                portal = EncontraPortal(jogo, portal);
                jogo.mapa.mapa[portal.linha][portal.coluna] = '>';
                jogo.mapa.mapa[pac_linha][pac_coluna] = '@';
                jogo.mapa.novo_mapa[pac_linha][pac_coluna] = jogo.contador + 1;
                jogo.mapa.novo_mapa[portal.linha][portal.coluna] = jogo.contador + 1;
            }
            jogo.movimentos[3].colisoes++;
            if (HouverFantasma(jogo, pac_linha, pac_coluna))
            {
                jogo.perdeu_parado = 1;
                PerdeJogo(jogo, resumo, ranking, estatisticas, trilha);
            }
            fprintf(resumo, "Movimento %d (%c) colidiu na parede\n", jogo.contador + 1, jogo.direcao);
            jogo.mapa.novo_mapa[pac_linha][pac_coluna] = jogo.contador + 1;
        }
    }

    return jogo;
}

tComida LocalizaComida(tJogo jogo)
{
    // CALCULA O NUMERO DE COMIDAS NO MAPA
    tComida comida;
    comida.qtd = 0;

    for (int i = 0; i < jogo.mapa.linhas; i++)
    {
        for (int j = 0; j < jogo.mapa.colunas; j++)
        {
            if (jogo.mapa.mapa[i][j] == '*')
            {
                comida.qtd++;
            }
        }
    }
    return comida;
}

tJogo Play(tJogo jogo, char geral[1001])
{

    FILE *resumo, *ranking, *estatisticas, *trilha;

    // ABRE OS ARQUIVOS A SEREM REDIGIDOS
    char Ranking[1001];
    sprintf(Ranking, "%s/saida/ranking.txt", geral);
    ranking = fopen(Ranking, "w");

    char Resumo[1001];
    sprintf(Resumo, "%s/saida/resumo.txt", geral);
    resumo = fopen(Resumo, "a");

    char Estatisticas[1001];
    sprintf(Estatisticas, "%s/saida/estatisticas.txt", geral);
    estatisticas = fopen(Estatisticas, "w");

    char Trilha[1001];
    sprintf(Trilha, "%s/saida/trilha.txt", geral);
    trilha = fopen(Trilha, "w");

    // INICIALIZA VARIAVEIS 
    int flag = 0;
    jogo.qtdcomidafinal = 0;

    jogo.comida = LocalizaComida(jogo);

    jogo = InicializaVariaveis(jogo);

    jogo = ZeraMovimentos(jogo);
    jogo = NovoMapa(jogo); // GERA MAPA DA TRILHA

    for (jogo.contador = 0; jogo.contador < jogo.mapa.jogadas; jogo.contador++)
    {

        scanf("%c", &jogo.direcao);
        scanf("%*c");

        // GERA A MOVIMENTACAO NO MAPA
        jogo = MoverFantasmas(jogo);

        jogo = MoverPacMan(jogo, resumo, ranking, estatisticas, trilha);

        if (jogo.qtdcomidafinal == jogo.comida.qtd)
        { // SE ELE COMER TODAS AS COMIDAS, O JOGO ACABA
            VenceJogo(jogo, ranking, estatisticas, trilha);
        }

        printf("Estado do jogo apos o movimento '%c':\n", jogo.direcao);

        PrintaMapa(jogo); // PRINTA O MAPA COM AS MODIFICACOES DE CADA JOGADA

        printf("Pontuacao: %d\n\n", jogo.qtdcomidafinal);
    }

    ImprimeRanking(jogo, ranking);
    ImprimeEstatisticas(jogo, estatisticas);
    ImprimeTrilha(jogo, trilha);

    // FECHA OS ARQUIVOS
    fclose(resumo);
    fclose(ranking);
    fclose(estatisticas);
    fclose(trilha);
    return jogo;
}

int main(int argc, char *argv[])
{
    tMapa mapa;
    tJogo jogo;

    // DA OUTRO NOME P/ ARGV
    char geral[1001];
    sprintf(geral, "%s", argv[1]);

    //-------INICIALIZACAO-------//
    mapa = LeMapa(argc, geral);
    mapa = InicializaMapa(mapa, geral);

    //-------JOGO-------//
    jogo.mapa = mapa;

    jogo = Play(jogo, geral);
    PrintaDerrota(jogo); // SE SAIU DA FUNCAO SEM GANHAR, AS JOGADAS ACABARAM E O JOGO TERMINA

    return 0;
}