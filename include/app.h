#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdbool.h>

/**
 * @file app.h
 * @brief API principale du jeu de stratégie Krojanty
 * @author Groupe 2
 * @version 1.0
 * @date 2025-09-18
 *
 * @brief Représente une pièce sur le plateau de jeu.
 * 
 * Cette structure contient toutes les informations nécessaires pour définir
 * une pièce du jeu : sa position, sa couleur et son type. C'est la structure
 * fondamentale du jeu car elle définit l'état du plateau.
 *
 * @var Piece::row
 * Position verticale sur le plateau (0-6, du haut vers le bas)
 * 
 * @var Piece::col
 * Position horizontale sur le plateau (0-6, de gauche à droite)
 * 
 * @var Piece::color
 * Couleur de la pièce :
 * - 'B' : Bleu (Premier joueur)
 * - 'R' : Rouge (Second joueur)
 * 
 * @var Piece::type
 * Type de la pièce :
 * - 'P' : Pion (pièce normale)
 * - 'K' : Roi (pièce spéciale avec règles particulières)
 * 
 * @note Les pièces capturées sont marquées avec row = col = -1
 * @note Un maximum de 20 pièces est supporté (12 par joueur)
 */
/**
 * @brief Structure représentant une pièce du jeu
 * 
 * Cette structure contient toutes les informations nécessaires pour 
 * définir et gérer une pièce sur le plateau : sa position, son type 
 * et son appartenance. Elle est utilisée pour les rois et les soldats.
 */
typedef struct {
    int row;    /**< Position verticale (0-8, -1 si capturée) */
    int col;    /**< Position horizontale (0-8, -1 si capturée) */
    char color; /**< Couleur de la pièce : 'B' (bleu) ou 'R' (rouge) */
    char type;  /**< Type de pièce : 'K' (roi) ou 'P' (soldat) */
} Piece;

/**
 * @name État Global du Jeu
 * @{
 */

/** @brief Tableau de toutes les pièces en jeu */
extern Piece pieces[20];

/** @brief Nombre total de pièces actuellement en jeu */
extern int piece_count;

/**
 * @brief Tableau de contrôle des cases du plateau
 * 
 * Stocke qui contrôle chaque case du plateau 9x9 :
 * - 0 : Case neutre
 * - 1 : Contrôlée par le joueur Bleu
 * - 2 : Contrôlée par le joueur Rouge
 */
extern int cell_control[9][9];

/**
 * @brief Tableau des mouvements possibles
 * 
 * Utilisé pour la surbrillance des coups valides :
 * - 0 : Mouvement impossible
 * - 1 : Mouvement possible
 */
extern int highlight_moves[9][9];

/**
 * @brief Index de la pièce sélectionnée
 * - -1 : Aucune pièce sélectionnée
 * - 0-19 : Index dans le tableau pieces[]
 */
extern int selected_piece;

/**
 * @brief Joueur dont c'est le tour
 * - 'B' : Tour du joueur Bleu
 * - 'R' : Tour du joueur Rouge
 */
extern char current_turn;

/**
 * @brief Numéro du tour actuel (1-64)
 * La partie se termine automatiquement au 64ème tour
 */
extern int turn_number;

/**
 * @brief État de fin de partie
 * - 0 : Partie en cours
 * - 1 : Partie terminée
 */
extern int game_over;

/**
 * @brief Score des joueurs
 * Calculé selon les règles de fin de partie :
 * - +1 par case contrôlée
 * - +1 par soldat en vie
 */
extern int score_blue;  /**< Score du joueur Bleu */
extern int score_red;   /**< Score du joueur Rouge */

/**
 * @brief Nombre de pièces restantes par joueur
 * Utilisé pour la condition de victoire par extermination
 */
extern int pieces_blue; /**< Pièces restantes du joueur Bleu */
extern int pieces_red;  /**< Pièces restantes du joueur Rouge */

/**
 * @brief Limite maximale de tours
 * Par défaut 64 tours (32 par joueur)
 */
extern short max_turn;

/** @} */

/**
 * @name Ressources Graphiques
 * @{
 */

/**
 * @brief Logos des joueurs
 * Images chargées depuis les fichiers logo-blue.png et logo-red.png
 */
extern GdkPixbuf *logo_blue; /**< Logo du joueur Bleu */
extern GdkPixbuf *logo_red;  /**< Logo du joueur Rouge */

/** @} */

#endif // APP_H
