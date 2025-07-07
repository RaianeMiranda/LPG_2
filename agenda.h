#ifndef EVENTO_H
#define EVENTO_H

struct Data {
    int dia, mes, ano;
};

struct Horario {
    int hora, minuto;
};

struct Evento {
    struct Data data;
    struct Horario inicio;
    struct Horario fim;
    char descricao[100];
    char local[100];
};

struct Data le_data();
void mostra_data(struct Data d);
int compara_data_hora(struct Data a, struct Horario ha, struct Data b, struct Horario hb);
int conflito(struct Evento novo, struct Evento existente);
int mesma_data(struct Data a, struct Data b);
int mesmo_horario(struct Horario a, struct Horario b);
void le_evento(struct Evento *e);
void mostra_evento(struct Evento e);
void cadastra_evento(struct Evento **agenda, int *n);
void remove_evento(struct Evento **agenda, int *n);
void libera_agenda(struct Evento **agenda, int *n);
int compara_eventos(const void *a, const void *b);

#endif
