#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
//#include <time.h>
#define MAX_CHAR 600
#define true 1
#define false 0

typedef struct Personne {
    char nom[30];
    char prenom[30];
    char email[50];
    char ville[30];
    char pays[30];
} Personne;

/* prendre le chemin de la BDD et renvoie une liste des chaine de caracteres */
char** lire(char* jsonbdd, int* n)
{
    FILE * bdd = fopen(jsonbdd, "r");
    char** personnes = malloc(1 * sizeof(char*));

    char c;
    *n = 0;
    int k;

    c = fgetc(bdd); // == '['
    while(1)
    {
        while(c != '{')
        {
            if(c == EOF) return personnes;
            c = fgetc(bdd);
        }
        c = fgetc(bdd); //  == '{'

        personnes = realloc(personnes, ((*n) + 1) * sizeof(char*));
        personnes[*n] = malloc(MAX_CHAR * sizeof(char));
        k = 0;
        while(c != '}') // remplire l'element personnes[i]
        {
            if(c != '\t' && c != '\n') personnes[*n][k++] = c;
            c = fgetc(bdd);
        }
        personnes[*n][k] = '\0';

        (*n)++;
    }
}

/* prendre une chaine de caractéres sous forme : ` "cle": "valeur" `
    et extraire "valeur" dans la variable *valeur */
char* extValeur(char* valeur, char* attr)
{
    int curseur;
    int k;
    char c;
    curseur = 0;
    c = attr[curseur];
    while(c != ':') c = attr[++curseur];
    while(c != '"') c = attr[++curseur];
    c = attr[++curseur]; // == '"'

    k = 0;
    while(1)
    {
        c = attr[curseur++];
        if(c == '"') break;
        valeur[k++] = c;
    }
    valeur[k] = '\0';
}

/* prendre un objet JSON et le transforme en struct Person */
Personne initPersonne(char* personne)
{
    Personne p;
    int i;
    int k;
    int curseur = 0;
    char arr[5][70];
    char c;
    c = personne[curseur];

    /* découper l'objet sur une liste de chaine de caracteres
    dont les elements sont sous forme ` "cle": "valeur" ` */
    for(i = 0; i<5; i++)
    {
        k = 0;
        while(c != ',' && c != '\0')
        {
            arr[i][k++] = c;
            c = personne[++curseur];
        }
        arr[i][k] = '\0';
        c = personne[++curseur];
    }

    extValeur(p.nom, arr[0]);
    extValeur(p.prenom, arr[1]);
    extValeur(p.email, arr[2]);
    extValeur(p.ville, arr[3]);
    extValeur(p.pays, arr[4]);

    return p;
}

/* Prendre le chemin de la BDD et charger tous données JSON dans une liste de struct Person */
Personne* charger(char* chemin, int* n) {

    int i;
    char** objets = lire(chemin, n);
    Personne* personnes = malloc(*n * sizeof(Personne));

    for(i=0; i<*n; i++)
        *(personnes + i) = initPersonne(objets[i]);

    return personnes;
}

/* Calculer la distance de Levenstein */
int levDistance(const char * word1, const char * word2)
{
    // on soustrait 1 de len1 pcq on veut ignorer le '\n' a la fin de la chaine de caracteres de la requete
    int len1 = strlen(word1) - 1;
    int len2 = strlen(word2);

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

            if (c1 == c2 || c1 + 32 == c2 || c1 == c2 + 32) {
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
int comparaison(Personne *p, Personne *q)
{
    if(
        (p->nom[0] == '\n' || !levDistance(p->nom, q->nom)) &&
        (p->prenom[0] == '\n' || !levDistance(p->prenom, q->prenom)) &&
        (p->email[0] == '\n' || !levDistance(p->email, q->email)) &&
        (p->ville[0] == '\n' || !levDistance(p->ville, q->ville)) &&
        (p->pays[0] == '\n' || !levDistance(p->pays, q->pays))
       )
        return true;
    else return false;
}

void rechercher(Personne* personnes, int n, int (*cpr)(Personne, Personne))
{
    int i, compteur = 1;
    int aucune = true;
    Personne requete;
    do{
        printf("remplir au minimum un champ pour chercher.\n");
        printf("nom: ");
        fgets(requete.nom, 100, stdin);
        printf("prenom: ");
        fgets(requete.prenom, 100, stdin);
        printf("email: ");
        fgets(requete.email, 100, stdin);
        printf("ville: ");
        fgets(requete.ville, 100, stdin);
        printf("pays: ");
        fgets(requete.pays, 100, stdin);
        system("cls");
    } while(
            (requete.nom[0] == '\n') &&
            (requete.prenom[0] == '\n') &&
            (requete.email[0] == '\n') &&
            (requete.ville[0] == '\n') &&
            (requete.pays[0] == '\n')
             );

    printf("RESULTATS:\n\n");
//    time_t t;
//    t = clock();
    for(i=0; i<n; i++)
    {
        if(cpr(requete, personnes[i])) {
            aucune = false;
            afficherUn(personnes[i], compteur++);
        }
    }
//    t = clock() - t;
//    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
//    printf("searching took %f seconds to execute \n", time_taken);

    if(aucune) printf("Aucune correspondance trouvee.\n\n\n");
    printf("\n\n\nFrapper une touche pour retourner au menu..");
    getch();
    system("cls");
}

void afficherUn(Personne personne, int compteur)
{
    printf("%d-nom: %s, prenom: %s, email: %s, ville: %s, pays: %s\n",
            compteur, personne.nom, personne.prenom, personne.email,
            personne.ville, personne.pays);
}

void afficherTous(Personne* personnes, int n)
{
    int i;
    for(i=0; i<n; i++)
        afficherUn(personnes[i], i+1);

    printf("\n\n\nFrapper une touche pour retourner au menu..");
    getch();
    system("cls");
}

int main()
{
    int n, choix;
//    time_t t;
//    t = clock();
    Personne* personnes = charger("BDD.json", &n);
//    t = clock() - t;
//    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
//    printf("charging took %f seconds to execute \n", time_taken);

    menu:
    do {
        printf("Bonjour a la base de donnees JSON.\nElle contient les informations de %d personnes.\n\n", n);
        printf("1. Afficher tous les personnes\n");
        printf("2. Rechercher\n\n\n");
        printf("0. Quitter\n");
        printf("votre choix: ");
        scanf("%d", &choix);
        system("cls");
    } while(choix != 1 &&
            choix != 2 &&
            choix != 0);
    fflush(stdin);

    switch(choix) {
        case 1:
            afficherTous(personnes, n);
            goto menu;
            break;
        case 2:
            rechercher(personnes, n, comparaison);
            goto menu;
            break;
        case 0:
            printf("Merci pour utiliser notre logiciel :)\n\n\n");
            getch();
            exit(0);
            break;
    }
    return 0;
}
