#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"

int parseIntStrict(const char *str, int *out){
    if(str == NULL || out == NULL){
        return -1;
    }

    char *fimPtr; //Ponteiro para informar fim de leitura

    long num = strtol(str, &fimPtr, 10);

    if(fimPtr == str || *fimPtr != '\0'){
        fprintf(stderr, "Erro: Valor '%s' não é numérico!\n", str);
        return -1;
    }
    
    if(num <= 0){
        fprintf(stderr, "Erro: Valor '%s' deve ser maior que zero!\n", str);
        return -1;
    }

    *out = (int)num;
    return 0;
}

int parseArquivo(const char *arquivo, int *tempoTotal, Tarefa *tarefas, int *numTarefas){
    FILE *fp = fopen(arquivo, "r");

    if(fp == NULL){
        fprintf(stderr, "Erro: arquivo '%s' falhou em ser aberto!\n", arquivo);
        return -1;
    }

    char linha[256];
    int linhaAtual = 0;
    int idx = 0;

    while(fgets(linha, sizeof(linha), fp) != NULL){
        //remove newline do final
        int len = strlen(linha);
        if(len > 0 && linha[len - 1] == '\n'){
            linha[len - 1] = '\0';
        }

        //pula linhas vazias
        if(strlen(linha) == 0){
            continue;
        }

        if(linhaAtual == 0){
            //primeira linha = tempo total
            if(parseIntStrict(linha, tempoTotal) != 0){
                fclose(fp);
                return -1;
            }
        } else {
            //linhas de tarefa: NOME PERIODO DEADLINE BURST
            if(idx >= MAX_TAREFAS){
                fprintf(stderr, "Erro: numero maximo de tarefas excedido!\n");
                fclose(fp);
                return -1;
            }

            char nome[MAX_NOME];
            char periodoStr[64];
            char deadlineStr[64];
            char burstStr[64];

            int lidos = sscanf(linha, "%31s %63s %63s %63s", nome, periodoStr, deadlineStr, burstStr);

            if(lidos != 4){
                fprintf(stderr, "Erro: linha da tarefa malformada (esperado 4 campos)!\n");
                fclose(fp);
                return -1;
            }

            int periodoVal, deadlineVal, burstVal;

            if(parseIntStrict(periodoStr, &periodoVal) != 0){
                fclose(fp);
                return -1;
            }
            if(parseIntStrict(deadlineStr, &deadlineVal) != 0){
                fclose(fp);
                return -1;
            }
            if(parseIntStrict(burstStr, &burstVal) != 0){
                fclose(fp);
                return -1;
            }

            if(deadlineVal > periodoVal){
                fprintf(stderr, "Erro: tarefa '%s' com deadline (%d) maior que periodo (%d)!\n", nome, deadlineVal, periodoVal);
                fclose(fp);
                return -1;
            }
            if(burstVal > deadlineVal){
                fprintf(stderr, "Erro: tarefa '%s' com burst (%d) maior que deadline (%d)!\n", nome, burstVal, deadlineVal);
                fclose(fp);
                return -1;
            }

            strncpy(tarefas[idx].nome, nome, MAX_NOME - 1);
            tarefas[idx].nome[MAX_NOME - 1] = '\0';
            tarefas[idx].periodo = periodoVal;
            tarefas[idx].deadline = deadlineVal;
            tarefas[idx].burst = burstVal;
            tarefas[idx].ordemIndex = idx;

            idx++;
        }

        linhaAtual++;
    }

    if(linhaAtual == 0){
        fprintf(stderr, "Erro: arquivo vazio!\n");
        fclose(fp);
        return -1;
    }

    if(idx == 0){
        fprintf(stderr, "Erro: nenhuma tarefa definida no arquivo!\n");
        fclose(fp);
        return -1;
    }

    *numTarefas = idx;
    fclose(fp);
    return 0;
}

int escritaSaida(const char *algoritmo, const Tarefa *tarefas, const EstadoTarefa *estados, int numTarefas, const Segmento *segmentos, int numSegmentos){
    char nomeArquivo[256];

    if(strcmp(algoritmo, "rate") == 0){
        snprintf(nomeArquivo, sizeof(nomeArquivo), "rate_%s.out", LOGIN);
    } else {
        snprintf(nomeArquivo, sizeof(nomeArquivo), "edf_%s.out", LOGIN);
    }

    FILE *fp = fopen(nomeArquivo, "w");

    if(fp == NULL){
        fprintf(stderr, "Erro: arquivo '%s' falhou em ser criado!\n", nomeArquivo);
        return -1;
    }

    //header
    if(strcmp(algoritmo, "rate") == 0){
        fprintf(fp, "EXECUTION BY RATE\n");
    } else {
        fprintf(fp, "EXECUTION BY EDF\n");
    }

    //segmentos
    for(int i = 0; i < numSegmentos; i++){
        if(segmentos[i].nome == NULL){
            fprintf(fp, "idle for %d units\n", segmentos[i].duracao);
        } else if(segmentos[i].motivo == 0){
            fprintf(fp, "[%s] for %d units\n", segmentos[i].nome, segmentos[i].duracao);
        } else {
            fprintf(fp, "[%s] for %d units - %c\n", segmentos[i].nome, segmentos[i].duracao, segmentos[i].motivo);
        }
    }

    //linha em branco separadora
    fprintf(fp, "\n");

    //LOST DEADLINES
    fprintf(fp, "LOST DEADLINES\n");
    for(int i = 0; i < numTarefas; i++){
        fprintf(fp, "[%s] %d\n", tarefas[i].nome, estados[i].numPerdidas);
    }

    fprintf(fp, "\n");

    //COMPLETE EXECUTION
    fprintf(fp, "COMPLETE EXECUTION\n");
    for(int i = 0; i < numTarefas; i++){
        fprintf(fp, "[%s] %d\n", tarefas[i].nome, estados[i].numCompletas);
    }

    fprintf(fp, "\n");

    //KILLED
    fprintf(fp, "KILLED\n");
    for(int i = 0; i < numTarefas; i++){
        int killed = 0;
        if(estados[i].ativa){
            killed = 1;
        }
        fprintf(fp, "[%s] %d\n", tarefas[i].nome, killed);
    }

    fclose(fp);
    return 0;
}