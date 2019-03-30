/*
 * File:  ex.c
 * Author:  David Baptista 92446
 * Description: Projeto em C para gestao de eventos
*/

/*===================================================
Bibliotecas e ficheiros
===================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/*===================================================
Constantes
===================================================*/
#define DIM_INPUT 350       /* Valor maximo de caracteres que pode ser inserido */
#define DIM_VARS 64         /* Dimensao maxima que uma string com dados pode ter*/
#define DIM_PLACEHOLDER 8   /* Dimensao do array usado para separar dados recebidos */
#define DIM_ROOMS 100       /* Numero maximo de eventos por quarto */

#define NUM_PARTICIPANTS 3  /* Valor maximo de participantes por evento */
#define NUM_ROOMS 10        /* Numero maximo de quartos */

#define DELIM ":"           /* Caracter usado para delimitar os dados */
#define EMPTY ""

/*===================================================
Estruturas
===================================================*/

/* Estrutura evento, usada para representar um dado evento */
typedef struct event
{
    char descricao[DIM_VARS];
    int data;
    int inicio; 
    int duracao;
    int sala; 
    char responsavel[DIM_VARS]; 
    char participantes[NUM_PARTICIPANTS][DIM_VARS];
}Event;

/*===================================================
Variaveis globais
===================================================*/
/*  Variavel que armazena o primeiro indice livre de cada sala. 
    O valor -1 significa que a sala esta vazia */
int size[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

int getParticipants(Event event)
{
    int i;

    for(i = 0; i < NUM_PARTICIPANTS; i++)
    {
        if(!strcmp(event.participantes[i], EMPTY))
        {
            break;
        }
    }

    return i;
}

void readString(char dest[DIM_VARS])
{
    int i;
    char c;
     
    for(i = 0; i < DIM_INPUT-1 && (c=getchar()) != '\n' && c != EOF; i++)
    {   
        dest[i] = c;
    }

    dest[i] = '\0';
}

/*===================================================
Funcooes Auxiliares
===================================================*/

/*
    Descricao: Adiciona a uma variavel que contem tempo, um dado valor
    Input: temp- o tempo no formato hhmm, val- valor a somar
    Output:  tempo com o valor adicionado
*/
int addTime(int temp, int val)
{
    int horas, mins;

    mins = (temp % 100) + ((temp/100) * 60);
    mins += val;
    horas = (mins/60)*100;

    while(mins >= 60)
    {
        mins -= 60;
    }

    return horas + mins;
}

/*
    Desricao: Verifica se os valores correspondentes a 2 eventos nao entram em conflito
    Input: start - inicio do evento, end - fim do evento (formato: hhmm)
    Output: False se entrarem em conflito, True se forem tempos validos
*/
int validTime(int start1, int end1, int start2, int end2)
{
    if((start1 < start2) && (end1 <= start2))
    {
        return 1;
    }
    if((start1 >= end2) && (end1 > end2))
    {
        return 1;
    }

    return 0;
}

/*
    Descricao: Verifica se uma dada sala tem algum evento na dada data
    Input: events - agenda dos eventos; dummy - evento a validar
    Output: True se valido, false caso contrario
*/
int validRoom(Event events[NUM_ROOMS][DIM_ROOMS], Event dummy)
{
    int i;
    int r = dummy.sala-1;

    for(i = 0; i <= size[r]; i++)   
    {
        if(events[r][i].data == dummy.data && strcmp(events[r][i].descricao, dummy.descricao))
        {
            if(!validTime(events[r][i].inicio, addTime(events[r][i].inicio, events[r][i].duracao) , dummy.inicio, addTime(dummy.inicio, dummy.duracao)))
            {
                return 0;
            }
        }
    }

    return 1;
}

/*
    Descricao: Verifica se uma dada pessa esta ocupada num dado intervalo de tempo
    Input: events - agenda dos eventos; pessoa - pessoa a validar
    Output: True se for valido, false caso contrario
*/
int validPerson(Event events[NUM_ROOMS][DIM_ROOMS], Event dummy, char pessoa[DIM_VARS])
{
    int i, j;

    for(i = 0; i < NUM_ROOMS; i++)
    {
        for(j = 0; j <= size[i]; j++)
        {
            if(events[i][j].data == dummy.data && strcmp(events[i][j].descricao, dummy.descricao)
                && ((strcmp(events[i][j].responsavel, pessoa) == 0)
                || (strcmp(events[i][j].participantes[0], pessoa) == 0) 
                || (strcmp(events[i][j].participantes[1], pessoa) == 0) 
                || (strcmp(events[i][j].participantes[2], pessoa) == 0)))
            {   
                if(!validTime(events[i][j].inicio, addTime(events[i][j].inicio, events[i][j].duracao), dummy.inicio, addTime(dummy.inicio, dummy.duracao)))
                {
                    return 0;
                }               
            }
        }
    }

    return 1;
}

/*
    Descricao: Verifica se um dado evento e valido
    Input: events - agenda dos eventos; dummy - evento a validar
    Output: True se for valido, false caso contrario
*/
int validEvent(Event events[NUM_ROOMS][DIM_ROOMS], Event dummy)
{
    int i;
    int valid = 1;

    if(!validRoom(events, dummy))
    {
        printf("Impossivel agendar evento %s. Sala%d ocupada.\n", dummy.descricao, dummy.sala);

        valid = 0;

        return valid;
    }

    if(!validPerson(events, dummy, dummy.responsavel))
    {
        printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", dummy.descricao, dummy.responsavel);
        valid = 0;
    }

    for(i = 0; i < getParticipants(dummy); i++)
    { 
        if(!validPerson(events, dummy, dummy.participantes[i]))
        {
            printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", dummy.descricao, dummy.participantes[i]);
            valid = 0;
        }        
    }

    return valid;
}

/*
    Usada para determinar se a primeira data e inferior a segunda data
*/
int smallerDate(int date1, int date2)
{
    int year1, year2;
    int month1, month2;
    int day1, day2;

    year1 = date1%10000;
    year2 = date2%10000;

    date1 = date1 / 10000;
    date2 = date2 / 10000;

    month1 = date1%100;
    month2 = date2%100;

    date1 = date1 / 100;
    date2 = date2 / 100;

    day1 = date1;
    day2 = date2;
    
    if(year1 > year2)
    {
        return 1;
    }
    else if(year1 == year2 && (month1 > month2))
    {
        return 1;
    }
    else if(year1 == year2 && month1 == month2 && (day1 > day2))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
    Ordena um dado array de eventos. Limite corresponde ao numero de eventos
*/
void bubbleSort(Event arr[1000], int lim)
{
    Event dummy;

    int i, j;
    int a, b;
    int swapped = 0;

    for(i = 0; i < lim; i++)
    {
        for(j = 0; j+1 <= lim; j++)
        {
            a = arr[j].data;
            b = arr[j+1].data;

            if((smallerDate(a, b)) || (a == b && arr[j].inicio > arr[j+1].inicio))
            {
                dummy = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = dummy;
                swapped = 1;
            }
        }

        if(swapped == 0)
        {
            break;
        }
    }
}

/* 
    Escreve um dado evento na consola
*/
void writeEvent(Event event)
{
    switch(getParticipants(event))
    {
        case 1:
            printf("%s %08d %04d %d Sala%d %s\n* %s\n",
                event.descricao,
                event.data,
                event.inicio,
                event.duracao,
                event.sala,
                event.responsavel,
                event.participantes[0]);
            break;
        case 2:
            printf("%s %08d %04d %d Sala%d %s\n* %s %s\n",
                event.descricao,
                event.data,
                event.inicio,
                event.duracao,
                event.sala,
                event.responsavel,
                event.participantes[0],                
                event.participantes[1]);
            break;
        case 3:
            printf("%s %08d %04d %d Sala%d %s\n* %s %s %s\n",
                    event.descricao,
                    event.data,
                    event.inicio,
                    event.duracao,
                    event.sala,
                    event.responsavel,
                    event.participantes[0],
                    event.participantes[1],
                    event.participantes[2]);
            break;
    }
}

/*
    Remove o primeiro caracter e o espaço inserido em cada input
*/
void fixInput(char str[DIM_INPUT])
{
    int i = 0;

    while(str[i+1] != '\0')
    {
        str[i] = str[i+1];
        i++;
    }

    str[i] = '\0';
}

/*
    Funcao que determina qual dos 10 eventos se da primeiro
*/
int smallestEventIndex(int index[NUM_ROOMS], Event events[NUM_ROOMS][DIM_ROOMS])
{
    int date = 0, start = 0;
    int r, i = -1;

    for(r = 0; r < NUM_ROOMS; r++)
    {
        if(index[r] <= size[r] && ((smallerDate(events[r][index[r]].data, date) || (events[r][index[r]].data == date && events[r][index[r]].inicio < start))))
        {
            start = events[r][index[r]].inicio;
            date = events[r][index[r]].data;
            i = r;
        }
    }

    return i;
}

/*===================================================
Funcooes Principais
===================================================*/

/*
    Descricao: Verifica se uma dada pessa esta ocupada num dado intervalo de tempo. ('a')
    Input: str - dados do evento, events - array com a agenda
    Output: ---
*/
void addEvent(char str[DIM_INPUT], Event events[NUM_ROOMS][DIM_ROOMS])
{
    Event dummy;

    char placeholder[9][64];
    char *token;

    int index = 0;

    strcpy(placeholder[6], "");
    strcpy(placeholder[7], "");
    strcpy(placeholder[8], "");

    token = strtok(str, DELIM);
   
    while(token != NULL) 
    {
        strcpy(placeholder[index], token);

        token = strtok(NULL, DELIM);

        index++;
    }
    
    strcpy(dummy.descricao, placeholder[0]);
    dummy.data = atoi(placeholder[1]);
    dummy.inicio = atoi(placeholder[2]);
    dummy.duracao = atoi(placeholder[3]);
    dummy.sala = atoi(placeholder[4]);
    strcpy(dummy.responsavel, placeholder[5]);
    strcpy(dummy.participantes[0], placeholder[6]);
    strcpy(dummy.participantes[1], placeholder[7]);
    strcpy(dummy.participantes[2], placeholder[8]);

    if(size[dummy.sala-1] == 99)
    {
        return;
    }

    if(validEvent(events, dummy))
    {
        size[dummy.sala-1]++;

        events[dummy.sala-1][size[dummy.sala-1]] = dummy;
    }
}

/*
    Descricao: Escreve na consola todos os eventos por ordem cronologica. ('l')
    Input: room - numero da sala, events - agenda com os eventos
    Output: ---
*/
void showAllEvents(Event events[NUM_ROOMS][DIM_ROOMS])
{
    int p[NUM_ROOMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    int r;

    for(r = 0; r < NUM_ROOMS; r++)
    {
        bubbleSort(events[r], size[r]);
    }

    r = smallestEventIndex(p, events);

    while(r != -1)
    { 
        writeEvent(events[r][p[r]]);
        p[r]++;
        r = smallestEventIndex(p, events);
    }
}

/*
    Descricao: Escreve na consola os eventos de um quarto por ordem cronologica. ('s')
    Input: room - numero da sala, events - agenda com os eventos
    Output: ---
*/
void showRoomEvents(int room, Event events[NUM_ROOMS][DIM_ROOMS])
{
    int i;
    int r = room-1;

    bubbleSort(events[r], size[r]);

    for(i = 0; i <= size[r]; i++)
    {
        writeEvent(events[r][i]);
    }
}

/*
    Descricao: Remove um dado evento da agenda. ('r')
    Input: str - descricao do evento, events - array com a agenda
    Output: ---
*/
void deleteEvent(char desc[DIM_VARS], Event events[NUM_ROOMS][DIM_ROOMS])
{
    int r, i, j;

    int loop = 1;

    for(r = 0; r < NUM_ROOMS && loop; r++)
    {
        for(i = 0; i <= size[r] && loop; i++)
        {
            if(!strcmp(events[r][i].descricao, desc))
            {
                for(j = i; j < size[r]; j++)
                {
                    events[r][j] = events[r][j+1];
                }

                size[r]--;
                
                loop = 0;
            }
        }
    }

    if(loop)
    {
        printf("Evento %s inexistente.\n", desc);
    }
}

/*
    Descricao: Altera o inicio de um evento na agenda. 
    Input: str - descricao do evento, val - valor a alterar, events - array com a agenda
    Output: ---
*/
void editEvent(char str[DIM_INPUT], int val, Event events[NUM_ROOMS][DIM_ROOMS])
{
    Event dummy;
    
    int r, i;
    int loop;

    char *token;
    
    token = strtok(str, DELIM);
    strcpy(dummy.descricao, token);
    token = strtok(NULL, DELIM);

    loop = 1;

    for(r = 0; r < NUM_ROOMS && loop; r++)
    {
        for(i = 0; i <= size[r] && loop; i++)
        {
            if(!strcmp(events[r][i].descricao, dummy.descricao))
            {
                dummy = events[r][i];
                loop = 0;
            }  
        }
    }

    if(!loop)
    {
        switch(val)
        {
            case 1:
                dummy.inicio = atoi(token);
                break;
            case 2:
                dummy.duracao = atoi(token);
                break;
            case 3:
                dummy.sala = atoi(token);
                break;
        }

        if(val <= 3 && validEvent(events, dummy))
        {
            switch(val)
            {
                case 1:
                    events[r-1][i-1].inicio = dummy.inicio;

                    break;
                case 2:
                    events[r-1][i-1].duracao = dummy.duracao;

                    break;
                case 3:
                    deleteEvent(dummy.descricao, events);
                    
                    size[dummy.sala-1]++;
                    events[dummy.sala-1][size[dummy.sala-1]] = dummy;

                    break;
            }
        }
    }
    else
    {
        printf("Evento %s inexistente.\n", dummy.descricao);
    }
}

/*
    Adiciona um dado participante a um dado evento.
*/
void addParticipant(char str[DIM_INPUT], Event events[NUM_ROOMS][DIM_ROOMS])
{
    Event dummy;

    int r, i;
    int loop = 1;

    char *token;

    char part[DIM_VARS];

    token = strtok(str, DELIM);
    strcpy(dummy.descricao, token);
    token = strtok(NULL, DELIM);
    strcpy(part, token);

    for(r = 0; r < NUM_ROOMS && loop; r++)
    {
        for(i = 0; i <= size[r] && loop; i++)
        {
            if(!strcmp(dummy.descricao, events[r][i].descricao))
            {
                if(getParticipants(events[r][i]) == 3)
                {
                    printf("Impossivel adicionar participante. Evento %s ja tem 3 participantes.\n", dummy.descricao);
                    return;
                }

                dummy = events[r][i];

                if(!strcmp(dummy.participantes[0], part) || !strcmp(dummy.participantes[1], part) || !strcmp(dummy.participantes[2], part))
                {
                    return;
                }

                strcpy(dummy.participantes[getParticipants(dummy)], part);
                loop = 0;

                if(validPerson(events, dummy, part))
                {
                    events[r][i] = dummy;
                }  
                else
                {
                    printf("Impossivel adicionar participante. Participante %s tem um evento sobreposto.\n", dummy.participantes[getParticipants(dummy)-1]);
                }                              
            }
        }
    }

    if(loop)
    {
        printf("Evento %s inexistente.\n", dummy.descricao);
    }    
}

/*
    Caso possivel, remove um dado participante de um evento
*/
void removeParticipant(char str[DIM_INPUT], Event events[NUM_ROOMS][DIM_ROOMS])
{
    Event dummy;
    
    int r, i;
    int loop;

    int c;
    int changed = 0;

    char placeholder[DIM_VARS];
    char *token;
    
    token = strtok(str, DELIM);
    strcpy(dummy.descricao, token);
    token = strtok(NULL, DELIM);
    strcpy(placeholder, token);

    loop = 1;

    for(r = 0; r < NUM_ROOMS && loop; r++)
    {
        for(i = 0; i <= size[r] && loop; i++)
        {
            if(!strcmp(events[r][i].descricao, dummy.descricao))
            {
                dummy = events[r][i];
                loop = 0;
            }
        }
    }

    if(loop)
    {
        printf("Evento %s inexistente.\n", dummy.descricao);
    }
    else if (!loop && getParticipants(dummy) > 1)
    {
        for(c = 0; c < getParticipants(dummy); c++)
        {
            if(!strcmp(dummy.participantes[c], placeholder))
            {
                changed = 1;
                break;
            }
        }

        if(changed)
        {
            while(c+1 < getParticipants(dummy))
            {
                strcpy(dummy.participantes[c], dummy.participantes[c+1]);
                c++;
            }
            strcpy(dummy.participantes[c], EMPTY);           

            events[r-1][i-1] = dummy;
        }
    }
    else if (getParticipants(dummy) == 1 && !strcmp(placeholder, dummy.participantes[0]))
    {
        printf("Impossivel remover participante. Participante %s e o unico participante no evento %s.\n", dummy.participantes[0], dummy.descricao);
    }
}

/*===================================================
Main
===================================================*/
int main()
{    
    Event events[NUM_ROOMS][DIM_ROOMS];

    char input[DIM_INPUT];
    char op;

    while(1)
    {    
        readString(input);

        op = input[0];

        fixInput(input);
        fixInput(input);

        switch (op)
        {
            case 'a':
                addEvent(input, events);
                break;
            case 'l':
                showAllEvents(events);
                break;
            case 's':
                showRoomEvents(atoi(input), events);
                break;
            case 'r':
                deleteEvent(input, events);
                break;
            case 'i':
                editEvent(input, 1, events);
                break;
            case 't':
                editEvent(input, 2, events);
                break;
            case 'm':
                editEvent(input, 3, events);
                break;
            case 'A':
                addParticipant(input, events);
                break;
            case 'R':
                removeParticipant(input, events);
                break;
            case 'x':
                return 0;
        }
    }

    return 0;
}
