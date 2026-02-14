#ifndef IA_H
#define IA_H

#include "app.h"  /* définit Piece, pieces[], piece_count */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file ia.h
 * @brief Module d'Intelligence Artificielle pour le jeu Krojanty
 * @author Valentin LERAY (valentin.leray@uha.fr)
 * @date 2025-09-18
 * @version 1.0
 *
 * Ce module implémente un adversaire artificiel sophistiqué pour le jeu Krojanty,
 * utilisant l'algorithme Minimax avec élagage Alpha-Beta. L'IA évalue les positions
 * selon plusieurs critères :
 * - Matériel (nombre et type de pièces)
 * - Contrôle territorial
 * - Mobilité des pièces
 * - Sécurité du roi
 * 
 * L'IA est capable de jouer à différents niveaux de difficulté en ajustant
 * la profondeur de recherche et utilise des optimisations comme la table
 * de transposition pour améliorer ses performances.
 *
 * \section ai_overview Vue d'ensemble
 * L'IA du Krojanty utilise l'algorithme Minimax avec élagage Alpha-Beta
 * et diverses optimisations pour choisir les meilleurs coups possibles.
 *
 * \section ai_components Composants Principaux
 *
 * \subsection minimax_algo Algorithme Minimax
 * - Implémentation récursive avec élagage Alpha-Beta
 * - Profondeur variable selon le niveau de difficulté
 * - Table de transposition pour mémoriser les positions
 *
 * \subsection eval_func Fonction d'Évaluation
 * La position est évaluée selon plusieurs critères :
 * - Nombre de pièces (200 points par pièce)
 * - Contrôle du centre (50 points par case)
 * - Mobilité des pièces (30 points par mouvement)
 * - Menaces de capture (100 points par menace)
 * - Structure défensive (40 points par pièce protégée)
 *
 * \subsection move_order Ordonnancement des Coups
 * Les coups sont examinés dans cet ordre :
 * 1. Captures (priorité maximale)
 * 2. Mouvements vers le centre
 * 3. Mouvements défensifs
 * 4. Autres mouvements
 *
 * \section ai_optimize Optimisations
 * - Table de transposition (cache des positions)
 * - History heuristic (mémorisation des bons coups)
 * - Killer moves (coups qui causent des coupures)
 * - Fenêtre Alpha-Beta adaptative
 *
 * Fournit des structures légères (GameState, Move) et des fonctions
 * pour calculer un coup via minimax avec alpha-bêta.
 */

/**
 * @brief Structure représentant un mouvement de pièce dans le jeu Krojanty
 * 
 * Cette structure encode un mouvement complet d'une pièce, comprenant
 * son identifiant unique et ses coordonnées de départ et d'arrivée.
 * Elle est utilisée pour :
 * - Valider les mouvements selon les règles
 * - Transmettre les coups en mode réseau
 * - Analyser les positions par l'IA
 * - Enregistrer l'historique des coups
 *
 * La structure garantit qu'un mouvement est complètement spécifié
 * avec toutes les informations nécessaires pour le valider et
 * l'appliquer de manière cohérente.
 *
 * Exemple d'utilisation :
 * @code
 * Move best_move;
 * trouverMeilleurCoupIA(game_state, &best_move, 3);
 * if (is_move_valid(&best_move)) {
 *     apply_move(&best_move);
 * }
 * @endcode
 *
 * @var Move::piece_index
 * Identifiant unique de la pièce :
 * - 0-9 : Pièces bleues (0=roi, 1-9=soldats)
 * - 10-19 : Pièces rouges (10=roi, 11-19=soldats)
 * 
 * @var Move::from_row
 * Ligne de départ :
 * - 0-8 : Position valide sur le plateau
 * - -1 : Pièce capturée ou invalide
 * 
 * @var Move::from_col
 * Colonne de départ :
 * - 0-8 : Position valide sur le plateau
 * - -1 : Pièce capturée ou invalide
 * 
 * @var Move::to_row
 * Ligne d'arrivée :
 * - 0-8 : Position valide sur le plateau
 * Doit respecter les règles de déplacement (orthogonal)
 * 
 * @var Move::to_col
 * Colonne d'arrivée :
 * - 0-8 : Position valide sur le plateau
 * Doit respecter les règles de déplacement (orthogonal)
 * 
 * @see GameState
 * @see trouverMeilleurCoupIA()
 * @note Un mouvement peut déclencher des captures Linca ou Seultou
 * @warning Vérifiez la validité du mouvement avant de l'appliquer
 */
/**
 * @struct Move
 * @brief Structure représentant un mouvement de pièce dans le jeu Krojanty
 *
 * Cette structure est l'unité fondamentale de mouvement du jeu. Elle encode
 * complètement un déplacement de pièce en stockant :
 * - L'identifiant unique de la pièce déplacée
 * - Sa position de départ sur le plateau
 * - Sa position d'arrivée sur le plateau
 *
 * Elle est utilisée par plusieurs composants du jeu :
 * - Le module de règles pour valider les mouvements
 * - L'IA pour analyser et évaluer les coups possibles
 * - Le module réseau pour synchroniser les mouvements
 * - L'interface graphique pour afficher les coups
 */
typedef struct {
    int piece_index;        /**< Index dans le tableau pieces[] */
    int from_row, from_col; /**< Position de départ */
    int to_row,   to_col;   /**< Position d'arrivée */
} Move;

/**
 * @brief Structure représentant l'état complet d'une partie de Krojanty
 * 
 * Cette structure fondamentale encapsule l'intégralité de l'état du jeu
 * à un instant donné. Elle inclut toutes les informations nécessaires pour :
 * - Représenter la position des pièces sur le plateau
 * - Gérer le contrôle territorial des cases
 * - Suivre l'état du tour et le compteur de coups
 * - Permettre à l'IA de simuler et évaluer des positions
 * - Sauvegarder/restaurer l'état du jeu
 *
 * C'est la structure de données centrale du jeu, utilisée par tous
 * les modules (interface graphique, réseau, IA) pour maintenir une
 * vue cohérente de la partie en cours.
 *
 * Exemple d'utilisation :
 * @code
 * GameState state = createGameStateFromCurrent();
 * if (isGameOver(&state)) {
 *     return evaluation(&state, 'B');
 * }
 * // Continuer l'analyse...
 * @endcode
 *
 * @var GameState::pieces
 * Tableau de toutes les pièces :
 * - [0-9] : Pièces bleues (0=roi, 1-9=soldats)
 * - [10-19] : Pièces rouges (10=roi, 11-19=soldats)
 * Chaque pièce inclut sa position et son type
 *
 * @var GameState::piece_count
 * Nombre de pièces encore en jeu :
 * - Initial : 20 (10 par camp)
 * - Mise à jour après chaque capture
 * - Utilisé pour la condition de victoire par extermination
 *
 * @var GameState::cell_control
 * Tableau de contrôle territorial [9][9] :
 * - 0 : Case neutre
 * - 1 : Case contrôlée par Bleu
 * - 2 : Case contrôlée par Rouge
 * Utilisé pour le calcul du score final si limite de tours atteinte
 *
 * @var GameState::current_player
 * Indique le joueur actif :
 * - 'B' : Tour du joueur Bleu
 * - 'R' : Tour du joueur Rouge
 *
 * @var GameState::turn_number
 * Compteur de tours :
 * - Maximum 64 tours (32 par joueur)
 * - Utilisé pour la détection de boucles et fin de partie
 *
 * @see Move
 * @see evaluation()
 * @see isGameOver()
 * @note Les cités ne sont pas comptabilisées dans le contrôle territorial
 * @warning Utilisez createGameStateFromCurrent() pour une copie valide
 */
/**
 * @struct GameState
 * @brief Structure principale représentant l'état complet d'une partie de Krojanty
 *
 * Cette structure est le cœur du jeu Krojanty, contenant toutes les informations
 * nécessaires pour représenter et manipuler une partie en cours. Elle inclut :
 * - La position et l'état de toutes les pièces
 * - Le contrôle territorial de chaque case
 * - Le joueur actif et le numéro du tour
 *
 * Elle est utilisée pour :
 * - Gérer l'état du jeu en temps réel
 * - Permettre à l'IA de simuler des positions
 * - Sauvegarder/charger des parties
 * - Synchroniser l'état en mode réseau
 * - Valider les règles du jeu
 *
 * C'est la structure la plus importante du jeu car elle maintient
 * la cohérence de l'état entre tous les modules (GUI, IA, réseau).
 */
typedef struct {
    Piece pieces[128];      /**< État des pièces */
    int   piece_count;      /**< Nombre de pièces actives */
    int   cell_control[9][9];/**< Contrôle des cases */
    char  current_player;   /**< 'B' ou 'R' */
    int   turn_number;      /**< Numéro du tour */
} GameState;

/* API publique utilisée par la GUI */
/**
 * @brief Construit un `GameState` à partir de l'état global courant.
 * @return GameState copié (passage par valeur).
 */
GameState createGameStateFromCurrent(void);

/**
 * @brief Remplit `best_move` avec le meilleur coup trouvé par l'IA.
 * @param jeu état du jeu (modifié localement pendant la recherche)
 * @param best_move sortie contenant le coup choisi
 * @param profondeur profondeur de recherche (0 = heuristique immédiate)
 */
void trouverMeilleurCoupIA(GameState* jeu, Move* best_move, int profondeur);
void trouverMeilleurCoupIA_Adaptatif(GameState* jeu, Move* best_move);
void trouverMeilleurCoupIA_Rapide(GameState* jeu, Move* best_move);
void reset_move_history(void);

/* Algorithmes/supports (utiles pour tests) */
/** Minimax alpha-bêta, retourne une valeur d'évaluation. */
int minimaxIA(GameState* jeu, int profondeur, char maximizing_player, int alpha, int beta);

/** Retourne 1 si l'état `jeu` est terminal (fin de partie). */
int isGameOver(GameState* jeu);

/** Fonction d'évaluation heuristique pour un joueur donné. */
int evaluation(GameState* jeu, char evaluating_player);

#ifdef __cplusplus
}
#endif

#endif // IA_H