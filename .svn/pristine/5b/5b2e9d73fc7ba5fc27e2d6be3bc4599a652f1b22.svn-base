#ifndef GAME_H
#define GAME_H
#include "app.h"
#include <gtk/gtk.h>  /* gboolean */
#include "ia.h"       /* GameState, Move */

/**
 * @file game.h
 * @brief Module central implémentant la logique et les règles du jeu Krojanty
 * @author Valentin PREVOT (valentin.prevot@uha.fr)
 * @date 2025-09-18
 * @version 1.0
 *
 * Ce module gère toute la mécanique de jeu du Krojanty :
 * - Initialisation du plateau et positionnement des pièces
 * - Validation des mouvements selon les règles officielles
 * - Gestion des captures Linca (orthogonales) et Seltou (diagonales)
 * - Calcul des scores et détection des conditions de victoire
 * - Interface avec l'IA et le système de réseau
 *
 * C'est le cœur du jeu qui garantit le respect des règles et
 * la cohérence de l'état du jeu à tout moment.
 *
 * \section game_rules Règles du Jeu
 *
 * \subsection setup Configuration
 * - Plateau de 9x9 cases
 * - 12 pièces par joueur au départ
 * - Position initiale symétrique par rapport au centre
 *
 * \subsection moves Mouvements
 * - Déplacement orthogonal ou diagonal
 * - Distance illimitée
 * - Pas de saut par-dessus d'autres pièces
 *
 * \subsection captures Captures
 * Deux types de captures sont possibles :
 *
 * \subsubsection linca Capture Linca (orthogonale)
 * - Se fait en ligne droite (horizontale/verticale)
 * - Nécessite une pièce alliée à l'autre bout
 * - Capture toutes les pièces ennemies entre les deux
 * - Code de vérification : game_check_linca_capture()
 *
 * \subsubsection seltou Capture Seltou (diagonale)
 * - Se fait en diagonale
 * - Nécessite une pièce alliée à l'autre bout
 * - Capture toutes les pièces ennemies entre les deux
 * - Code de vérification : game_check_seltou_capture()
 *
 * \subsection victory Conditions de Victoire
 * La partie se termine quand :
 * - Un joueur capture toutes les pièces adverses
 * - Un joueur n'a plus de mouvements légaux
 * - Code de vérification : game_check_victory()
 *
 * \section game_api API du Module
 * Ce module expose les fonctions pour :
 * - Initialiser la position
 * - Effectuer des mouvements
 * - Calculer les surbrillances (highlight)
 * - Interagir avec l'IA (callback `trigger_ia_move`)
 * - Valider les règles
 * - Gérer les captures
 */

/**
 * @struct Piece
 * @brief Représente une pièce sur le plateau
 *
 * Cette structure encode l'état complet d'une pièce du jeu,
 * incluant sa position, son type et son statut.
 *
 * @var Piece::row
 * Position verticale sur le plateau (0-8)
 *
 * @var Piece::col  
 * Position horizontale sur le plateau (0-8)
 *
 * @var Piece::is_king
 * true si c'est un roi, false pour un soldat
 *
 * @var Piece::is_blue
 * true pour une pièce bleue, false pour une rouge
 *
 * @var Piece::is_captured
 * true si la pièce a été capturée
 */

/**
 * @brief Indicateur de fin de partie
 * 
 * Cette variable globale indique si la partie est terminée.
 * - 0 : Partie en cours
 * - 1 : Partie terminée
 * 
 * Une partie peut se terminer pour plusieurs raisons :
 * - Un joueur a perdu toutes ses pièces
 * - Un joueur n'a plus de mouvements légaux
 * - Le nombre maximum de tours est atteint
 * - Un joueur abandonne
 */
extern int game_over;

/**
 * @name Configuration de l'IA
 * @{
 * 
 * Ensemble des variables globales qui contrôlent le
 * comportement de l'Intelligence Artificielle.
 */

/**
 * @brief État d'activation de l'IA
 * 
 * Contrôle si l'IA est active dans la partie.
 * - 0 : IA désactivée (mode joueur contre joueur)
 * - 1 : IA activée (mode joueur contre IA)
 */
extern int ia_active;

/**
 * @brief Couleur jouée par l'IA
 * 
 * Définit quelle couleur l'IA va jouer.
 * - 'B' : L'IA joue les Bleus
 * - 'R' : L'IA joue les Rouges
 * 
 * @note Cette variable est ignorée si ia_both_active est 1
 */
extern char ia_color;

/**
 * @brief Mode IA contre IA
 * 
 * Permet d'activer le mode où deux IA jouent l'une contre l'autre.
 * - 0 : Mode normal (un seul joueur IA)
 * - 1 : Mode IA vs IA activé
 * 
 * @note Si activé, ignore la valeur de ia_color
 */
extern int ia_both_active;

/**
 * @brief Profondeur de recherche de l'algorithme Minimax
 * 
 * Contrôle la profondeur de recherche de l'IA.
 * - 1-2 : IA facile (réponse rapide)
 * - 3-4 : IA moyenne (bon équilibre)
 * - 5+ : IA difficile (plus lente)
 * 
 * La complexité augmente exponentiellement avec la profondeur.
 * Une profondeur de N examine environ 20^N positions.
 * 
 * @note Une profondeur > 6 peut causer des ralentissements
 */
extern int ia_search_depth;

/** @} */

/* Initialisation et positionnement */
/** @brief Charge la configuration par défaut des pièces sur le plateau. */
void game_setup_default(void);

/**
 * @brief Définit la position courante à partir d'un tableau de pièces.
 * @param arr tableau de `Piece` contenant la position
 * @param count nombre d'éléments valides dans `arr`
 */
void game_set_position(const Piece *arr, int count);

/* Utilitaires d'état */
/** Initialise `cell_control` selon l'état courant. */
void initialize_controlled_cells(void);

/** Recalcule et met à jour les scores (`score_blue`, `score_red`). */
void update_scores(void);

/** Marque une case comme contrôlée par une couleur.
 * @param row ligne
 * @param col colonne
 * @param color 'B' ou 'R'
 */
void mark_cell_controlled(int row, int col, char color);

/* Mouvement et règles */
/** Retourne 1 si la case est libre (aucune pièce), 0 sinon. */
int  is_cell_free(int row, int col);

/**
 * @brief Vérifie si un mouvement est valide pour l'index de pièce donné.
 * @param index indice de la pièce dans `pieces[]`
 * @param dest_row destination ligne
 * @param dest_col destination colonne
 * @return 1 si le mouvement est autorisé, 0 sinon
 */
int  can_move(int index, int dest_row, int dest_col);

/**
 * @brief Déplace la pièce indiquée et applique captures/effets.
 * @return 0 si le jeu continue, >0 si la partie est terminée (victoire, etc.).
 */
int  move_piece(int piece_idx, int to_row, int to_col);

/* Sélection / surbrillance */
/** Efface toutes les surbrillances de mouvements. */
void clear_highlight(void);

/** Calcule les coups valides pour la pièce sélectionnée et remplit `highlight_moves`. */
void compute_highlight_for_selected(void);

/* Recherche */
/** Retourne l'index de la pièce trouvée à la case (row,col) ou -1 si vide. */
int  find_piece_at(int row, int col);

/* Snapshot / Reset (logique pure, sans GTK) */
/** Sauvegarde l'état initial pour un éventuel replay/reset. */
void snapshot_initial_state(void);

/** Restaure l'état initial précédemment sauvegardé. */
void restore_initial_state(void);

/* Fonctions IA */
/** Callback GTK pour déclencher un mouvement de l'IA (utilisé par un timer). */
gboolean trigger_ia_move(gpointer user_data);

#endif // GAME_H
