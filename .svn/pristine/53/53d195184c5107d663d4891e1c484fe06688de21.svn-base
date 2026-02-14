#ifndef CAPTURES_H
#define CAPTURES_H
#include "app.h"

/**
 * @file captures.h
 * @brief Règles de capture et vérification des conditions de victoire.
 *
 * Fournit des fonctions pour détecter et appliquer les captures Linca
 * (encadrement) et Seltou (arrière vide), ainsi que la règle
 * d'auto-défaite spécifique au jeu.
 *
 * Ces fonctions mettent à jour l'état global (tableau `pieces[]`,
 * `cell_control`, scores) et appellent le module `status` pour afficher
 * des messages de victoire si nécessaire.
 */

/**
 * @brief Vérifie et applique les captures par encadrement (Linca).
 *
 * Cette fonction regarde les pièces adjacentes à la pièce déplacée et
 * applique la capture si une pièce ennemie est encadrée horizontalement
 * ou verticalement.
 *
 * @param moved_index indice de la pièce qui vient d'être déplacée.
 */
void check_linca_capture(int moved_index);

/**
 * @brief Vérifie et applique les captures par arrière vide (Seltou).
 *
 * Détecte et supprime les pièces ennemies devenues vulnérables suite au
 * déplacement de la pièce indiquée.
 *
 * @param moved_index indice de la pièce déplacée.
 * @param old_row ligne d'origine du mouvement.
 * @param old_col colonne d'origine du mouvement.
 */
void check_seltou_capture(int moved_index, int old_row, int old_col);

/**
 * @brief Applique la règle d'auto-défaite si un joueur ne garde que
 *        certains types de pièces (par ex. seulement pion+roi selon règles).
 */
void check_auto_defeat(void);

#endif // CAPTURES_H
