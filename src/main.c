#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"

int main(int argc, char **argv){
    //validação dos args
    if(argc != 3){
        fprintf(stderr, "Uso: %s <rate|edf> <arquivo>\n", argv[0]);
        return 1;
    }

    if(strcmp(argv[1], "rate") != 0 && strcmp(argv[1], "edf") != 0){
        fprintf(stderr, "Erro: algoritmo '%s' desconhecido (use rate ou edf)!\n", argv[1]);
        return 1;
    }

    Tarefa tarefas[MAX_TAREFAS];
    int numTarefas = 0;
    int tempoTotal = 0;

    if(parseArquivo(argv[2], &tempoTotal, tarefas, &numTarefas) != 0){
        return 1;
    }

    //inicializa estados de todas as tarefas
    EstadoTarefa estados[MAX_TAREFAS];
    memset(estados, 0, sizeof(estados));

    for(int i = 0; i < numTarefas; i++){
        estados[i].proximaChegada = 0;
        estados[i].ativa = 0;
        estados[i].restante = 0;
    }

    //executa a simulação
    Segmento segmentos[MAX_SEGMENTOS];
    int numSegmentos = 0;

    if(executaSimulacao(argv[1], tarefas, estados, numTarefas, tempoTotal, segmentos, &numSegmentos) != 0){
        return 1;
    }

    //escreve a saída
    escritaSaida(argv[1], tarefas, estados, numTarefas, segmentos, numSegmentos);

    return 0;
}