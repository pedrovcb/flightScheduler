#include "base.h"

int selecionaTarefa(const Tarefa *tarefas, const EstadoTarefa *estados, int numTarefas, const char *algoritmo){
    int melhorIndex = -1;

    for(int i = 0; i < numTarefas; i++){
        //tarefa só concorre se está ativa e tem burst restante
        if(estados[i].ativa == 0 || estados[i].restante <= 0){
            continue;
        }

        if(melhorIndex == -1){
            melhorIndex = i; //primeira tarefa pronta encontrada
            continue;
        }

        if(strcmp(algoritmo, "rate") == 0){
            //rate: menor período = maior prioridade
            if(tarefas[i].periodo < tarefas[melhorIndex].periodo){
                melhorIndex = i;
            } else if(tarefas[i].periodo == tarefas[melhorIndex].periodo){
                //desempate: menor ordemIndex
                if(tarefas[i].ordemIndex < tarefas[melhorIndex].ordemIndex){
                    melhorIndex = i;
                }
            }
        } else {
            //edf: menor deadline absoluto = maior prioridade
            if(estados[i].deadlineAbsoluto < estados[melhorIndex].deadlineAbsoluto){
                melhorIndex = i;
            } else if(estados[i].deadlineAbsoluto == estados[melhorIndex].deadlineAbsoluto){
                //desempate: menor ordemIndex
                if(tarefas[i].ordemIndex < tarefas[melhorIndex].ordemIndex){
                    melhorIndex = i;
                }
            }
        }
    }

    return melhorIndex;
}