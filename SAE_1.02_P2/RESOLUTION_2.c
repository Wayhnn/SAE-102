#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define N 4
#define TAILLE (N*N)

typedef struct {
    int valeur;
    bool candidats[TAILLE + 1];

    int nbCandidat;
} tCase2;

typedef tCase2 tabSudoku[TAILLE][TAILLE];
typedef int tabNbCandidatsParValeur[TAILLE+1]; /** Permet de stocker le nombre de fois qu'apparait un candidat dans une colonne / ligne / carré */

int nbCandidatsRetires;
int nbCasesRemplies;

void chargerGrille(tabSudoku g, char nomFichier[30], int *nbCaseVide);

void afficherGrille(tabSudoku grille);
void chercherCandidat(tabSudoku grille, int indiceLigne, int indiceColonne);
void ajouterCandidat(tCase2 *laCase, int val);
void retirerCandidat(tCase2 *laCase, int val);
void retirerCandidatsSuiteAjout(tabSudoku grille, int indiceL, int indiceCol);
void retirerTousCandidatsCase(tCase2 *laCase);
bool estCandidat(tCase2 laCase, int val);
int nbCandidats(tCase2 laCase);
void initCaseCandidatsTrue(tabSudoku grille); 
int initCandidat(tabSudoku grille);

void singleton(tabSudoku grille);

void singletonCache(tabSudoku grille);
void ajouterCandidatsCase(tCase2 laCase, tabNbCandidatsParValeur tab); /** Permet d'ajouter les candidats d'une case au tableau*/

bool backtracking(tabSudoku grille, int numeroCase);

int main()
{
    tabSudoku grille;
    char nomFichier[30];
    int nbCaseVide;
    int nbCandidatsInitial;
    int estEffectif;
    
    nbCaseVide = 0;
    chargerGrille(grille, nomFichier, &nbCaseVide);
    nbCasesRemplies = 0;


    nbCandidatsInitial = initCandidat(grille);
    nbCandidatsRetires = 0;

    printf("Grille initiale\n");
	afficherGrille(grille); // Affichage de la grille

    /** applique en boucle les différentes méthodes jusqu'à ce qu'il n'y ait plus aucune évolution */

    clock_t begin = clock();

    do
    {
        estEffectif = nbCandidatsRetires;
        // singleton(grille);
        // singletonCache(grille);

    } while (estEffectif < nbCandidatsRetires);

    backtracking(grille, 0);
    
    clock_t end = clock();
    printf("Grille finale\n");
    afficherGrille(grille);

    double tmpsCPU = (end - begin)*1.0 / CLOCKS_PER_SEC;
    printf("Résolution de la grille en %.6f sec\n",tmpsCPU);
    return EXIT_SUCCESS;
}

void chargerGrille(tabSudoku g, char nomFichier[30], int *nbCaseVide) {

    /**
     * @fn void chargerGrille(tabSudoku g)
     * 
     * @brief Charge une grille Sudoku à partir d'un fichier binaire.
     * 
     * @param g La grille vide à remplire.
     * 
     * Fonction qui demande à l'utilisateur de 
     * saisir un nom de fichier. Tant que le 
     * nom du fichier n'est pas valide, redemande 
     * la saisie. Puis remplis g en lisant le fichier
    */

    FILE * f;

    printf("Nom du fichier ? ");
    scanf("%s", nomFichier);
    f = fopen(nomFichier, "rb");

    /* Redemande la saisie jusqu'à ce que le nom du fichier soit valide */
    while (f==NULL)
    {
        printf("\nERREUR sur le fichier %s\n\n", nomFichier);
        printf("Nom du fichier ? ");
        scanf("%s",nomFichier);
        f = fopen(nomFichier, "rb");
    }
    
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            fread(&(g[i][j].valeur), sizeof(int), 1, f);
            if (g[i][j].valeur == 0)
            {
                *nbCaseVide = *nbCaseVide +1;
            }
            
        }
    }

    fclose(f);
}

void chercherCandidat(tabSudoku grille, int indiceLigne, int indiceColonne)
{
    int indiceInitLigne;  /** indice de la ligne en haut à gauche du carré où se trouve la case */
    int indiceInitCol;  /** indice de la colonne en haut à gauche du carré où se trouve la case */

    /** Initialisation */

    indiceInitLigne = N * (int)(indiceLigne/N);
    indiceInitCol = N * (int)(indiceColonne/N);

    /** vérification des sous-tableaux */
    for (int i = indiceInitLigne; i < indiceInitLigne + N; i++)
    {
        for (int j = indiceInitCol; j < indiceInitCol + N; j++)
        {
            if (grille[i][j].valeur != 0 && estCandidat(grille[indiceLigne][indiceColonne], grille[i][j].valeur))
            {
                retirerCandidat(&grille[indiceLigne][indiceColonne], grille[i][j].valeur);
            }
        }
    }

    /** vérification des lignes */

    for (int i = 0; i < TAILLE; i++)
    {
        if(grille[i][indiceColonne].valeur != 0 && estCandidat(grille[indiceLigne][indiceColonne], grille[i][indiceColonne].valeur))
        {
            retirerCandidat(&grille[indiceLigne][indiceColonne], grille[i][indiceColonne].valeur);
        }
    }
    

    /** vérification des colonnes */

    for (int i = 0; i < TAILLE; i++)
    {
        if(grille[indiceLigne][i].valeur != 0 && estCandidat(grille[indiceLigne][indiceColonne], grille[indiceLigne][i].valeur))
        {
            retirerCandidat(&grille[indiceLigne][indiceColonne], grille[indiceLigne][i].valeur);
        }
    }
}


void afficherGrille(tabSudoku grille)
{

    /**
     * @fn void afficherGrille(tabSudoku grille)
     * 
     * @brief Affiche la grille Sudoku.
     * 
     * @param grille La grille Sudoku à afficher.
     * 
     * Fonction qui affiche et met en page la grille de Sudoku. 
     * Permet de bien afficher chaque ligne/colonne mais également 
     * de séparer les carrés de N*N. Ajoute également des indices 
     * en face de chaque ligne et de chaque colonne. 
    */

    int indiceSousTab = N;
    
    for(int i = 0; i < TAILLE ; i++)
    {
        if (i%indiceSousTab == 0)
        {
            /** affiche les séparations horizontales*/
            printf(" +");
            for (int k = 1; k <= TAILLE; k++)
            {
                printf("---");
                if (k% (int)N == 0)
                {
                    printf("-+");
                }
                
            }
            printf("\n");
        }

        for(int j = 0; j < TAILLE; j++)
        {
            if (j%indiceSousTab == 0)
            {
                printf(" |"); /** affiche les séparations verticales */
            }
            
            if(grille[i][j].valeur != 0 )
            {
                printf(" %2d",grille[i][j].valeur);
            }
            else
            {
                printf("  ."); /** Remplace les 0 par des . (pour l'affichage uniquement) */
            }
        }
        printf(" |\n");
    }

    /** affiche la séparation verticale de fin*/
    printf(" +");
    for (int k = 1; k <= TAILLE; k++)
    {
        printf("---");
        if (k% (int)N == 0)
        {
            printf("-+");
        }
                
    }
    printf("\n\n");
}

void ajouterCandidat(tCase2 *laCase, int val)
{
    (*laCase).candidats[val] = true;
    (*laCase).nbCandidat ++;  
}

void retirerCandidat(tCase2 *laCase, int val)
{
    nbCandidatsRetires ++;
    (*laCase).candidats[val] = false;
    (*laCase).nbCandidat --;
}

void retirerTousCandidatsCase(tCase2 *laCase)
{
    /** Permet de supprimer tous les candidats d'une case*/
    for (int i = 1; i < TAILLE+1; i++)
    {
        if (estCandidat(*laCase, i))
        {
            retirerCandidat(laCase, i);
        }
    }
}

bool estCandidat(tCase2 laCase, int val)
{
    return laCase.candidats[val];
}

int nbCandidats(tCase2 laCase)
{
    return laCase.nbCandidat;
}

void retirerCandidatsSuiteAjout(tabSudoku grille, int indiceL, int indiceCol)
{
    int indiceInitLigne;  /** indice de la ligne en haut à gauche du carré où se trouve la case */
    int indiceInitCol;  /** indice de la colonne en haut à gauche du carré où se trouve la case */

    /** Initialisation */

    indiceInitLigne = N * (int)(indiceL/N);
    indiceInitCol = N * (int)(indiceCol/N);

    /** vérification des sous-tableaux */
    for (int i = indiceInitLigne; i < indiceInitLigne + N; i++)
    {
        for (int j = indiceInitCol; j < indiceInitCol + N; j++)
        {
            if (grille[i][j].valeur == 0 && estCandidat(grille[i][j], grille[indiceL][indiceCol].valeur))
            {
                retirerCandidat(&grille[i][j], grille[indiceL][indiceCol].valeur);
            }
        }
    }

    /** vérification des colonnes */

    for (int i = 0; i < TAILLE; i++)
    {
        if(grille[i][indiceCol].valeur == 0 && estCandidat(grille[i][indiceCol], grille[indiceL][indiceCol].valeur))
        {
            retirerCandidat(&grille[i][indiceCol], grille[indiceL][indiceCol].valeur);
        }
    }
    

    /** vérification des lignes */

    for (int i = 0; i < TAILLE; i++)
    {
        if(grille[indiceL][i].valeur == 0 && estCandidat(grille[indiceL][i], grille[indiceL][indiceCol].valeur))
        {
            retirerCandidat(&grille[indiceL][i], grille[indiceL][indiceCol].valeur);
        }
    }
}

void initCaseCandidatsTrue(tabSudoku grille)
{
    /** Permet d'initialiser tous les candidats de toutes les cases à True*/
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            if (grille[i][j].valeur == 0)
            {    
                for (int k = 1; k < TAILLE +1; k++)
                {
                    grille[i][j].candidats[k] = true;
                }
                grille[i][j].nbCandidat = TAILLE;
            }
        }
    }
}

int initCandidat(tabSudoku grille)
{

    /** initialise les candidats des cases*/
    int nbCandidatsInitial = 0;

    initCaseCandidatsTrue(grille);
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            if (grille[i][j].valeur == 0)
            {
                chercherCandidat(grille, i,j);
                nbCandidatsInitial += nbCandidats(grille[i][j]);
            }
            else
            {
                retirerTousCandidatsCase(&grille[i][j]);
            }
        }
    }
    return nbCandidatsInitial;
}

void singleton(tabSudoku grille)
{
    /** Parcourt la grille et attribut les valeurs des cases 
     * ayant un seul candidats, puis met à jour les candidats 
     * des autres cases*/

    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            if (grille[i][j].valeur == 0)
            {
                if (grille[i][j].nbCandidat == 1)
                {
                    nbCasesRemplies++;
                    int cdt = 1;
                    bool trouver = false;
                    while (cdt < TAILLE+1 && !trouver)
                    {
                        if (estCandidat(grille[i][j], cdt))
                        {
                            grille[i][j].valeur = cdt;
                            retirerCandidat(&grille[i][j],cdt);
                            retirerCandidatsSuiteAjout(grille,i,j);
                            trouver = true;
                        }
                        cdt++;
                    }
                }
            }
        }
    }
}


void singletonCache(tabSudoku grille) 
{
    tabNbCandidatsParValeur nbCandidatsParValeur = {0,0,0,0,0,0,0,0,0,0};

    int indiceInitLigne;  /** indice de la ligne en haut à gauche du carré où se trouve la case */
    int indiceInitCol;  /** indice de la colonne en haut à gauche du carré où se trouve la case */

    for (int k = 0; k < TAILLE; k++)
    {

        /** Initialisation */

        indiceInitLigne = N*(int)(k/N);
        indiceInitCol = k%N*N;

        /**--------------- Vérification des carrés -----------*/
        for (int i = indiceInitLigne; i < indiceInitLigne + N; i++)
        {
            for (int j = indiceInitCol; j < indiceInitCol + N; j++)
            {
                if (grille[i][j].valeur == 0)
                {
                    ajouterCandidatsCase(grille[i][j], nbCandidatsParValeur);
                }
            }
        }

        /** Met à jour les cases si des singletons cachés ont été trouvés*/
        for (int i = 1; i < TAILLE +1; i++)
        {
            if (nbCandidatsParValeur[i] == 1)
            {
                for (int j = indiceInitLigne; j < indiceInitLigne + N; j++)
                {
                    for (int k = indiceInitCol; k < indiceInitCol + N; k++)
                    {
                        if(grille[j][k].valeur == 0 && estCandidat(grille[j][k], i))
                        {
                            grille[j][k].valeur = i;
                            nbCasesRemplies ++;
                            retirerTousCandidatsCase(&grille[j][k]);
                            retirerCandidatsSuiteAjout(grille,j,k);
                        }
                    }
                }
            }
            nbCandidatsParValeur[i] = 0;
        }
    }

    /**--------------- Vérification des colonnes -----------*/
    for (int k = 0; k < TAILLE; k++)
    {
        for (int i = 0; i < TAILLE; i++)
        {
            if(grille[i][k].valeur == 0)
            {
                ajouterCandidatsCase(grille[i][k], nbCandidatsParValeur);
            }
        }

        /** Met à jour les cases si des singletons cachés ont été trouvés*/
        for (int i = 1; i < TAILLE +1; i++)
        {
            if (nbCandidatsParValeur[i] == 1)
            {
                for (int j = 0; j < TAILLE; j++)
                {
                    if(grille[j][k].valeur == 0 && estCandidat(grille[j][k], i))
                    {
                        grille[j][k].valeur = i;
                        nbCasesRemplies ++;
                        retirerTousCandidatsCase(&grille[j][k]);
                        retirerCandidatsSuiteAjout(grille,j,k);
                    }
                }
            }
            nbCandidatsParValeur[i] = 0;
        }
    }
            
    for (int k = 0; k < TAILLE; k++)
    {
        /**--------------- Vérification des lignes -----------*/

        for (int i = 0; i < TAILLE; i++)
        {
            if(grille[k][i].valeur == 0)
            {
                ajouterCandidatsCase(grille[k][i], nbCandidatsParValeur);
            }
        }

        /** Met à jour les cases si des singletons cachés ont été trouvés*/
        for (int i = 1; i < TAILLE +1; i++)
        {
            if (nbCandidatsParValeur[i] == 1)
            {
                for (int j = 0; j < TAILLE; j++)
                {
                    if(grille[k][j].valeur == 0 && estCandidat(grille[k][j], i))
                    {
                        grille[k][j].valeur = i;
                        nbCasesRemplies ++;
                        retirerTousCandidatsCase(&grille[k][j]);
                        retirerCandidatsSuiteAjout(grille,k,j);
                    }
                }
            }
            nbCandidatsParValeur[i] = 0;
        }
    }
}

void ajouterCandidatsCase(tCase2 laCase, tabNbCandidatsParValeur tab)
{
    /** Permet de compter les candidats d'une case et de les ajouter à "tab"*/
    int cdt = 1;
    int nbCandidatTrouve = 0;
    while (cdt < TAILLE+1 && nbCandidatTrouve < laCase.nbCandidat)
    {
        if (estCandidat(laCase, cdt))
        {
            tab[cdt] = tab[cdt] + 1;
            nbCandidatTrouve ++;
        }
        cdt++;
        
    }
}


bool backtracking(tabSudoku grille, int numeroCase) {
    int ligne, colonne;
    bool resultat = false;

    if (numeroCase == TAILLE*TAILLE) {
        resultat = true;
    } else {
        ligne = numeroCase / TAILLE;
        colonne = numeroCase % TAILLE;
        if (grille[ligne][colonne].valeur != 0) {
            resultat = backtracking(grille, numeroCase+1);
        } else {
            for (int valeur=1 ; valeur < TAILLE +1; valeur++) {
                if (estCandidat(grille[ligne][colonne], valeur)) {
                    grille[ligne][colonne].valeur = valeur;

                    if (backtracking(grille, numeroCase+1) == true) {
                        resultat = true;
                    } else {
                        grille[ligne][colonne].valeur = 0;
                    }
                }
            }
        }
    }
    return resultat;
}