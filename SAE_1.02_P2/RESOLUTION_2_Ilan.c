#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define N 4
#define TAILLE (N*N)

typedef struct {
    int c_valeur;
    int c_candidats[TAILLE];
    int c_nbCandidats;
} tCase1;

typedef tCase1 tabSudoku[TAILLE][TAILLE];
typedef int tabNbCandidatsParValeur[TAILLE+1]; /** Permet de stocker le nombre de fois qu'apparait un candidat dans une colonne / ligne / carré */

int nbCandidatsRetires;
int nbCasesRemplies;


void chargerGrille(tabSudoku g, char nomFichier[30], int *nbCaseVide);
void afficherGrille(tabSudoku grille);
void chercherCandidat(tabSudoku grille, int indiceLigne, int indiceColonne);
void ajouterCandidat(tCase1 *laCase, int val);
void retirerCandidat(tCase1 *laCase, int val);
void retirerCandidatsSuiteAjout(tabSudoku grille, int indiceL, int indiceCol);
void retirerTousCandidatsCase(tCase1 *laCase);
bool estCandidat(tCase1 laCase, int val);
int nbCandidats(tCase1 laCase);
void initCaseCandidats(tabSudoku grille); 
int initCandidat(tabSudoku grille);

void singleton(tabSudoku grille);

void singletonCache(tabSudoku grille);
void ajouterCandidatsCase(tCase1 laCase, tabNbCandidatsParValeur tab); /** Permet d'ajouter les candidats d'une case au tableau*/

void afficherResult(char nomF[], int nbCandidatsInitial, int nbCandidatsSupr, int nbCaseVide, int nbCasesRemplies);

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
        singleton(grille);
        singletonCache(grille);

    } while (estEffectif < nbCandidatsRetires);

    backtracking(grille, 0);
    
    clock_t end = clock();
    printf("Grille finale\n");
    afficherGrille(grille);

    double tmpsCPU = (end - begin)*1.0 / CLOCKS_PER_SEC;
    printf("Résolution de la grille en %.6f sec\n",tmpsCPU);
    return EXIT_SUCCESS;
}

void chargerGrille(tabSudoku g, char nomFichier[30], int *nbCaseVide)
{

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
            fread(&(g[i][j].c_valeur), sizeof(int), 1, f);
            if (g[i][j].c_valeur == 0)
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
            if (grille[i][j].c_valeur != 0 && estCandidat(grille[indiceLigne][indiceColonne], grille[i][j].c_valeur))
            {
                retirerCandidat(&grille[indiceLigne][indiceColonne], grille[i][j].c_valeur);
            }
        }
    }

    /** vérification des lignes */

    for (int i = 0; i < TAILLE; i++)
    {
        if(grille[i][indiceColonne].c_valeur != 0 && estCandidat(grille[indiceLigne][indiceColonne], grille[i][indiceColonne].c_valeur))
        {
            retirerCandidat(&grille[indiceLigne][indiceColonne], grille[i][indiceColonne].c_valeur);
        }
    }
    

    /** vérification des colonnes */

    for (int i = 0; i < TAILLE; i++)
    {
        if(grille[indiceLigne][i].c_valeur != 0 && estCandidat(grille[indiceLigne][indiceColonne], grille[indiceLigne][i].c_valeur))
        {
            retirerCandidat(&grille[indiceLigne][indiceColonne], grille[indiceLigne][i].c_valeur);
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
            
            if(grille[i][j].c_valeur != 0 )
            {
                printf(" %2d",grille[i][j].c_valeur);
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


void ajouterCandidat(tCase1 *laCase, int val)
{
    (*laCase).c_nbCandidats++;
    (*laCase).c_candidats[(*laCase).c_nbCandidats] = val;
}

void retirerCandidat(tCase1 *laCase, int val)
{
    int i=0;
    bool continuer=true;
    while (continuer && i < TAILLE) {
        if ((*laCase).c_candidats[i] == val) {
            (*laCase).c_candidats[i] = 0;
            (*laCase).c_nbCandidats--;
            nbCandidatsRetires++;
            continuer = false;
        }
        i++;
    }
}

void retirerTousCandidatsCase(tCase1 *laCase)
{
    for (int i = 1; i < TAILLE+1; i++)
    {
        if (estCandidat(*laCase, i))
        {
            retirerCandidat(laCase, i);
        }
    }
    
}

bool estCandidat(tCase1 laCase, int val)
{
    int i=0;
    bool resultat = false;
    while (!resultat && i < TAILLE) {
        if (laCase.c_candidats[i] == val) {
            resultat = true;
        }
        i++;
    }
    return resultat;
}

int nbCandidats(tCase1 laCase)
{
    return laCase.c_nbCandidats;
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
            if (grille[i][j].c_valeur == 0 && estCandidat(grille[i][j], grille[indiceL][indiceCol].c_valeur))
            {
                retirerCandidat(&grille[i][j], grille[indiceL][indiceCol].c_valeur);
            }
        }
    }

    /** vérification des colonnes */

    for (int i = 0; i < TAILLE; i++)
    {
        if(grille[i][indiceCol].c_valeur == 0 && estCandidat(grille[i][indiceCol], grille[indiceL][indiceCol].c_valeur))
        {
            retirerCandidat(&grille[i][indiceCol], grille[indiceL][indiceCol].c_valeur);
        }
    }
    

    /** vérification des lignes */

    for (int i = 0; i < TAILLE; i++)
    {
        if(grille[indiceL][i].c_valeur == 0 && estCandidat(grille[indiceL][i], grille[indiceL][indiceCol].c_valeur))
        {
            retirerCandidat(&grille[indiceL][i], grille[indiceL][indiceCol].c_valeur);
        }
    }
}

void initCaseCandidats(tabSudoku grille)
{
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            if (grille[i][j].c_valeur == 0)
            {    
                for (int k = 0; k < TAILLE; k++)
                {
                    grille[i][j].c_candidats[k] = k+1;
                }
                grille[i][j].c_nbCandidats = TAILLE;
            }
        }
    }
}

int initCandidat(tabSudoku grille)
{
    int nbCandidatsInitial = 0;

    initCaseCandidats(grille);
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            if (grille[i][j].c_valeur == 0)
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
            if (grille[i][j].c_valeur == 0)
            {
                if (grille[i][j].c_nbCandidats == 1)
                {
                    nbCasesRemplies++;
                    int cdt = 1;
                    bool trouver = false;
                    while (cdt < TAILLE+1 && !trouver)
                    {
                        if (estCandidat(grille[i][j], cdt))
                        {
                            grille[i][j].c_valeur = cdt;
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
                if (grille[i][j].c_valeur == 0)
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
                        if(grille[j][k].c_valeur == 0 && estCandidat(grille[j][k], i))
                        {
                            grille[j][k].c_valeur = i;
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
            if(grille[i][k].c_valeur == 0)
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
                    if(grille[j][k].c_valeur == 0 && estCandidat(grille[j][k], i))
                    {
                        grille[j][k].c_valeur = i;
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
            if(grille[k][i].c_valeur == 0)
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
                    if(grille[k][j].c_valeur == 0 && estCandidat(grille[k][j], i))
                    {
                        grille[k][j].c_valeur = i;
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

void ajouterCandidatsCase(tCase1 laCase, tabNbCandidatsParValeur tab)
{
    /** Permet de compter les candidats d'une case et de les ajouter à "tab"*/
    int cdt = 1;
    int nbCandidatTrouve = 0;
    while (cdt < TAILLE+1 && nbCandidatTrouve < laCase.c_nbCandidats)
    {
        if (estCandidat(laCase, cdt))
        {
            tab[cdt] = tab[cdt] + 1;
            nbCandidatTrouve ++;
        }
        cdt++;
        
    }
}

void afficherResult(char nomF[], int nbCandidatsInitial, int nbCandidatsRetires, int nbCaseVide, int nbCasesRemplies)
{
    /** Afficher le résultat du programme*/
    printf("*********  RESULTATS POUR %s  **************** \n\n", nomF);
    printf("Nombre de cases remplies = %d / %d      ", nbCasesRemplies, nbCaseVide);
    printf("Taux de remplissage = %.3f%%", (nbCasesRemplies * 100.0) / (float) nbCaseVide);
    printf("\n");
    printf("Nombre de candidats elimines = %d      ", nbCandidatsRetires);
    printf("Pourcentage d'élimination = %.3f%%", (nbCandidatsRetires * 100.0) / (float) nbCandidatsInitial);
    printf("\n");
}

bool backtracking(tabSudoku grille, int numeroCase) {
    int ligne, colonne;
    bool resultat = false;

    if (numeroCase == TAILLE*TAILLE) {
        resultat = true;
    } else {
        ligne = numeroCase / TAILLE;
        colonne = numeroCase % TAILLE;
        if (grille[ligne][colonne].c_valeur != 0) {
            resultat = backtracking(grille, numeroCase+1);
        } else {
            for (int valeur=1 ; valeur < TAILLE +1; valeur++) {
                if (estCandidat(grille[ligne][colonne], valeur)) {
                    grille[ligne][colonne].c_valeur = valeur;

                    if (backtracking(grille, numeroCase+1) == true) {
                        resultat = true;
                    } else {
                        grille[ligne][colonne].c_valeur = 0;
                    }
                }
            }
        }
    }
    return resultat;
}