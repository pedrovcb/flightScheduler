#include <stdio.h>
#include "base.h"

int executaSimulacao(const char *algoritmo, const Tarefa *tarefas, EstadoTarefa *estados, int numTarefas, int tempoTotal, Segmento *segmentos, int *numSegmentos){
    int segAtual = -1;
    int tarefaAtual = -1;

    for(int t = 0; t < tempoTotal; t++){

        //Passo 1: chegadas
        for(int i = 0; i < numTarefas; i++){
            if(estados[i].proximaChegada == t){
                estados[i].restante = tarefas[i].burst;
                estados[i].deadlineAbsoluto = t + tarefas[i].deadline;
                estados[i].ativa = 1;
                estados[i].proximaChegada += tarefas[i].periodo;
            }
        }

        //Passo 2: perdas de deadline
        for(int i = 0; i < numTarefas; i++){
            if(estados[i].ativa == 1 && estados[i].restante > 0 && estados[i].deadlineAbsoluto == t){
                estados[i].numPerdidas++;
                estados[i].ativa = 0;
                estados[i].restante = 0;
                //se essa tarefa estava executando, fecha segmento com L
                if(i == tarefaAtual){
                    if(segAtual >= 0){
                        segmentos[segAtual].motivo = 'L';
                    }
                    tarefaAtual = -1;
                }
            }
        }

        //Passo 3: seleção
        int proximo = selecionaTarefa(tarefas, estados, numTarefas, algoritmo);

        //Passo 4: gerenciar segmento
        if(proximo != tarefaAtual){
            //fecha segmento anterior
            if(segAtual >= 0){
                if(tarefaAtual >= 0 && segmentos[segAtual].motivo == 0){
                    //tarefa ainda ativa e sem motivo = preemptada
                    segmentos[segAtual].motivo = 'H';
                }
                //se motivo já foi setado (L ou F), não sobrescreve
                //se idle (tarefaAtual == -1), não tem letra
            }
            //inicia novo segmento
            segAtual++;
            segmentos[segAtual].duracao = 0;
            segmentos[segAtual].motivo = 0;
            if(proximo >= 0){
                segmentos[segAtual].nome = tarefas[proximo].nome;
            } else {
                segmentos[segAtual].nome = NULL;
            }
            tarefaAtual = proximo;
        }

        //Passo 5: execução
        if(tarefaAtual >= 0){
            estados[tarefaAtual].restante--;
            segmentos[segAtual].duracao++;

            if(estados[tarefaAtual].restante == 0){
                //terminou a execução
                estados[tarefaAtual].numCompletas++;
                estados[tarefaAtual].ativa = 0;
                segmentos[segAtual].motivo = 'F';
            }
        }
    }

    //fecha último segmento
    if(segAtual >= 0){
        if(tarefaAtual >= 0 && segmentos[segAtual].motivo == 0){
            segmentos[segAtual].motivo = 'H';
        }
    }

    *numSegmentos = segAtual + 1;
    return 0;
}