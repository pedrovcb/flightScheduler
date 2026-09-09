#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOGIN "pvcb"
#define MAX_NOME 32
#define MAX_TAREFAS 64
#define MAX_SEGMENTOS 1024

typedef struct {
    char nome[MAX_NOME];
    int periodo;
    int deadline;
    int burst;
    int ordemIndex; //posição no arquivo de entrada, para desempate
} Tarefa;

typedef struct {
    int restante; //burst restante da execução atual
    int deadlineAbsoluto; //deadline absoluto = chegada + deadline relativo
    int proximaChegada; //próximo instante de chegada
    int numCompletas; //quantas execuções completaram (F)
    int numPerdidas; //quantas execuções foram perdidas (L)
    int ativa; //1: tem alguma execução ativa; 0: não tem
} EstadoTarefa;

typedef struct {
    const char *nome;
    int duracao;
    char motivo; //F: finished; H: halted; L: lost; 0: idle
} Segmento;

//Parser dos argumentos e validação
int parseIntStrict(const char *str, int *out);
int parseArquivo(const char *arquivo, int *tempoTotal, Tarefa *tarefas, int *numTarefas);

//Escrita do arquivo .out
int escritaSaida(const char *algoritmo, const Tarefa *tarefas,
                 const EstadoTarefa *estados, int numTarefas,
                 const Segmento *segmentos, int numSegmentos);

//Critério pra prioridade (rate e edf)
int selecionaTarefa(const Tarefa *tarefas, const EstadoTarefa *estados, int numTarefas, const char *algoritmo);

//Motor de simulação
int executaSimulacao(const char *algoritmo, const Tarefa *tarefas, EstadoTarefa *estados,
                     int numTarefas, int tempoTotal, Segmento *segmentos, int *numSegmentos);

#endif