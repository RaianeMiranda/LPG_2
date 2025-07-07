#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agenda.h"

/* ──────────────────────────────────────────────────────────────
   Validação de data e horário
   ────────────────────────────────────────────────────────────── */

int data_valida(struct Data d) {
    if (d.ano < 1 || d.mes < 1 || d.mes > 12 || d.dia < 1)
        return 0;

    int dias_no_mes[] = {31,28,31,30,31,30,31,31,30,31,30,31};

    if ((d.ano % 4 == 0 && d.ano % 100 != 0) || (d.ano % 400 == 0))
        dias_no_mes[1] = 29;

    if (d.dia > dias_no_mes[d.mes - 1])
        return 0;

    return 1;
}

int horario_valido(struct Horario h) {
    return (h.hora >= 0 && h.hora <= 23 && h.minuto >= 0 && h.minuto <= 59);
}

/* ──────────────────────────────────────────────────────────────
   Entrada de data com validação por campo (dia, mês, ano)
   ────────────────────────────────────────────────────────────── */

struct Data le_data() {
    struct Data d;

    do {
        printf("Dia: ");
        if (scanf("%d", &d.dia) != 1) {
            printf("Entrada invalida! Digite um número.\n");
            while (getchar() != '\n');
            d.dia = 0;
            continue;
        }
        if (d.dia < 1 || d.dia > 31)
            printf("Dia invalido! Deve estar entre 1 e 31.\n");
    } while (d.dia < 1 || d.dia > 31);

    do {
        printf("Mes: ");
        if (scanf("%d", &d.mes) != 1) {
            printf("Entrada invalida! Digite um número.\n");
            while (getchar() != '\n');
            d.mes = 0;
            continue;
        }
        if (d.mes < 1 || d.mes > 12)
            printf("Mes invalido! Deve estar entre 1 e 12.\n");
    } while (d.mes < 1 || d.mes > 12);

    do {
        printf("Ano: ");
        if (scanf("%d", &d.ano) != 1) {
            printf("Entrada invalida! Digite um número.\n");
            while (getchar() != '\n');
            d.ano = 0;
            continue;
        }
        if (d.ano < 2020)
            printf("Ano invalido! Deve ser maior que 2020.\n");
    } while (d.ano < 2020);

    if (!data_valida(d)) {
        printf("Data inválida! Verifique o dia para o mês e ano fornecidos.\n");
        return le_data();
    }

    return d;
}

/* ──────────────────────────────────────────────────────────────
   Utilitários para comparação e exibição
   ────────────────────────────────────────────────────────────── */

void mostra_data(struct Data d) {
    printf("%02d/%02d/%04d \n", d.dia, d.mes, d.ano);
}

int compara_data_hora(struct Data a, struct Horario ha, struct Data b, struct Horario hb) {
    if (a.ano != b.ano) return a.ano - b.ano;
    if (a.mes != b.mes) return a.mes - b.mes;
    if (a.dia != b.dia) return a.dia - b.dia;
    if (ha.hora != hb.hora) return ha.hora - hb.hora;
    return ha.minuto - hb.minuto;
}

int mesma_data(struct Data a, struct Data b) {
    return a.dia == b.dia && a.mes == b.mes && a.ano == b.ano;
}

int mesmo_horario(struct Horario a, struct Horario b) {
    return a.hora == b.hora && a.minuto == b.minuto;
}

int validacao(struct Evento novo, struct Evento existente) {
    if (compara_data_hora(novo.data, novo.inicio, existente.data, existente.fim) >= 0 ||
        compara_data_hora(novo.data, novo.fim, existente.data, existente.inicio) <= 0)
        return 0;
    return 1;
}

int compara_eventos(const void *a, const void *b) {
    const struct Evento *ea = (const struct Evento *)a;
    const struct Evento *eb = (const struct Evento *)b;
    return compara_data_hora(ea->data, ea->inicio, eb->data, eb->inicio);
}

/* ──────────────────────────────────────────────────────────────
   Entrada e exibição de eventos
   ────────────────────────────────────────────────────────────── */

void le_evento(struct Evento *e) {
    printf("Digite a data do evento:\n");
    e->data = le_data();

    do {
        printf("Horario de inicio (hora minuto): ");
        scanf("%d %d", &e->inicio.hora, &e->inicio.minuto);
        if (!horario_valido(e->inicio))
            printf("Horario inválido! Por favor, digite um horário válido (00:00 a 23:59).\n");
    } while (!horario_valido(e->inicio));

    do {
        printf("Horario de fim (hora minuto): ");
        scanf("%d %d", &e->fim.hora, &e->fim.minuto);
        if (!horario_valido(e->fim))
            printf("Horario inválido! Por favor, digite um horário válido (00:00 a 23:59).\n");
    } while (!horario_valido(e->fim));

    getchar();

    printf("Descricao: ");
    scanf(" %[^\n]", e->descricao);
    printf("Local: ");
    scanf(" %[^\n]", e->local);
}

void mostra_evento(struct Evento e) {
    mostra_data(e.data);
    printf("%02d:%02d - %02d:%02d \n", e.inicio.hora, e.inicio.minuto, e.fim.hora, e.fim.minuto);
    printf("%s \n", e.descricao);
    printf("%s \n", e.local);
    printf("------------------------\n");
}

/* ──────────────────────────────────────────────────────────────
   Cadastro, remoção e liberação de eventos
   ────────────────────────────────────────────────────────────── */

void cadastra_evento(struct Evento **agenda, int *n) {
    struct Evento novo;
    le_evento(&novo);

    for (int i = 0; i < *n; i++) {
        if (validacao(novo, (*agenda)[i])) {
            printf("Erro: evento conflita com outro ja existente!\n");
            return;
        }
    }

    *agenda = realloc(*agenda, (*n + 1) * sizeof(struct Evento));
    if (*agenda == NULL) {
        printf("Erro de memoria!\n");
        return;
    }

    (*agenda)[*n] = novo;
    (*n)++;
    qsort(*agenda, *n, sizeof(struct Evento), compara_eventos);
    printf("Evento cadastrado com sucesso!\n");
}

void remove_evento(struct Evento **agenda, int *n) {
    if (*n == 0) {
        printf("Nenhum evento cadastrado.\n");
        return;
    }

    struct Data d;
    struct Horario h;

    printf("Digite a data do evento a remover:\n");
    d = le_data();

    printf("Horario de inicio (hora minuto): ");
    scanf("%d %d", &h.hora, &h.minuto);
    getchar();

    int i, achou = 0;
    for (i = 0; i < *n; i++) {
        if (mesma_data(d, (*agenda)[i].data) && mesmo_horario(h, (*agenda)[i].inicio)) {
            achou = 1;
            break;
        }
    }

    if (!achou) {
        printf("Evento nao encontrado.\n");
        return;
    }

    for (int j = i; j < *n - 1; j++) {
        (*agenda)[j] = (*agenda)[j + 1];
    }

    (*n)--;
    *agenda = realloc(*agenda, (*n) * sizeof(struct Evento));
    printf("Evento removido com sucesso.\n");
}

void libera_agenda(struct Evento **agenda, int *n) {
    if (*agenda != NULL) {
        free(*agenda);
        *agenda = NULL;
        *n = 0;
        printf("Memoria liberada.\n");
    }
}
