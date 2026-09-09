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
            if(segAtual >= 0){
                //mudança: descarta segmentos vazios pra nao gerar idle for 0 units
                if(segmentos[segAtual].duracao == 0){
                    segAtual--;
                } else if(tarefaAtual >= 0 && segmentos[segAtual].motivo == 0){
                    segmentos[segAtual].motivo = 'H';
                }
            }
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

            //mudança: só seta F se motivo ainda nao tiver sido definido (evita sobrescrever L)
            if(estados[tarefaAtual].restante == 0 && segmentos[segAtual].motivo == 0){
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
        //mudança: descarta ultimo segmento vazio
        if(segmentos[segAtual].duracao == 0){
            segAtual--;
        }
    }

    *numSegmentos = segAtual + 1;
    return 0;
}