#include <stdio.h>
#include <string.h>
#include <time.h>

#define TAM_MAX 50
#define ARQUIVO_CSV "navegacao.csv"

// Estrutura do registro 
typedef struct {
    char url[100];
    char titulo[50];
    char dataHora[20];
} Pagina;

//Estrutura das pilhas 
typedef struct {
    Pagina dados[TAM_MAX];
    int topo; // -1 quando vazia
} Pilha;

//Variáveis Globais
Pilha historico;
Pilha futuro;

//Operacoes

void inicializarPilha(Pilha *p) {
    p->topo = -1;
}

int pilhaCheia(Pilha *p) {
    return p->topo == TAM_MAX - 1;
}

int pilhaVazia(Pilha *p) {
    return p->topo == -1;
}

// Push: insere elemento no topo da pilha
int push(Pilha *p, Pagina pagina) {
    if (pilhaCheia(p)) {
        printf("Erro: a pilha esta cheia. Nao e possivel adicionar nova pagina.\n");
        return 0;
    }
    p->topo++;
    p->dados[p->topo] = pagina;
    return 1;
}

// Pop: remove e retorna o elemento do topo
int pop(Pilha *p, Pagina *paginaRemovida) {
    if (pilhaVazia(p)) {
        printf("Erro: a pilha esta vazia. Nao ha pagina para remover.\n");
        return 0;
    }
    *paginaRemovida = p->dados[p->topo];
    p->topo--;
    return 1;
}

// Peek: consulta o topo sem remover
int peek(Pilha *p, Pagina *paginaTopo) {
    if (pilhaVazia(p)) {
        printf("Erro: a pilha esta vazia. Nao ha pagina no topo.\n");
        return 0;
    }
    *paginaTopo = p->dados[p->topo];
    return 1;
}

// Limpa todo o conteudo da pilha (usado ao visitar nova pagina)
void limparPilha(Pilha *p) {
    p->topo = -1;
}

// Listar: mostra todos os elementos da pilha, do topo para a base
void listarPilha(Pilha *p, const char *nomePilha) {
    if (pilhaVazia(p)) {
        printf("\nA pilha [%s] esta vazia.\n", nomePilha);
        return;
    }

    printf("\n--- Pilha [%s] (do topo para a base) ---\n", nomePilha);
    for (int i = p->topo; i >= 0; i--) {
        printf("URL: %-40s | Titulo: %-20s | Data/Hora: %s\n",
               p->dados[i].url, p->dados[i].titulo, p->dados[i].dataHora);
    }
}

// Captura a data e hora atual no formato dd/mm/aaaa hh:mm:ss
void obterDataHoraAtual(char *destino) {
    time_t agora = time(NULL);
    struct tm *tempoLocal = localtime(&agora);

    sprintf(destino, "%02d/%02d/%04d %02d:%02d:%02d",
            tempoLocal->tm_mday,
            tempoLocal->tm_mon + 1,
            tempoLocal->tm_year + 1900,
            tempoLocal->tm_hour,
            tempoLocal->tm_min,
            tempoLocal->tm_sec);
}

// Remove o caractere de quebra de linha (\n) que o fgets deixa na string
void removerQuebraDeLinha(char *str) {
    int tamanho = strlen(str);
    if (tamanho > 0 && str[tamanho - 1] == '\n') {
        str[tamanho - 1] = '\0';
    }
}

// Verifica se uma URL ja existe em uma pilha (evita duplicidade)
int urlExisteNaPilha(Pilha *p, const char *url) {
    for (int i = 0; i <= p->topo; i++) {
        if (strcmp(p->dados[i].url, url) == 0) {
            return 1;
        }
    }
    return 0;
}

/* ---------- Funcoes principais do navegador ---------- */

// Visitar nova pagina: empilha em historico e limpa o futuro
void visitarPagina(Pilha *historico, Pilha *futuro) {
    Pagina novaPagina;

    printf("\nDigite a URL da pagina: ");
    fgets(novaPagina.url, sizeof(novaPagina.url), stdin);
    removerQuebraDeLinha(novaPagina.url);

    printf("Digite o titulo da pagina: ");
    fgets(novaPagina.titulo, sizeof(novaPagina.titulo), stdin);
    removerQuebraDeLinha(novaPagina.titulo);

    obterDataHoraAtual(novaPagina.dataHora);

    if (push(historico, novaPagina)) {
        // Ao visitar uma nova pagina, a pilha futuro deve ser limpa
        limparPilha(futuro);
        printf("Pagina '%s' visitada com sucesso em %s.\n",
               novaPagina.titulo, novaPagina.dataHora);
    }
}

// Voltar: sai de historico e vai para futuro
void voltarPagina(Pilha *historico, Pilha *futuro) {
    Pagina paginaAtual;

    if (pilhaVazia(historico)) {
        printf("\nNao ha pagina anterior no historico para voltar.\n");
        return;
    }

    // Remove a pagina atual do historico
    pop(historico, &paginaAtual);

    // Envia a pagina removida para o futuro
    if (!push(futuro, paginaAtual)) {
        printf("Nao foi possivel mover a pagina para o futuro (pilha cheia).\n");
        return;
    }

    if (pilhaVazia(historico)) {
        printf("\nVoce voltou. Nao ha mais paginas anteriores no historico.\n");
    } else {
        Pagina novaAtual;
        peek(historico, &novaAtual);
        printf("\nVoce voltou para: %s (%s)\n", novaAtual.titulo, novaAtual.url);
    }
}

// Avancar: sai de futuro e volta para historico
void avancarPagina(Pilha *historico, Pilha *futuro) {
    Pagina proximaPagina;

    if (pilhaVazia(futuro)) {
        printf("\nNao ha pagina para avancar.\n");
        return;
    }

    // Remove a pagina do futuro
    pop(futuro, &proximaPagina);

    // Envia a pagina removida de volta para o historico
    if (!push(historico, proximaPagina)) {
        printf("Nao foi possivel avancar (pilha de historico cheia).\n");
        return;
    }

    printf("\nVoce avancou para: %s (%s)\n", proximaPagina.titulo, proximaPagina.url);
}

/* ---------- Funcoes de CSV ---------- */

// Salva o conteudo das duas pilhas em um unico arquivo CSV
// Formato: pilha;url;titulo;dataHora
void salvarCSV(Pilha *historico, Pilha *futuro) {
    FILE *arquivo = fopen(ARQUIVO_CSV, "w");

    if (arquivo == NULL) {
        printf("\nErro: nao foi possivel abrir o arquivo para salvar.\n");
        return;
    }

    // Cabecalho do arquivo
    fprintf(arquivo, "pilha;url;titulo;dataHora\n");

    // Salva do fundo para o topo, assim o load (que usa push) recria a ordem correta
    for (int i = 0; i <= historico->topo; i++) {
        fprintf(arquivo, "HISTORICO;%s;%s;%s\n",
                historico->dados[i].url,
                historico->dados[i].titulo,
                historico->dados[i].dataHora);
    }

    for (int i = 0; i <= futuro->topo; i++) {
        fprintf(arquivo, "FUTURO;%s;%s;%s\n",
                futuro->dados[i].url,
                futuro->dados[i].titulo,
                futuro->dados[i].dataHora);
    }

    fclose(arquivo);
    printf("\nDados salvos com sucesso em '%s'.\n", ARQUIVO_CSV);
}

// Carrega o conteudo do arquivo CSV e reconstroi as duas pilhas
void carregarCSV(Pilha *historico, Pilha *futuro) {
    FILE *arquivo = fopen(ARQUIVO_CSV, "r");

    if (arquivo == NULL) {
        // Trata o caso de arquivo inexistente na primeira execucao
        printf("\nArquivo '%s' nao encontrado. Iniciando com pilhas vazias.\n", ARQUIVO_CSV);
        return;
    }

    char linha[256];
    int numeroLinha = 0;
    int registrosCarregados = 0;

    // Limpa as pilhas atuais antes de carregar (evita duplicar o que ja existe)
    limparPilha(historico);
    limparPilha(futuro);

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        numeroLinha++;

        // Ignora a primeira linha (cabecalho)
        if (numeroLinha == 1) {
            continue;
        }

        removerQuebraDeLinha(linha);

        // Ignora linhas vazias
        if (strlen(linha) == 0) {
            continue;
        }

        char tipoPilha[20];
        Pagina paginaLida;

        // Faz uma copia da linha pois strtok modifica a string original
        char copiaLinha[256];
        strcpy(copiaLinha, linha);

        char *token;
        int campoAtual = 0;

        token = strtok(copiaLinha, ";");
        while (token != NULL && campoAtual < 4) {
            switch (campoAtual) {
                case 0:
                    // strncpy evita overflow caso o campo seja maior que o buffer
                    strncpy(tipoPilha, token, sizeof(tipoPilha) - 1);
                    tipoPilha[sizeof(tipoPilha) - 1] = '\0';
                    break;
                case 1:
                    strncpy(paginaLida.url, token, sizeof(paginaLida.url) - 1);
                    paginaLida.url[sizeof(paginaLida.url) - 1] = '\0';
                    break;
                case 2:
                    strncpy(paginaLida.titulo, token, sizeof(paginaLida.titulo) - 1);
                    paginaLida.titulo[sizeof(paginaLida.titulo) - 1] = '\0';
                    break;
                case 3:
                    strncpy(paginaLida.dataHora, token, sizeof(paginaLida.dataHora) - 1);
                    paginaLida.dataHora[sizeof(paginaLida.dataHora) - 1] = '\0';
                    break;
            }
            campoAtual++;
            token = strtok(NULL, ";");
        }

        // Valida se a linha possui todos os campos esperados (4 campos)
        if (campoAtual != 4) {
            printf("Aviso: linha %d invalida (campos incompletos). Ignorada: \"%s\"\n",
                   numeroLinha, linha);
            continue;
        }

        // Valida se o tipo de pilha e reconhecido
        if (strcmp(tipoPilha, "HISTORICO") != 0 && strcmp(tipoPilha, "FUTURO") != 0) {
            printf("Aviso: linha %d invalida (pilha desconhecida '%s'). Ignorada.\n",
                   numeroLinha, tipoPilha);
            continue;
        }

        // Direciona para a pilha correta, evitando duplicidade de URL
        if (strcmp(tipoPilha, "HISTORICO") == 0) {
            if (urlExisteNaPilha(historico, paginaLida.url)) {
                printf("Aviso: linha %d ignorada (URL duplicada no historico: %s)\n",
                       numeroLinha, paginaLida.url);
                continue;
            }
            if (!push(historico, paginaLida)) {
                printf("Aviso: pilha historico cheia. Linha %d nao carregada.\n", numeroLinha);
                continue;
            }
        } else {
            if (urlExisteNaPilha(futuro, paginaLida.url)) {
                printf("Aviso: linha %d ignorada (URL duplicada no futuro: %s)\n",
                       numeroLinha, paginaLida.url);
                continue;
            }
            if (!push(futuro, paginaLida)) {
                printf("Aviso: pilha futuro cheia. Linha %d nao carregada.\n", numeroLinha);
                continue;
            }
        }

        registrosCarregados++;
    }

    fclose(arquivo);
    printf("\nCarregamento concluido. %d registro(s) carregado(s) de '%s'.\n",
           registrosCarregados, ARQUIVO_CSV);
}

// MENU 
void exibirMenu() {
    printf("\n===== SIMULADOR DE NAVEGADOR =====\n");
    printf("1 - Visitar nova pagina\n");
    printf("2 - Voltar\n");
    printf("3 - Avancar\n");
    printf("4 - Listar Historico\n");
    printf("5 - Listar Futuro\n");
    printf("6 - Salvar dados em CSV\n");
    printf("7 - Carregar dados do CSV\n");
    printf("0 - Sair\n");
    printf("=========================================================\n");
    printf("Escolha uma opcao: ");
}

int main() {
    int opcao;

    inicializarPilha(&historico);
    inicializarPilha(&futuro);

    // Tenta carregar dados salvos anteriormente ao iniciar o programa
    carregarCSV(&historico, &futuro);

    do {
        exibirMenu();
        scanf("%d", &opcao);
        getchar(); // Limpa o '\n' deixado pelo scanf no buffer

        switch (opcao) {
            case 1:
                visitarPagina(&historico, &futuro);
                break;
            case 2:
                voltarPagina(&historico, &futuro);
                break;
            case 3:
                avancarPagina(&historico, &futuro);
                break;
            case 4:
                listarPilha(&historico, "HISTORICO");
                break;
            case 5:
                listarPilha(&futuro, "FUTURO");
                break;
            case 6:
                salvarCSV(&historico, &futuro);
                break;
            case 7:
                carregarCSV(&historico, &futuro);
                break;
            case 0:
                // Salva automaticamente antes de sair, preservando os registros validos
                salvarCSV(&historico, &futuro);
                printf("\nEncerrando o programa. Ate logo!\n");
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n");
        }

    } while (opcao != 0);

    return 0;
}