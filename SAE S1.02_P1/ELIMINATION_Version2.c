#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 3
#define TAILLE (N*N)

typedef struct {
    int valeur;
    bool candidats[TAILLE + 1];

    int nbCandidat;
} tCase2;

typedef tCase2 tabSudoku[TAILLE][TAILLE];


int chargerGrille(tabSudoku g);
void afficherGrille(tabSudoku grille);

void chercherCandidat(tabSudoku grille, int indiceLigne, int indiceColonne);
void ajouterCandidat(tCase2 *laCase, int val);
void retirerCandidat(tCase2 *laCase, int val);
void retirerCandidatsSuiteAjout(tabSudoku grille, int indiceL, int indiceCol);
bool estCandidat(tCase2 laCase, int val);
int nbCandidats(tCase2 laCase, int val);
void initCaseCandidatsTrue(tabSudoku grille);
void initCandidat(tabSudoku grille);
void singleton(tabSudoku grille);



int main()
{
    tabSudoku grille;
    
    int nbCseVide = chargerGrille(grille);
    printf("%d\n", nbCseVide);
    afficherGrille(grille);
    initCandidat(grille);
    singleton(grille);
    

    afficherGrille(grille);

    return EXIT_SUCCESS;
}

int chargerGrille(tabSudoku g)
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

    char nomFichier[30];
    FILE * f;
    int cmptCaseVide;

    printf("Nom du fichier ? ");
    scanf("%s", nomFichier);
    f = fopen(nomFichier, "rb");

    /* Redemande la saisie jusqu'à ce que le nom du fichier soit valide */
    while (f==NULL)
    {
        printf("\nERREUR sur le fichier %s\n\n", nomFichier);
        printf("Nom du fichier ? ");
        scanf("%s", nomFichier);
        f = fopen(nomFichier, "rb");
    }
    
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            fread(&(g[i][j].valeur), sizeof(int), 1, f);
            if (g[i][j].valeur == 0)
            {
                cmptCaseVide ++;
            }
            
        }
    }
    fclose(f);
    return cmptCaseVide;
}

void chercherCandidat(tabSudoku grille, int indiceLigne, int indiceColonne)
{
    int indiceInitLigne;  /** indice de la ligne en haut à gauche du carré ou se trouve la case */
    int indiceInitCol;  /** indice de la colonne en haut à gauche du carré ou se trouve la case */

    /** Initialisation */

    indiceInitLigne = N * (int)(indiceLigne/N);
    indiceInitCol = N * (int)(indiceColonne/N);

    /** vérification des sous-tableau */
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

    /** affiche les indices des colonnes */
    printf("    ");
    for (int i = 1; i <= TAILLE; i++)
    {
        printf("%3d",i);
        if (i% (int)N == 0)
        {
            printf(" ");
        }
        
    }
    printf("\n");
    
    for(int i = 0; i < TAILLE ; i++)
    {
        if (i%indiceSousTab == 0)
        {
            /** affiche les séparations horizontales*/
            printf("    +");
            for (int k = 1; k <= TAILLE; k++)
            {
                printf("---");
                if (k% (int)N == 0)
                {
                    printf("+");
                }
                
            }
            printf("\n");
        }

        printf("%3d ",i+1); /** affiche les indices des lignes */


        for(int j = 0; j < TAILLE; j++)
        {
            if (j%indiceSousTab == 0)
            {
                printf("|"); /** affiche les séparations verticales */
            }
            
            if(grille[i][j].valeur != 0 )
            {
                printf(" %d ",grille[i][j].valeur);
            }
            else
            {
                printf(" . "); /** Remplace les 0 par des . (pour l'affichage uniquement) */
            }
            }
        
        printf("|\n");
    }

    /** affiche la séparation verticale de fin*/
    printf("    +");
    for (int k = 1; k <= TAILLE; k++)
    {
        printf("---");
        if (k% (int)N == 0)
        {
            printf("+");
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
    (*laCase).candidats[val] = false;
    (*laCase).nbCandidat --;
}

bool estCandidat(tCase2 laCase, int val)
{
    return laCase.candidats[val];
}

int nbCandidats(tCase2 laCase, int val)
{
    return laCase.nbCandidat;
}

void retirerCandidatsSuiteAjout(tabSudoku grille, int indiceL, int indiceCol)
{
    int indiceInitLigne;  /** indice de la ligne en haut à gauche du carré ou se trouve la case */
    int indiceInitCol;  /** indice de la colonne en haut à gauche du carré ou se trouve la case */

    /** Initialisation */

    indiceInitLigne = N * (int)(indiceL/N);
    indiceInitCol = N * (int)(indiceCol/N);

    /** vérification des sous-tableau */
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

    /** vérification des lignes */

    for (int i = 0; i < TAILLE; i++)
    {
        if(grille[i][indiceCol].valeur == 0 && estCandidat(grille[i][indiceCol], grille[indiceL][indiceCol].valeur))
        {
            retirerCandidat(&grille[i][indiceCol], grille[indiceL][indiceCol].valeur);
        }
    }
    

    /** vérification des colonnes */

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
                grille[i][j].nbCandidat = 9;
            }
        }
    }
}

void initCandidat(tabSudoku grille)
{
    initCaseCandidatsTrue(grille);
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            if (grille[i][j].valeur == 0)
            {
                chercherCandidat(grille, i,j);
            }
        }
    }
}

void singleton(tabSudoku grille)
{
    bool estEffectif = true;

    while (estEffectif)
    {
        estEffectif = false;
        for (int i = 0; i < TAILLE; i++)
        {
            for (int j = 0; j < TAILLE; j++)
            {
                if (grille[i][j].valeur == 0)
                {
                    if (grille[i][j].nbCandidat == 1)
                    {
                        estEffectif = true;
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
}
