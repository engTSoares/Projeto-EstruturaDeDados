#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CAPACIDADE 50
#define QUANTUM 2
#define ARQUIVO_CSV "processos.csv"

//Estrutura do processo
typedef struct {
    int pid;
    char nome[30];
    int prioridade; // Meramente ilustrativo
    int tempoRestante;
} Processo;

//Estrutura da fila circular
typedef struct {
    Processo dados[CAPACIDADE];
    int inicio;
    int fim;
    int total; // quantidade de elementos na fila 
} FilaCircular;


void inicializarFila(FilaCircular *fila) {
    fila->inicio = 0;
    fila->fim = 0;
    fila->total = 0;
}

int filaVazia(FilaCircular *fila) {
    return fila->total == 0;
}

int filaCheia(FilaCircular *fila) {
    return fila->total == CAPACIDADE;
}

//Operações
int enfileirar(FilaCircular *fila, Processo p) {
    if (filaCheia(fila)) {
        printf("Erro: a fila esta cheia. Nao foi possivel adicionar o processo PID %d.\n", p.pid);
        return 0;
    }

    fila->dados[fila->fim] = p;
    fila->fim = (fila->fim + 1) % CAPACIDADE; /* avanca com modulo */
    fila->total++;
    return 1;
}

int desenfileirar(FilaCircular *fila, Processo *saida) {
    if (filaVazia(fila)) {
        return 0; /* quem chamou e responsavel por avisar o usuario */
    }

    *saida = fila->dados[fila->inicio];
    fila->inicio = (fila->inicio + 1) % CAPACIDADE; /* avanca com modulo */
    fila->total--;
    return 1;
}

int consultarProximo(FilaCircular *fila, Processo *saida) {
    if (filaVazia(fila)) {
        printf("Erro: a fila esta vazia. Nao ha proximo processo.\n");
        return 0;
    }

    *saida = fila->dados[fila->inicio];
    return 1;
}

void listarFila(FilaCircular *fila) {
    if (filaVazia(fila)) {
        printf("A fila de prontos esta vazia.\n");
        return;
    }

    printf("\n--- Fila de prontos (%d processo(s)) ---\n", fila->total);
    printf("%-6s %-20s %-12s %-15s\n", "PID", "Nome", "Prioridade", "TempoRestante");

    int indice = fila->inicio;
    for (int i = 0; i < fila->total; i++) {
        Processo *p = &fila->dados[indice];
        printf("%-6d %-20s %-12d %-15d\n", p->pid, p->nome, p->prioridade, p->tempoRestante);
        indice = (indice + 1) % CAPACIDADE; /* avanca com modulo */
    }
    printf("---------------------------------------\n");
}

int buscarIndicePorPid(FilaCircular *fila, int pid) {
    int indice = fila->inicio;
    for (int i = 0; i < fila->total; i++) {
        if (fila->dados[indice].pid == pid) {
            return indice;
        }
        indice = (indice + 1) % CAPACIDADE;
    }
    return -1;
}

int existePid(FilaCircular *fila, int pid) {
    return buscarIndicePorPid(fila, pid) != -1;
}

int removerPorPid(FilaCircular *fila, int pid) {
    if (filaVazia(fila)) {
        printf("Erro: a fila esta vazia. Nao ha processo para remover.\n");
        return 0;
    }

    int indiceAlvo = buscarIndicePorPid(fila, pid);
    if (indiceAlvo == -1) {
        printf("Processo com PID %d nao encontrado na fila.\n", pid);
        return 0;
    }

    /* Copia os processos (exceto o alvo) para um vetor temporario,
       mantendo a ordem original, depois reconstroi a fila. */
    Processo temporario[CAPACIDADE];
    int quantidadeTemp = 0;

    int indice = fila->inicio;
    for (int i = 0; i < fila->total; i++) {
        if (indice != indiceAlvo) {
            temporario[quantidadeTemp] = fila->dados[indice];
            quantidadeTemp++;
        }
        indice = (indice + 1) % CAPACIDADE;
    }

    inicializarFila(fila);
    for (int i = 0; i < quantidadeTemp; i++) {
        enfileirar(fila, temporario[i]);
    }

    printf("Processo PID %d removido (finalizado) com sucesso.\n", pid);
    return 1;
}

// Executa o proximo processo simulando o quantum.
void executarProximo(FilaCircular *fila) {
    Processo atual;

    if (filaVazia(fila)) {
        printf("Nao ha processos na fila para executar.\n");
        return;
    }

    desenfileirar(fila, &atual);

    printf("\nExecutando PID %d (%s) por %d unidade(s) de tempo...\n",
           atual.pid, atual.nome, QUANTUM);

    if (atual.tempoRestante <= QUANTUM) {
        printf("Processo PID %d (%s) terminou a execucao e saiu da fila.\n",
               atual.pid, atual.nome);
    } else {
        atual.tempoRestante -= QUANTUM;
        printf("Processo PID %d (%s) ainda nao terminou. Tempo restante: %d. Voltando para o fim da fila.\n",
               atual.pid, atual.nome, atual.tempoRestante);
        enfileirar(fila, atual);
    }
}
//Funcao para remover espaço
void removerEspacos(char *texto) {
    int tamanho = strlen(texto);

    // remove espacos/quebras de linha no final 
    while (tamanho > 0 && (texto[tamanho - 1] == ' ' || texto[tamanho - 1] == '\n' || texto[tamanho - 1] == '\r')) {
        texto[tamanho - 1] = '\0';
        tamanho--;
    }

    /* remove espacos no inicio */
    int inicio = 0;
    while (texto[inicio] == ' ') {
        inicio++;
    }

    if (inicio > 0) {
        memmove(texto, texto + inicio, tamanho - inicio + 1);
    }
}

// Salvar fila em arquivo CSV
void salvarCSV(FilaCircular *fila) {
    FILE *arquivo = fopen(ARQUIVO_CSV, "w");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    fprintf(arquivo, "pid;nome;prioridade;tempoRestante\n");

    int indice = fila->inicio;
    for (int i = 0; i < fila->total; i++) {
        Processo *p = &fila->dados[indice];
        fprintf(arquivo, "%d;%s;%d;%d\n", p->pid, p->nome, p->prioridade, p->tempoRestante);
        indice = (indice + 1) % CAPACIDADE;
    }

    fclose(arquivo);
    printf("Dados salvos com sucesso em '%s'.\n", ARQUIVO_CSV);
}

// Carregar fila a partir do arquivo CSV
void carregarCSV(FilaCircular *fila) {
    FILE *arquivo = fopen(ARQUIVO_CSV, "r");

    if (arquivo == NULL) {
        printf("Arquivo '%s' nao encontrado. Iniciando com a fila vazia.\n", ARQUIVO_CSV);
        return;
    }

    char linha[256];
    int numeroLinha = 0;
    int carregados = 0;

    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        printf("Arquivo CSV vazio. Iniciando com a fila vazia.\n");
        fclose(arquivo);
        return;
    }
    numeroLinha++; /* linha 1 = cabecalho, ja foi consumida e sera ignorada */

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        numeroLinha++;

        /* ignora linhas completamente vazias (ex: ultima linha do arquivo) */
        char copiaLinha[256];
        strcpy(copiaLinha, linha);
        removerEspacos(copiaLinha);
        if (strlen(copiaLinha) == 0) {
            continue;
        }

        char linhaTrabalho[256];
        strcpy(linhaTrabalho, linha);

        char *campoPid = strtok(linhaTrabalho, ";");
        char *campoNome = strtok(NULL, ";");
        char *campoPrioridade = strtok(NULL, ";");
        char *campoTempo = strtok(NULL, ";");

        if (campoPid == NULL || campoNome == NULL || campoPrioridade == NULL || campoTempo == NULL) {
            printf("Linha %d invalida (campos faltando): \"%s\". Ignorando essa linha.\n", numeroLinha, linha);
            continue;
        }

        removerEspacos(campoPid);
        removerEspacos(campoNome);
        removerEspacos(campoPrioridade);
        removerEspacos(campoTempo);

        if (strlen(campoPid) == 0 || strlen(campoNome) == 0 ||
            strlen(campoPrioridade) == 0 || strlen(campoTempo) == 0) {
            printf("Linha %d invalida (campo vazio). Ignorando essa linha.\n", numeroLinha);
            continue;
        }

        Processo novo;
        novo.pid = atoi(campoPid);
        strncpy(novo.nome, campoNome, sizeof(novo.nome) - 1);
        novo.nome[sizeof(novo.nome) - 1] = '\0';
        novo.prioridade = atoi(campoPrioridade);
        novo.tempoRestante = atoi(campoTempo);

        if (existePid(fila, novo.pid)) {
            printf("Linha %d ignorada: PID %d ja existe na fila (duplicado).\n", numeroLinha, novo.pid);
            continue;
        }

        if (!enfileirar(fila, novo)) {
            printf("Linha %d ignorada: fila cheia, nao foi possivel carregar mais processos.\n", numeroLinha);
            break;
        }

        carregados++;
    }

    fclose(arquivo);
    printf("Carregamento concluido: %d processo(s) carregado(s) de '%s'.\n", carregados, ARQUIVO_CSV);
}

/* ----------------------------------------------------
 * Le uma linha inteira de texto do teclado com seguranca
 * ---------------------------------------------------- */
void lerTexto(char *destino, int tamanho) {
    fgets(destino, tamanho, stdin);
    removerEspacos(destino);
}

/* ----------------------------------------------------
 * Le um inteiro do teclado com validacao simples
 * ---------------------------------------------------- */
int lerInteiro(void) {
    char buffer[50];
    fgets(buffer, sizeof(buffer), stdin);
    return atoi(buffer);
}

/* ----------------------------------------------------
 * Opcao de menu: adicionar processo
 * ---------------------------------------------------- */
void opcaoAdicionarProcesso(FilaCircular *fila) {
    Processo novo;

    printf("\n--- Adicionar processo ---\n");

    printf("PID: ");
    novo.pid = lerInteiro();

    if (existePid(fila, novo.pid)) {
        printf("Erro: ja existe um processo com PID %d na fila.\n", novo.pid);
        return;
    }

    printf("Nome: ");
    lerTexto(novo.nome, sizeof(novo.nome));

    printf("Prioridade: ");
    novo.prioridade = lerInteiro();

    printf("Tempo restante (de execucao): ");
    novo.tempoRestante = lerInteiro();

    if (enfileirar(fila, novo)) {
        printf("Processo PID %d (%s) adicionado a fila com sucesso.\n", novo.pid, novo.nome);
    }
}

/* ----------------------------------------------------
 * Opcao de menu: consultar proximo processo
 * ---------------------------------------------------- */
void opcaoConsultarProximo(FilaCircular *fila) {
    Processo proximo;
        if (filaVazia(fila)) {
        printf("A fila esta vazia.\n");
        return;
    }

    if (consultarProximo(fila, &proximo)) {
        printf("\nProximo processo a ser executado:\n");
        printf("PID: %d | Nome: %s | Prioridade: %d | Tempo restante: %d\n",
               proximo.pid, proximo.nome, proximo.prioridade, proximo.tempoRestante);
    }
}

/* ----------------------------------------------------
 * Opcao de menu: remover processo (matar processo)
 * ---------------------------------------------------- */
void opcaoRemoverProcesso(FilaCircular *fila) {
    if (filaVazia(fila)) {
        printf("[ERRO] A fila vazia. Nao ha processos para remover\n");
        return;
    }

    printf("\n--- Remover (matar) processo ---\n");
    printf("Informe o PID do processo a remover: ");
    int pid = lerInteiro();

    removerPorPid(fila, pid);
}

// Funcao do Menu
void exibirMenu(void) {
    printf("\n========== ESCALONADOR ROUND-ROBIN ==========\n");
    printf("1 - Adicionar processo\n");
    printf("2 - Executar proximo (simular quantum)\n");
    printf("3 - Listar fila de prontos\n");
    printf("4 - Consultar proximo processo\n");
    printf("5 - Remover processo (matar processo)\n");
    printf("6 - Salvar dados em CSV\n");
    printf("7 - Carregar dados de CSV\n");
    printf("0 - Sair\n");
    printf("===============================================\n");
    printf("Escolha uma opcao: ");
}

// Funcao principal
int main(void) {
    FilaCircular fila;
    inicializarFila(&fila);

    printf("Carregando dados salvos anteriormente...\n");
    carregarCSV(&fila);

    int opcao;
    do {
        exibirMenu();
        opcao = lerInteiro();

        switch (opcao) {
            case 1:
                opcaoAdicionarProcesso(&fila);
                break;
            case 2:
                executarProximo(&fila);
                break;
            case 3:
                listarFila(&fila);
                break;
            case 4:
                opcaoConsultarProximo(&fila);
                break;
            case 5:
                opcaoRemoverProcesso(&fila);
                break;
            case 6:
                salvarCSV(&fila);
                break;
            case 7:
                carregarCSV(&fila);
                break;
            case 0:
                printf("Salvando dados antes de encerrar...\n");
                salvarCSV(&fila);
                printf("Encerrando o programa. Ate logo!\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }

    } while (opcao != 0);

    return 0;
}