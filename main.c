#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#define MAX_CHAR 600
#define true 1
#define false 0

typedef struct Person {
    char nom[30];
    char prenom[30];
    char email[50];
    char ville[30];
    char pays[30];
} Person;

/* prendre le chemin de la BDD et renvoie une liste des chaine de caracteres */
char** lire(char* jsonfile, int* n)
{
    FILE * db = fopen(jsonfile, "r");
    char** persons = malloc(1 * sizeof(char*));

    char c;
    *n = 0;
    int k;

    c = fgetc(db); // == '['
    while(1)
    {
        while(c != '{')
        {
            if(c == EOF) return persons;
            c = fgetc(db);
        }
        c = fgetc(db); //  == '{'

        persons = realloc(persons, ((*n) + 1) * sizeof(char*));
        persons[*n] = malloc(MAX_CHAR * sizeof(char));
        k = 0;
        while(c != '}') // remplire l'element persons[i]
        {
            if(c != '\t' && c != '\n') persons[*n][k++] = c;
            c = fgetc(db);
        }
        persons[*n][k] = '\0';

        (*n)++;
    }
}

/* prendre une chaine de caractéres sous forme : ` "key": "value" ` et extraire "value" dans la variable *value */
char* getValue(char* value, char* attr)
{
    int cursor;
    int k;
    char c;
    cursor = 0;
    c = attr[cursor];
    while(c != ':') c = attr[++cursor];
    while(c != '"') c = attr[++cursor];
    c = attr[++cursor]; // == '"'

    k = 0;
    while(1)
    {
        c = attr[cursor++];
        if(c == '"') break;
        value[k++] = c;
    }
    value[k] = '\0';
}

/* prendre un objet JSON et le transforme en struct Person */
Person initPersonne(char* person)
{
    Person p;
    int i;
    int k;
    int cursor = 0;
    char arr[5][70];
    char c;
    c = person[cursor];

    /* découper l'objet sur une liste de chaine de caracteres dont les elements sont sous forme ` "key": "value" ` */
    for(i = 0; i<5; i++)
    {
        k = 0;
        while(c != ',' && c != '\0')
        {
            arr[i][k++] = c;
            c = person[++cursor];
        }
        arr[i][k] = '\0';
        c = person[++cursor];
    }

    getValue(p.nom, arr[0]);
    getValue(p.prenom, arr[1]);
    getValue(p.email, arr[2]);
    getValue(p.ville, arr[3]);
    getValue(p.pays, arr[4]);

    return p;
}

/* Prendre le chemin de la BDD et charger tous données JSON dans une liste de struct Person */
Person* charger(char* chemin, int* n) {

    int i;
    char** objects = lire(chemin, n);
    Person* persons = malloc(*n * sizeof(Person));

    for(i=0; i<*n; i++)
        *(persons + i) = initPersonne(objects[i]);

    return persons;
}

/* Calculer la distance de Levenstein */
int levDistance(const char * word1, int len1, const char * word2, int len2)
{
    int matrix[len1 + 1][len2 + 1];
    int i;

    /* Cas où j=0, donc min(i,j) = 0 alors lev(i, j) = max(i,j) = i */
    for (i = 0; i <= len1; i++) {
        matrix[i][0] = i;
    }

    /* Cas où i=0, donc min(i,j) = 0 alors lev(i, j) = max(i,j) = j */
    for (i = 0; i <= len2; i++) {
        matrix[0][i] = i;
    }

    for (i = 1; i <= len1; i++) {
        int j;
        char c1;

        c1 = word1[i-1];
        for (j = 1; j <= len2; j++) {
            char c2;

            c2 = word2[j-1];

            if (c1 == c2) {
                matrix[i][j] = matrix[i-1][j-1];
            }
            else {
                int delete;
                int insert;
                int substitute;
                int minimum;

                delete = matrix[i-1][j] + 1;
                insert = matrix[i][j-1] + 1;
                substitute = matrix[i-1][j-1] + 1;
                minimum = delete;
                if (insert < minimum) {
                    minimum = insert;
                }
                if (substitute < minimum) {
                    minimum = substitute;
                }
                matrix[i][j] = minimum;
            }
        }
    }
    return matrix[len1][len2];
}

/* cette fonction compare deux elements struct Person */
int comparing(Person *p, Person *q)
{
    if(
       // on soustrait -1 de strlen(p->attr) pcq on veut ignorer le '\n' a la fin de la chaine de caracteres de la requete
        (p->nom[0] == '\n' || !levDistance(p->nom, strlen(p->nom) - 1, q->nom, strlen(q->nom))) &&
        (p->prenom[0] == '\n' || !levDistance(p->prenom, strlen(p->prenom) - 1, q->prenom, strlen(q->prenom))) &&
        (p->email[0] == '\n' || !levDistance(p->email, strlen(p->email) - 1, q->email, strlen(q->email))) &&
        (p->ville[0] == '\n' || !levDistance(p->ville, strlen(p->ville) - 1, q->ville, strlen(q->ville))) &&
        (p->pays[0] == '\n' || !levDistance(p->pays, strlen(p->pays) - 1, q->pays, strlen(q->pays)))
       )
        return true;
    else return false;
}

void searching(Person* persons, int n, int (*cpr)(Person, Person))
{
    int i;
    int none = true;
    Person query;
    do{
        printf("remplir au minimum un champ pour chercher.\n");
        printf("nom: ");
        fgets(query.nom, 100, stdin);
        printf("prenom: ");
        fgets(query.prenom, 100, stdin);
        printf("email: ");
        fgets(query.email, 100, stdin);
        printf("ville: ");
        fgets(query.ville, 100, stdin);
        printf("pays: ");
        fgets(query.pays, 100, stdin);
        system("cls");
    } while(
            (query.nom[0] == '\n') &&
            (query.prenom[0] == '\n') &&
            (query.email[0] == '\n') &&
            (query.ville[0] == '\n') &&
            (query.pays[0] == '\n')
             );

    printf("RESULTATS:\n\n");
    for(i=0; i<n; i++)
    {
        if(cpr(query, persons[i])) {
            none = false;
            afficherUn(persons[i]);
        }
    }
    if(none) printf("Aucune correspondance trouvee.\n\n\n");
    printf("\n\n\nFrapper une touche pour retourner au menu..");
    getch();
    system("cls");
}

void afficherUn(Person person)
{
    printf("->nom: %s, prenom: %s, email: %s, ville: %s, pays: %s\n",
            person.nom, person.prenom, person.email,
            person.ville, person.pays);
}

void afficherTous(Person* persons, int n)
{
    int i;
    for(i=0; i<n; i++)
        afficherUn(persons[i]);

    printf("\n\n\nFrapper une touche pour retourner au menu..");
    getch();
    system("cls");
}

int main()
{
    int n, choice;
    Person* persons = charger("BDD.json", &n);
    mainmenu:
    do {
        printf("Bonjour a la base de donnees JSON.\nElle contient les informations de %d personnes.\n\n", n);
        printf("1. Afficher tous les personnes\n");
        printf("2. Rechercher\n");
        printf("votre choix: ");
        scanf("%d", &choice);
        system("cls");
    } while(choice != 1 &&
            choice != 2);
    fflush(stdin);

    switch(choice) {
        case 1:
            afficherTous(persons, n);
            goto mainmenu;
        break;
        case 2:
            searching(persons, n, comparing);
            goto mainmenu;
        break;
    }
    return 0;
}
