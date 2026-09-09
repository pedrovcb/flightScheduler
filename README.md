# FlightScheduler

Simulador de escalonamento de tarefas críticas de voo feito em C pra atividade da disciplina. O programa compara dois algoritmos clássicos de tempo real, Rate Monotonic (RATE) e Earliest Deadline First (EDF), simulando como um computador de bordo decide "quem vai executar agora" quando várias tarefas periódicas competem por uma única CPU com prazos apertados.

## Arquivos do projeto

- `src/main.c` — entry point, valida argumentos, orquestra parse → simulação → escrita

- `src/base.h` — header central com as structs (Tarefa, EstadoTarefa, Segmento) e os protótipos de todas as funções
- `src/base.c` — parser do arquivo de entrada, validações (C ≤ D ≤ P, valores positivos, campos malformados) e escrita do arquivo .out
- `src/prioridade.c` — implementa os dois critérios de prioridade: rate (menor período = maior prioridade) e edf (menor deadline absoluto = maior prioridade), com desempate por ordemIndex
- `src/scheduler.c` — motor de simulação tempo-a-tempo: laço de t=0 até tempoTotal-1 processando chegadas, perdas de deadline, seleção de tarefa, gerenciamento de segmentos (F/H/L/idle)

## Como compilar

```
make
```

Isso gera o executável `scheduler` na raiz.
</br></br>

```
make clean
```
 Pra limpar os .o e o executável.

## Como executar

Rate Monotonic (rate):
```
./scheduler rate voo.txt
```
</br></br>
Earliest Deadline First (edf):
```
./scheduler edf voo.txt
```
</br></br>
O resultado é gravado em `rate_pvcb.out` ou `edf_pvcb.out`, nada é impresso em stdout durante execução normal.

## Testes

O projeto vem com um script que cobre happy path e casos de erro:

```
./run_tests.sh
```

O run_tests.sh roda 12 testes no total:
- 4 testes de happy path: rate/edf com voo.txt e exemplopvcb.txt
- 8 testes de erro: sem argumentos, algoritmo inválido, arquivo inexistente, campo não numérico, campo faltando, deadline zero, C > D e D > P

Cada teste verifica exit code e saída. Os arquivos de saída esperada ficam na pasta `testes/` e são comparados via `diff`.

## Arquitetura de arquivos
```
pvcb/
├── src/
│   ├── base.h
│   ├── base.c
│   ├── prioridade.c
│   ├── scheduler.c
│   └── main.c
├── testes/
│   ├── esperado_rate_voo.out
│   └── esperado_edf_voo.out
├── run_tests.sh
├── Makefile
├── voo.txt
└── exemplopvcb.txt
```

## Sistema Operacional

Esse projeto foi feito com o Linux, mais especificamente na distro Omarchy (Arch Linux). A implementação utiliza exclusivamente funções da biblioteca padrão C (strtol, fopen, fprintf, snprintf) o que garante compatibilidade com qualquer sistema que siga o padrão POSIX, incluindo outras distribuições Linux, macOS e sistemas Unix em geral. A compilação foi feita com o GCC usando as flags -Wall -Wextra -O2.