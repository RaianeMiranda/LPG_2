#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "agenda.h"

/* ──────────────────────────────────────────────────────────────
   Funções auxiliares para formatação de data e horário
   ────────────────────────────────────────────────────────────── */

void formatar_data(struct Data d, char *buffer) {
    sprintf(buffer, "%02d/%02d/%04d", d.dia, d.mes, d.ano);
}

void formatar_horario(struct Horario h, char *buffer) {
    sprintf(buffer, "%02d:%02d", h.hora, h.minuto);
}

/* ──────────────────────────────────────────────────────────────
   Programa principal
   ────────────────────────────────────────────────────────────── */

int main() {
    setlocale(LC_ALL, "Pt.BR.UTF-8");

    struct Evento *agenda = NULL;
    int n = 0, op;

    /* ────── Leitura do arquivo de eventos ────── */
    FILE *entrada = fopen("eventos.txt", "rt");
    if (entrada != NULL) {
        fscanf(entrada, "%d\n", &n);
        agenda = malloc(sizeof(struct Evento) * n);

        for (int i = 0; i < n; i++) {
            char buffer[128];

            fgets(buffer, sizeof(buffer), entrada); // ignora linha do índice
            fgets(buffer, sizeof(buffer), entrada);
            sscanf(buffer, "%d/%d/%d", &agenda[i].data.dia, &agenda[i].data.mes, &agenda[i].data.ano);

            fgets(buffer, sizeof(buffer), entrada);
            sscanf(buffer, "%d:%d", &agenda[i].inicio.hora, &agenda[i].inicio.minuto);

            fgets(buffer, sizeof(buffer), entrada);
            sscanf(buffer, "%d:%d", &agenda[i].fim.hora, &agenda[i].fim.minuto);

            fgets(agenda[i].descricao, sizeof(agenda[i].descricao), entrada);
            agenda[i].descricao[strcspn(agenda[i].descricao, "\n")] = 0;

            fgets(agenda[i].local, sizeof(agenda[i].local), entrada);
            agenda[i].local[strcspn(agenda[i].local, "\n")] = 0;
        }

        fclose(entrada);
        printf("%d eventos carregados do arquivo.\n", n);
    } else {
        printf("Arquivo não encontrado. Agenda iniciada vazia.\n");
    }

    /* ────── Menu interativo ────── */
     do {
        printf("1. Cadastrar evento\n");
        printf("2. Mostrar todos os eventos\n");
        printf("3. Mostrar eventos de uma data\n");
        printf("4. Mostrar eventos por descricao\n");
        printf("5. Remover evento\n");
        printf("6. Sair\n");
        printf("Opcao: ");

        if (scanf("%d", &op) != 1) {
            // Entrada inválida (ex: letras ou símbolos)
            printf("Opcao invalida.\n");

            // Limpa o buffer (descarta a entrada inválida até o '\n')
            while (getchar() != '\n');

            // Continua para repetir o menu
            continue;
        }

        switch (op) {
            case 1:
                cadastra_evento(&agenda, &n);
                break;
            case 2:
                mostra_todos_eventos(agenda, n);
                break;
            case 3:
                mostra_eventos_data(agenda, n);
                break;
            case 4:
                mostra_eventos_descricao(agenda, n);
                break;
            case 5:
                remove_evento(&agenda, &n);
                break;
            case 6:
                printf("Salvando agenda e saindo...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }

    } while (op != 6);

    /* ────── Escrita da agenda no arquivo ────── */
    if (n > 1) {
        qsort(agenda, n, sizeof(struct Evento), compara_eventos);
    }

    FILE *saida = fopen("eventos.txt", "wt");
    if (saida == NULL) {
        printf("Erro ao abrir arquivo para salvar!\n");
    } else {
        fprintf(saida, "%d\n", n);

        for (int i = 0; i < n; i++) {
            fprintf(saida, "[%d]\n", i);
            fprintf(saida, "%02d/%02d/%04d\n", agenda[i].data.dia, agenda[i].data.mes, agenda[i].data.ano);
            fprintf(saida, "%02d:%02d\n", agenda[i].inicio.hora, agenda[i].inicio.minuto);
            fprintf(saida, "%02d:%02d\n", agenda[i].fim.hora, agenda[i].fim.minuto);
            fprintf(saida, "%s\n", agenda[i].descricao);
            fprintf(saida, "%s\n", agenda[i].local);
        }

        fclose(saida);
    }

    libera_agenda(&agenda, &n);
    return 0;
}
