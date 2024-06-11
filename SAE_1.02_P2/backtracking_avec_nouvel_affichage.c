#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define N 4
#define TAILLE (N*N)

typedef int tGrille[TAILLE][TAILLE];

void chargerGrille(tGrille g);
void afficherGrille(tGrille grille1);
bool backtracking(tGrille grille, int numeroCase);
bool possible(tGrille grille1, int numLigne, int numColonne, int valeur);

int main() {
    //déclaration des variables
	tGrille grille1;
	int numLigne, numColonne, valeur;
	
   	// saisie des données
	chargerGrille(grille1);
	afficherGrille(grille1); // Affichage de la grille

    clock_t begin = clock();

    printf("\nDébut de résolution !\n");
    backtracking(grille1, 0);
    
    printf("\n\n");
    afficherGrille(grille1);
    printf("\n\nFin de résolution: \n\n");

    clock_t end = clock();
    double tmpsCPU = (end - begin)*1.0 / CLOCKS_PER_SEC;
    printf( "Temps CPU = %.3f secondes\n",tmpsCPU);

    

	// programme principale

   	return EXIT_SUCCESS;

}

/*****************************************************
		* LISTE DES PROCÉDURES/FONCTIONS *
*****************************************************/


/**
 * \fn void chargerGrille(tGrille g)
 * 
 * \brief Procédure initialisant le début de la partie.
 * 
 * \param g : le tableau représentant le plateau de Sudoku qui est à remplir
 * 
 * Consiste à remplir une partie du Sudoku en chargeant un fichier.sud
 * 
*/
void chargerGrille(tGrille g){
	char nomFichier[30];
	FILE * f;
	printf("Nom du fichier ? ");
	scanf("%s", nomFichier);
	f = fopen(nomFichier, "rb");
	while (f==NULL) { // On traite le cas d'erreur
		printf("\n Un nom de fichier valide est attendu\n");
		printf("Nom du fichier ? ");
		scanf("%s", nomFichier);
		f = fopen(nomFichier, "rb");	
	}
	fread(g, sizeof(int), TAILLE*TAILLE, f);
	fclose(f);
}

void afficherGrille(tGrille grille)
{

    /**
     * @fn void afficherGrille(tGrille grille)
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
            
            if(grille[i][j] != 0 )
            {
                printf(" %2d",grille[i][j]);
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


bool possible(tGrille grille, int indiceLigne, int indiceColonne, int valeur)
{
    /**
     * @fn bool possible(tGrille grille, int indiceLigne, int indiceColonne, int valeur)
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
        }
        cmptCol++;
    }

    /** vérification des colonnes */

    while(estValide && cmptLigne < TAILLE){

        if (grille[cmptLigne][indiceColonne] == valeur)
        {
            estValide = false;
        }
        cmptLigne++;
    }

    return estValide;
}        

bool backtracking(tGrille grille, int numeroCase) {
    int ligne, colonne;
    bool resultat = false;

    if (numeroCase == TAILLE*TAILLE) {
        resultat = true;
    } else {
        ligne = numeroCase / TAILLE;
        colonne = numeroCase % TAILLE;
        if (grille[ligne][colonne] != 0) {
            resultat = backtracking(grille, numeroCase+1);
        } else {
            for (int valeur=1 ; valeur < TAILLE +1; valeur++) {
                if (possible(grille, ligne, colonne, valeur)) {
                    grille[ligne][colonne] = valeur;

                    if (backtracking(grille, numeroCase+1) == true) {
                        resultat = true;
                    } else {
                        grille[ligne][colonne] = 0;
                    }
                }
            }
        }
    }
    return resultat;
}