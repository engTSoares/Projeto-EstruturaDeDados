# Projeto-EstruturaDeDados

Repositório com os três trabalhos da disciplina de Estrutura de Dados do curso de Análise e Desenvolvimento de Sistemas (matutino) sob supervisão do professor [Nirton Afonso](https://github.com/NirtonAfonso), implementados em **linguagem C**. Cada trabalho está em sua própria pasta, com código-fonte, executável compilado e arquivo de persistência em CSV.

Desenvolvido por: [Tiago da E. Soares](https://github.com/engTSoares)

```
Projeto-EstruturaDeDados/
├── Trabalho-A/
│   ├── Playlist.c
│   ├── Playlist.exe
│   └── playlist.csv
├── Trabalho-B/
│   ├── HistoricoNavegacao.c
│   ├── HistoricoNavegacao.exe
│   └── navegacao.csv
├── Trabalho-C/
│   ├── RoundRobin.c
│   ├── RoundRobin.exe
│   └── processos.csv
└── README.md
```

---

## Trabalho A — Playlist de Músicas

Simulação de uma playlist usando um **vetor dinâmico** (alocado com `malloc`), onde cada posição representa uma música cadastrada. A lista não precisa ficar ordenada por nenhum campo — as músicas são mantidas na ordem em que foram cadastradas.

### Funcionalidades

| Opção | Função no código | Descrição |
|---|---|---|
| 1 | `inserirMusica` | Cadastra uma nova música, verificando se o `id` já existe antes de inserir |
| 2 | `buscarMusica` | Busca uma música pelo `id` e exibe seus dados |
| 3 | `editarMusica` | Edita título, artista, duração e gênero de uma música (o `id` permanece fixo) |
| 4 | `excluirMusica` | Remove uma música pelo `id`, deslocando os elementos posteriores para preencher o espaço |
| 5 | `listarMusicas` | Lista todas as músicas na ordem em que foram armazenadas |
| 6 | `salvarCSV` | Salva a playlist atual no arquivo `playlist.csv` |
| 7 | `carregarCSV` | Recarrega a playlist a partir do `playlist.csv` |
| 0 | — | Sai do programa, perguntando se deseja salvar antes |

### Regras implementadas

- O `id` é único: o programa impede o cadastro de duas músicas com o mesmo `id` (`buscarIndicePorId`).
- A playlist é mantida na ordem de cadastro (inserção sempre no final do vetor).
- Ao excluir uma música, todos os elementos posteriores são deslocados uma posição para frente, fechando o espaço vazio.
- O carregamento do CSV ignora linhas inválidas, duplicadas ou incompletas, informando o usuário via mensagens de aviso.
- A playlist é carregada automaticamente do CSV ao iniciar o programa.

### Persistência (`playlist.csv`)

Os dados são salvos em formato CSV separado por `;`, com cabeçalho fixo:

```csv
id;titulo;artista;duracao;genero
1;Bohemian Rhapsody;Queen;360;rock
2;TIMEZONE;Maneskin;187;rock
```

---

## Trabalho B — Histórico de Navegação Web

Simulação do funcionamento dos botões **voltar** e **avançar** de um navegador, utilizando **duas pilhas** (estruturas LIFO baseadas em vetor): uma para o histórico de páginas visitadas e outra para as páginas disponíveis para avançar.

### Funcionalidades

| Opção | Função no código | Descrição |
|---|---|---|
| 1 | `visitarPagina` | Empilha uma nova página em `historico` e limpa a pilha `futuro` |
| 2 | `voltarPagina` | Desempilha a página atual de `historico` e a empilha em `futuro` |
| 3 | `avancarPagina` | Desempilha a página de `futuro` e a empilha novamente em `historico` |
| 4 | `listarPilha` (historico) | Lista as páginas do histórico, do topo para a base |
| 5 | `listarPilha` (futuro) | Lista as páginas disponíveis para avançar, do topo para a base |
| 6 | `salvarCSV` | Salva o conteúdo das duas pilhas em `navegacao.csv` |
| 7 | `carregarCSV` | Recarrega as pilhas a partir do `navegacao.csv` |
| 0 | — | Salva automaticamente e encerra o programa |

### Operações de pilha disponíveis

`inicializarPilha`, `pilhaCheia`, `pilhaVazia`, `push`, `pop`, `peek` e `limparPilha` — implementadas de forma genérica e reutilizadas pelas duas pilhas (`historico` e `futuro`).

### Regras implementadas

- Ao visitar uma nova página, ela é empilhada em `historico` e a pilha `futuro` é **limpa** (o "futuro" de uma navegação antiga deixa de existir).
- Ao voltar, a página do topo de `historico` é movida para `futuro`.
- Ao avançar, a página do topo de `futuro` retorna para `historico`.
- O programa trata os casos em que não há página para voltar (`historico` vazio) ou para avançar (`futuro` vazio), exibindo uma mensagem em vez de falhar.
- A data e hora de cada visita são capturadas automaticamente (`obterDataHoraAtual`), no formato `dd/mm/aaaa hh:mm:ss`.
- O programa evita URLs duplicadas dentro da mesma pilha ao carregar do CSV (`urlExisteNaPilha`).

### Persistência (`navegacao.csv`)

Os dados são salvos em formato CSV separado por `;`, com um campo extra indicando a qual pilha cada registro pertence:

```csv
pilha;url;titulo;dataHora
HISTORICO;https://exemplo.com;Exemplo;28/06/2026 10:00:00
FUTURO;https://outro.com;Outro;28/06/2026 10:05:00
```

---

## Trabalho C — Escalonador Round-Robin

Simulação de um **escalonador de processos Round-Robin**, algoritmo clássico de sistemas operacionais. Os processos aguardam em uma **fila circular** (vetor com índices `inicio`/`fim` controlados por módulo) e são executados em fatias de tempo fixas (*quantum*), retornando ao fim da fila se ainda não tiverem concluído.

### Funcionalidades

| Opção | Função no código | Descrição |
|---|---|---|
| 1 | `opcaoAdicionarProcesso` | Adiciona um novo processo à fila, verificando se o `pid` já existe |
| 2 | `executarProximo` | Retira o processo do início da fila e simula a execução de um *quantum* (`QUANTUM = 2`); se ainda restar tempo, o processo volta para o fim da fila |
| 3 | `listarFila` | Lista todos os processos da fila de prontos, do início ao fim |
| 4 | `opcaoConsultarProximo` | Mostra qual processo será o próximo a ser executado, sem removê-lo da fila |
| 5 | `opcaoRemoverProcesso` | Remove (finaliza) um processo da fila pelo `pid` |
| 6 | `salvarCSV` | Salva a fila atual em `processos.csv` |
| 7 | `carregarCSV` | Recarrega a fila a partir do `processos.csv` |
| 0 | — | Salva automaticamente e encerra o programa |

### Regras implementadas

- O `pid` é único: o programa impede a adição de dois processos com o mesmo `pid` (`existePid` / `buscarIndicePorPid`).
- A fila é circular: os índices `inicio` e `fim` avançam com módulo (`% CAPACIDADE`), reaproveitando os espaços liberados pelo vetor.
- Cada execução consome um *quantum* fixo de tempo (`QUANTUM = 2`); se o `tempoRestante` do processo for maior que o quantum, ele retorna ao final da fila com o tempo restante atualizado.
- Se o `tempoRestante` for menor ou igual ao quantum, o processo é considerado finalizado e não retorna à fila.
- Ao remover um processo específico pelo `pid` (mesmo que não esteja no início da fila), a fila é reconstruída preservando a ordem dos demais processos.
- O carregamento do CSV ignora linhas inválidas, com campos vazios/faltando ou com `pid` duplicado.
- A fila é carregada automaticamente do CSV ao iniciar o programa.

### Persistência (`processos.csv`)

Os dados são salvos em formato CSV separado por `;`, com cabeçalho fixo:

```csv
pid;nome;prioridade;tempoRestante
```

---

## Como compilar e executar

Todos os programas são em C puro (sem dependências externas) e podem ser compilados com `gcc`:

```bash
# Trabalho A — Playlist
gcc Trabalho-A/Playlist.c -o playlist
./playlist

# Trabalho B — Histórico de Navegação
gcc Trabalho-B/HistoricoNavegacao.c -o navegacao
./navegacao

# Trabalho C — Escalonador Round-Robin
gcc Trabalho-C/RoundRobin.c -o roundrobin
./roundrobin
```

Cada programa exibe um menu interativo no terminal e salva/carrega automaticamente seus dados a partir do respectivo arquivo `.csv`, que é criado na mesma pasta de execução caso ainda não exista.

