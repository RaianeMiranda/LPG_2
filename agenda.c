#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agenda.h"



// VERIFICAÇÃO SE DATA É EXISTENTE NO CALENDÁRIO 
int data_valida(struct Data d) {
    if (d.ano < 1 || d.mes < 1 || d.mes > 12 || d.dia < 1)
        return 0;

    int dias_no_mes[] = {31,28,31,30,31,30,31,31,30,31,30,31};

// Ano bissexto: se for divisível por 4 e não por 100, ou divisível por 400
    if ((d.ano % 4 == 0 && d.ano % 100 != 0) || (d.ano % 400 == 0))
        dias_no_mes[1] = 29;

    return d.dia <= dias_no_mes[d.mes - 1];
}

/* ──────────────────────────────────────────────────────────────
   VALIDAÇÃO DE DADOS DE DIA, MES, ANO (ERROS)
   ────────────────────────────────────────────────────────────── */

struct Data le_data() {
    struct Data d;

    while (1) {
        printf("Dia: ");
        if (scanf("%d", &d.dia) != 1 || d.dia < 1 || d.dia > 31) {
            printf("Dia invalido! Deve estar entre 1 e 31.\n");
            while (getchar() != '\n');
            continue;
        }

        printf("Mes: ");
        if (scanf("%d", &d.mes) != 1 || d.mes < 1 || d.mes > 12) {
            printf("Mes invalido! Deve estar entre 1 e 12.\n");
            while (getchar() != '\n');
            continue;
        }

        printf("Ano: ");
        if (scanf("%d", &d.ano) != 1 || d.ano < 2020) {
            printf("Ano invalido! Deve ser maior que 2020.\n");
            while (getchar() != '\n');
            continue;
        }

        if (!data_valida(d)) {
            printf("Data inválida! Verifique o dia para o mês e ano fornecidos.\n");
            continue;
        }

        break;
    }

    return d;
}

//====================================================
//DETALHAMENTO
int compara_data_hora(struct Data a, struct Horario ha, struct Data b, struct Horario hb) {
    if (a.ano != b.ano) return a.ano - b.ano;
    if (a.mes != b.mes) return a.mes - b.mes;
    if (a.dia != b.dia) return a.dia - b.dia;
    if (ha.hora != hb.hora) return ha.hora - hb.hora;
    return ha.minuto - hb.minuto;
}
int conflito(struct Evento novo, struct Evento existente) {
    return !(compara_data_hora(novo.data, novo.inicio, existente.data, existente.fim) >= 0 ||
             compara_data_hora(novo.data, novo.fim, existente.data, existente.inicio) <= 0);
}

int compara_eventos(const void *a, const void *b) {
    const struct Evento *ea = (const struct Evento *)a;
    const struct Evento *eb = (const struct Evento *)b;
    return compara_data_hora(ea->data, ea->inicio, eb->data, eb->inicio);
}

//==================================================
/* ──────────────────────────────────────────────────────────────
    1) CADASTRAR NOVO EVENTO => ENTRADA DE DADOS DOS EVENTOS
   ────────────────────────────────────────────────────────────── */
// VERIFICAR SE HORA PODE SER CADASTRADA DENTRO DE 00 - 23
int horario_valido(struct Horario h) {
    return (h.hora >= 0 && h.hora <= 23 && h.minuto >= 0 && h.minuto <= 59);
}

void le_evento(struct Evento *e) {
    printf("Digite a data do evento:\n");
    e->data = le_data();

    do {
        printf("Horario de inicio (hora): ");
        scanf("%d", &e->inicio.hora);
        printf("Horario de inicio (minuto): ");
        scanf("%d", &e->inicio.minuto);
        if (!horario_valido(e->inicio))
            printf("Horario inválido! Por favor, digite um horário válido (00:00 a 23:59).\n");
    } while (!horario_valido(e->inicio));

    do {
        printf("Horario de fim (hora): ");
        scanf("%d", &e->fim.hora);
        printf("Horario de fim (minuto): ");
        scanf("%d", &e->fim.minuto);

        if (!horario_valido(e->fim)) {
            printf("Horario inválido! Por favor, digite um horário válido (00:00 a 23:59).\n");
            continue;
        }

       if (compara_data_hora(e->data, e->fim, e->data, e->inicio) <= 0) {
    printf("O horário de fim deve ser posterior ao início.\n");
    continue;
}
        break;
    } while (1);

    getchar();

    printf("Descricao: ");
    scanf(" %[^\n]", e->descricao);
    printf("Local: ");
    scanf(" %[^\n]", e->local);
}


/* ──────────────────────────────────────────────────────────────
   FUNÇÃO PRINCIPAL DE CADASTRO COM ORDENAÇÃO CRONOLÓGICA
   ────────────────────────────────────────────────────────────── */

void cadastra_evento(struct Evento **agenda, int *n) {
    struct Evento novo;
    le_evento(&novo);

    for (int i = 0; i < *n; i++) {
        if (conflito(novo, (*agenda)[i])) {
            printf("Erro: evento conflita com outro já existente!\n");
            return;
        }
    }
    //Aumentando o vetor
    *agenda = realloc(*agenda, (*n + 1) * sizeof(struct Evento));
    if (!*agenda) {
        printf("Erro de memória!\n");
        return;
    }

    //ordenando
    (*agenda)[(*n)++] = novo;
    qsort(*agenda, *n, sizeof(struct Evento), compara_eventos);
    printf("Evento cadastrado com sucesso!\n");
}

// 2) MOSTRANDO TODOS OS EVENTOS 
void mostra_data(struct Data d) {
    printf("%02d/%02d/%04d\n", d.dia, d.mes, d.ano);
} //função pra printar struct de Data

void mostra_evento(struct Evento e) {
    mostra_data(e.data);
    printf("%02d:%02d - %02d:%02d\n", e.inicio.hora, e.inicio.minuto, e.fim.hora, e.fim.minuto);
    printf("%s\n%s\n", e.descricao, e.local);
    printf("------------------------\n");
} //Função reutilizada em todos os "mostras" para print

void mostra_todos_eventos(struct Evento *agenda, int n) {
    if (n == 0) {
        printf("Agenda vazia.\n");
        return;
    }

    printf("\n--- Todos os Eventos ---\n");
    for (int i = 0; i < n; i++) {
        printf("[%d]\n", i);
        mostra_evento(agenda[i]);
    }
}

// 3) MOSTRA EVENTOS POR DATA (usa o mostra_evento)
void mostra_eventos_data(struct Evento *agenda, int n) {
    if (n == 0) {
        printf("Agenda vazia. Nao ha eventos para listar.\n");
        return;
    }
    
    struct Data d_busca;
    printf("Digite a data que deseja buscar:\n");
    d_busca = le_data();
    
    int encontrados = 0;
    printf("\n--- Eventos em %02d/%02d/%04d ---\n", d_busca.dia, d_busca.mes, d_busca.ano);
    
    for (int i = 0; i < n; i++) {
        if (mesma_data_remove(agenda[i].data, d_busca)) {
            mostra_evento(agenda[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum evento encontrado para esta data.\n");
    }
}
// 4) MOSTRA EVENTOS POR DATA (usa o mostra_evento)
void mostra_eventos_descricao(struct Evento *agenda, int n) {
    if (n == 0) {
        printf("Agenda vazia. Não há eventos para listar.\n");
        return;
    }

    char descricao_busca[100];
    printf("Digite a descrição para buscar: ");
    getchar(); // Limpa o buffer do teclado antes de ler a string
    scanf("%[^\n]", descricao_busca);
    
    int encontrados = 0;
    printf("\n--- Eventos contendo a descrição \"%s\" ---\n", descricao_busca);
    
    for (int i = 0; i < n; i++) {
        // strstr encontra a primeira ocorrência de uma substring em outra.
        // Se não encontrar, retorna NULL.
        if (strstr(agenda[i].descricao, descricao_busca) != NULL) {
            mostra_evento(agenda[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum evento encontrado com essa descricao.\n");
    }
}


// 5) FUNÇÕES PARA COMPARAR E REMOVER EVENTO
int mesma_data_remove(struct Data a, struct Data b) {
    return a.dia == b.dia && a.mes == b.mes && a.ano == b.ano;
}

int mesmo_horario_remove(struct Horario a, struct Horario b) {
    return a.hora == b.hora && a.minuto == b.minuto;
}

// 5) FUNÇÃO DE REMOVER EVENTO
void remove_evento(struct Evento **agenda, int *n) {
    if (*n == 0) {
        printf("Nenhum evento cadastrado.\n");
        return;
    }

    struct Data d = le_data();
    struct Horario h;
    printf("Horario de inicio (hora): ");
    scanf("%d", &h.hora);
    printf("Horario de inicio (minuto): ");
    scanf("%d", &h.minuto);
    getchar();

    for (int i = 0; i < *n; i++) {
        if (mesma_data_remove(d, (*agenda)[i].data) && mesmo_horario_remove(h, (*agenda)[i].inicio)) {
            for (int j = i; j < *n - 1; j++) {
                (*agenda)[j] = (*agenda)[j + 1];
            }
            (*n)--;
            *agenda = realloc(*agenda, (*n) * sizeof(struct Evento));
            printf("Evento removido com sucesso.\n");
            return;
        }
    }
    printf("Evento nao encontrado.\n");
}

//6) SAIR DO PROGRAMA
void libera_agenda(struct Evento **agenda, int *n) {
    free(*agenda);
    *agenda = NULL;
    *n = 0;
    printf("Memoria liberada.\n");
}