#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MUSICAS 100
#define TAM_STR 50
#define NOME_ARQUIVO "playlist.csv"

/* ----------- Estrutura do registro ----------- */
typedef struct {
    int id;
    char titulo[TAM_STR];
    char artista[TAM_STR];
    int duracao; 
    char genero[TAM_STR];
} Musica;

/* ----------- Estrutura da playlist (vetor dinamico) ----------- */
typedef struct {
    Musica *dados;   /* vetor alocado dinamicamente */
    int total;       /* quantidade de musicas cadastradas */
    int capacidade;  /* capacidade maxima (MAX_MUSICAS) */
} Playlist;

/* ===========================================================
   Funcoes de apoio (entrada de dados)
   =========================================================== */

/* Le uma linha de texto com segurança, removendo o '\n' final */
void lerString(char *destino, int tamanho) {
    if (fgets(destino, tamanho, stdin) != NULL) {
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        }
    } else {
        destino[0] = '\0';
    }
}

/* Le um inteiro com validacao basica */
int lerInteiro(void) {
    char buffer[64];
    lerString(buffer, sizeof(buffer));
    return atoi(buffer);
}

/* Remove espacos em branco extras no inicio/fim de uma string
   (usado para garantir que o CSV nao tenha espacos indesejados) */
void removerEspacos(char *str) {
    int inicio = 0;
    int fim = (int)strlen(str) - 1;

    while (str[inicio] == ' ') inicio++;
    while (fim >= inicio && str[fim] == ' ') fim--;

    int j = 0;
    for (int i = inicio; i <= fim; i++) {
        str[j++] = str[i];
    }
    str[j] = '\0';
}

/* ===========================================================
   Funcoes da Playlist
   =========================================================== */

/* Inicializa a playlist alocando o vetor dinamicamente */
void inicializarPlaylist(Playlist *p) {
    p->dados = (Musica *) malloc(sizeof(Musica) * MAX_MUSICAS);
    if (p->dados == NULL) {
        printf("Erro: nao foi possivel alocar memoria para a playlist.\n");
        exit(1);
    }
    p->total = 0;
    p->capacidade = MAX_MUSICAS;
}

/* Libera a memoria usada pela playlist */
void liberarPlaylist(Playlist *p) {
    free(p->dados);
    p->dados = NULL;
    p->total = 0;
}

/* Retorna o indice de uma musica pelo id, ou -1 se nao encontrada */
int buscarIndicePorId(Playlist *p, int id) {
    for (int i = 0; i < p->total; i++) {
        if (p->dados[i].id == id) {
            return i;
        }
    }
    return -1;
}

/* Insere uma nova musica na playlist (no final do vetor, ordem de cadastro) */
void inserirMusica(Playlist *p) {
    if (p->total >= p->capacidade) {
        printf("\n[ERRO] A playlist esta cheia! Nao e possivel inserir mais musicas.\n");
        return;
    }

    Musica nova;

    printf("\n--- Cadastro de Nova Musica ---\n");

    printf("ID (numero unico): ");
    nova.id = lerInteiro();

    /* Verifica se o ID ja existe */
    if (buscarIndicePorId(p, nova.id) != -1) {
        printf("[ERRO] Ja existe uma musica cadastrada com o ID %d.\n", nova.id);
        return;
    }

    printf("Titulo: ");
    lerString(nova.titulo, TAM_STR);

    printf("Artista: ");
    lerString(nova.artista, TAM_STR);

    printf("Duracao (em segundos): ");
    nova.duracao = lerInteiro();

    printf("Genero: ");
    lerString(nova.genero, TAM_STR);

    /* Insere no final do vetor, mantendo a ordem de cadastro */
    p->dados[p->total] = nova;
    p->total++;

    printf("\nMusica cadastrada com sucesso! (posicao %d na lista)\n", p->total);
}

/* Busca uma musica pelo ID e exibe todos os dados */
void buscarMusica(Playlist *p) {
    if (p->total == 0) {
        printf("\n[AVISO] A playlist esta vazia. Nada para buscar.\n");
        return;
    }

    printf("\n--- Buscar Musica ---\n");
    printf("Digite o ID da musica: ");
    int id = lerInteiro();

    int indice = buscarIndicePorId(p, id);

    if (indice == -1) {
        printf("[ERRO] Nenhuma musica encontrada com o ID %d.\n", id);
        return;
    }

    Musica *m = &p->dados[indice];
    printf("\n--- Musica Encontrada (posicao %d) ---\n", indice + 1);
    printf("ID......: %d\n", m->id);
    printf("Titulo..: %s\n", m->titulo);
    printf("Artista.: %s\n", m->artista);
    printf("Duracao.: %d segundos\n", m->duracao);
    printf("Genero..: %s\n", m->genero);
}

/* Edita os campos nao-chave (titulo, artista, duracao, genero) de uma musica */
void editarMusica(Playlist *p) {
    if (p->total == 0) {
        printf("\n[AVISO] A playlist esta vazia. Nada para editar.\n");
        return;
    }

    printf("\n--- Editar Musica ---\n");
    printf("Digite o ID da musica que deseja editar: ");
    int id = lerInteiro();

    int indice = buscarIndicePorId(p, id);

    if (indice == -1) {
        printf("[ERRO] Nenhuma musica encontrada com o ID %d.\n", id);
        return;
    }

    Musica *m = &p->dados[indice];

    printf("\nEditando musica (ID %d permanece o mesmo).\n", m->id);

    printf("Novo titulo [%s]: ", m->titulo);
    lerString(m->titulo, TAM_STR);

    printf("Novo artista [%s]: ", m->artista);
    lerString(m->artista, TAM_STR);

    printf("Nova duracao em segundos [%d]: ", m->duracao);
    m->duracao = lerInteiro();

    printf("Novo genero [%s]: ", m->genero);
    lerString(m->genero, TAM_STR);

    printf("\nMusica atualizada com sucesso!\n");
}

/* Exclui uma musica pelo ID, deslocando os elementos posteriores */
void excluirMusica(Playlist *p) {
    if (p->total == 0) {
        printf("\n[AVISO] A playlist esta vazia. Nada para excluir.\n");
        return;
    }

    printf("\n--- Excluir Musica ---\n");
    printf("Digite o ID da musica que deseja excluir: ");
    int id = lerInteiro();

    int indice = buscarIndicePorId(p, id);

    if (indice == -1) {
        printf("[ERRO] Nenhuma musica encontrada com o ID %d.\n", id);
        return;
    }

    /* Desloca todos os elementos posteriores uma posicao para frente */
    for (int i = indice; i < p->total - 1; i++) {
        p->dados[i] = p->dados[i + 1];
    }

    p->total--;

    printf("\nMusica removida com sucesso! A lista foi reorganizada.\n");
}

/* Lista todas as musicas na ordem atual do vetor */
void listarMusicas(Playlist *p) {
    if (p->total == 0) {
        printf("\n[AVISO] A playlist esta vazia.\n");
        return;
    }

    printf("\n=========== PLAYLIST (%d musica(s)) ===========\n", p->total);
    for (int i = 0; i < p->total; i++) {
        Musica *m = &p->dados[i];
        printf("\nPosicao %d:\n", i + 1);
        printf("  ID......: %d\n", m->id);
        printf("  Titulo..: %s\n", m->titulo);
        printf("  Artista.: %s\n", m->artista);
        printf("  Duracao.: %d segundos\n", m->duracao);
        printf("  Genero..: %s\n", m->genero);
    }
    printf("\n================================================\n");
}

/*Funcoes de CSV (Salvar / Carregar)*/
void salvarCSV(Playlist *p) {
    FILE *arquivo = fopen(NOME_ARQUIVO, "w");
    if (arquivo == NULL) {
        printf("\n[ERRO] Nao foi possivel abrir/criar o arquivo %s para salvar.\n", NOME_ARQUIVO);
        return;
    }

    /* Cabecalho exatamente com os nomes dos campos */
    fprintf(arquivo, "id;titulo;artista;duracao;genero\n");

    for (int i = 0; i < p->total; i++) {
        Musica *m = &p->dados[i];
        fprintf(arquivo, "%d;%s;%s;%d;%s\n",
                m->id, m->titulo, m->artista, m->duracao, m->genero);
    }

    fclose(arquivo);
    printf("\nDados salvos com sucesso em \"%s\"!\n", NOME_ARQUIVO);
}

/* Carrega os dados do CSV ao iniciar o programa, ignorando a 1a linha
   (cabecalho), validando os campos e nao permitindo IDs duplicados. */
void carregarCSV(Playlist *p) {
    FILE *arquivo = fopen(NOME_ARQUIVO, "r");

    if (arquivo == NULL) {
        /* Arquivo nao existe ainda - nao e um erro fatal, apenas comeca vazio */
        printf("[INFO] Arquivo \"%s\" nao encontrado. Iniciando playlist vazia.\n", NOME_ARQUIVO);
        return;
    }

    char linha[256];
    int numeroLinha = 0;
    int carregadas = 0;
    int ignoradas = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        numeroLinha++;

        /* Remove o '\n' do final, se existir */
        size_t len = strlen(linha);
        if (len > 0 && linha[len - 1] == '\n') {
            linha[len - 1] = '\0';
        }

        /* Ignora a primeira linha (cabecalho) */
        if (numeroLinha == 1) {
            continue;
        }

        /* Ignora linhas completamente vazias (ex: linha final do arquivo) */
        if (strlen(linha) == 0) {
            continue;
        }

        /* Verifica se a playlist ja esta cheia */
        if (p->total >= p->capacidade) {
            printf("[AVISO] Limite da playlist atingido. Linha %d do CSV ignorada.\n", numeroLinha);
            ignoradas++;
            continue;
        }

        /* Faz uma copia da linha pois strtok modifica a string original */
        char copia[256];
        strcpy(copia, linha);

        char *campos[5];
        int qtdCampos = 0;

        char *token = strtok(copia, ";");
        while (token != NULL && qtdCampos < 5) {
            campos[qtdCampos] = token;
            qtdCampos++;
            token = strtok(NULL, ";");
        }

        /* Valida se a linha possui todos os 5 campos esperados */
        if (qtdCampos != 5) {
            printf("[AVISO] Linha %d invalida (esperado 5 campos, encontrado %d). Ignorada.\n",
                   numeroLinha, qtdCampos);
            ignoradas++;
            continue;
        }

        Musica m;
        m.id = atoi(campos[0]);

        strncpy(m.titulo, campos[1], TAM_STR - 1);
        m.titulo[TAM_STR - 1] = '\0';

        strncpy(m.artista, campos[2], TAM_STR - 1);
        m.artista[TAM_STR - 1] = '\0';

        m.duracao = atoi(campos[3]);

        strncpy(m.genero, campos[4], TAM_STR - 1);
        m.genero[TAM_STR - 1] = '\0';

        /* Remove possiveis espacos extras nos campos de texto */
        removerEspacos(m.titulo);
        removerEspacos(m.artista);
        removerEspacos(m.genero);

        /* Nao permite registros duplicados (mesmo ID) ao carregar */
        if (buscarIndicePorId(p, m.id) != -1) {
            printf("[AVISO] ID %d duplicado na linha %d do CSV. Registro ignorado.\n",
                   m.id, numeroLinha);
            ignoradas++;
            continue;
        }

        p->dados[p->total] = m;
        p->total++;
        carregadas++;
    }

    fclose(arquivo);

    printf("[INFO] Carregamento concluido: %d musica(s) carregada(s), %d linha(s) ignorada(s).\n",
           carregadas, ignoradas);
}

/* ===========================================================
   Menu principal
   =========================================================== */

void exibirMenu(void) {
    printf("\n==================== PLAYLIST DE MUSICAS ====================\n");
    printf("1 - Inserir musica\n");
    printf("2 - Buscar musica (por ID)\n");
    printf("3 - Editar musica\n");
    printf("4 - Excluir musica\n");
    printf("5 - Listar todas as musicas\n");
    printf("6 - Salvar playlist em CSV\n");
    printf("7 - Carregar playlist do CSV\n");
    printf("0 - Sair\n");
    printf("===============================================================\n");
    printf("Escolha uma opcao: ");
}

int main(void) {
    Playlist playlist;
    inicializarPlaylist(&playlist);

    /* Carrega o CSV automaticamente ao iniciar o programa */
    carregarCSV(&playlist);

    int opcao;
    int continuar = 1;

    do {
        exibirMenu();
        opcao = lerInteiro();

        switch (opcao) {
            case 1:
                inserirMusica(&playlist);
                break;
            case 2:
                buscarMusica(&playlist);
                break;
            case 3:
                editarMusica(&playlist);
                break;
            case 4:
                excluirMusica(&playlist);
                break;
            case 5:
                listarMusicas(&playlist);
                break;
            case 6:
                salvarCSV(&playlist);
                break;
            case 7:
                /* recarregar substitui os dados em memoria pelos do arquivo.
                   Aqui optamos por zerar e recarregar do zero para evitar duplicidade. */
                playlist.total = 0;
                carregarCSV(&playlist);
                break;
            case 0:
                printf("\nDeseja salvar antes de sair? (1 - Sim / 0 - Nao): ");
                int resposta = lerInteiro();
                if (resposta == 1) {
                    salvarCSV(&playlist);
                }
                continuar = 0;
                printf("\nEncerrando o programa...\n");
                break;
            default:
                printf("\n[ERRO] Opcao invalida. Tente novamente.\n");
                break;
        }

    } while (continuar);

    /* Libera a memoria usada pela estrutura antes de encerrar */
    liberarPlaylist(&playlist);

    return 0;
}