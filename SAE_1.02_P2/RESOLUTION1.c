#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define TAILLE 9 /** @def Taille de notre tableau de jeu */
#define N sqrt(TAILLE) /** @def Taille de chaque sous tableau*/

void chargerGrille(tabSudoku g);
void afficherGrille(tabSudoku grille);

int main()
{
    return EXIT_SUCCESS;
}


void chargerGrille(tabSudoku g)
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
    fread(g, sizeof(int), TAILLE*TAILLE, f);
    fclose(f);
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
            
            if(grille[i][j] != 0 )
            {
                printf(" %d ",grille[i][j]);
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

bool possible(tabSudoku grille, int indiceLigne, int indiceColonne, int valeur)
{
    /**
     * @fn bool possible(tabSudoku grille, int indiceLigne, int indiceColonne, int valeur)
     * 
     * @brief Vérifie si une valeur peut être placée dans une certaine case de la grille.
     * 
     * @param grille La grille Sudoku.
     * @param indiceLigne Indice de ligne de la case à vérifier.
     * @param indiceColonne Indice de colonne de la case à vérifier.
     * @param valeur La valeur à placer.
     * 
     * @return true si la valeur peut être placée, false sinon.
     * 
     * Fonction qui vérifie que la valeur peut être placée dans la case
     * Vérifie si la valeur est déjà présente dans le carrés de N*N 
     * ou elle se trouve, puis dans la ligne, et enfin dans la colonne.
     * Renvoit true si la valeur n'est dans aucun des 3 (et donc qu'elle 
     * peut être placé) et false sinon.
    */

    bool estValide;
    int cmptLigne;
    int cmptCol;

    int indiceInitLigne;  /** indice de la ligne en haut à gauche du carré ou se trouve la case */
    int indiceInitCol;  /** indice de la colonne en haut à gauche du carré ou se trouve la case */

    /** Initialisation */
    estValide = true;

    indiceInitLigne = N * (int)(indiceLigne/N);
    indiceInitCol = N * (int)(indiceColonne/N);
    cmptLigne = indiceInitLigne;
    cmptCol = indiceInitCol;

    /** vérification des sous-tableau */
    while (estValide && cmptLigne < indiceInitLigne + N)
    {
        while (estValide && cmptCol < indiceInitCol + N)
        {
            if (grille[cmptLigne][cmptCol] == valeur)
            {
                estValide = false;
                printf("\nIMPOSSIBLE : cette valeur est deja presente dans le carre\n\n");
            }
            cmptCol ++;
        }
        cmptLigne ++;
        cmptCol = indiceInitCol;
    }

    /** Remise à 0 pour vérifier les lignes puis les colonnes */
    cmptLigne = 0;
    cmptCol = 0;

    /** vérification des lignes */

    while (estValide && cmptCol < TAILLE){

        if (grille[indiceLigne][cmptCol] == valeur)
        {
            estValide = false;
            printf("\nIMPOSSIBLE : cette valeur est deja presente dans la ligne\n\n");
        }
        cmptCol++;
    }

    /** vérification des colonnes */

    while(estValide && cmptLigne < TAILLE){

        if (grille[cmptLigne][indiceColonne] == valeur)
        {
            estValide = false;
            printf("\nIMPOSSIBLE : cette valeur est deja presente dans la colonne\n\n");
        }
        cmptLigne++;
    }

    return estValide;
}        