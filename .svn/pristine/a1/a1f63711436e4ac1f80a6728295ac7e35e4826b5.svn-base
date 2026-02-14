/**
 * \file ia.c
 * \brief IA Minimax avec élagage alpha-bêta pour un jeu de plateau 9x9.
 * \author ayoub.darkaoui@uha.fr, valentin.prevot@uha.fr, valentin.leray@uha.fr
 * \version 0.1
 * \date 2025-09-17
 *
 * \details
 * Implémentations principales :
 * - Représentation interne du plateau et des pièces
 * - Détection des mouvements valides
 * - Calcul des vulnérabilités (Linca/Seltou)
 * - Application des coups et captures
 * - Évaluation heuristique
 * - Minimax avec élagage alpha-bêta
 * - Système anti-boucle
 * - Optimisations de performance
 */

#include "ia.h"
#include <stdlib.h>
#include <string.h>

// Historique des coups pour détecter les répétitions
#define MAX_MOVE_HISTORY 8
static Move move_history[MAX_MOVE_HISTORY];
static int history_count = 0;


// --- Utilitaires internes ---

/**
 * \brief Retourne la valeur absolue d’un entier.
 * 
 * \param x Entier.
 * \return Valeur absolue de \p x.
 */
static inline int abs_i(int x) { return (x < 0) ? -x : x; }

/**
 * \fn static int findPieceAt(GameState* s, int row, int col)
 * \brief Recherche l’index d’une pièce aux coordonnées données.
 * 
 * \param s État du jeu.
 * \param row Ligne.
 * \param col Colonne.
 * \return Index de la pièce trouvée, ou -1 si vide.
 */
static int findPieceAt(GameState* s, int row, int col) {
    for (int i = 0; i < s->piece_count; ++i)
        if (s->pieces[i].row == row && s->pieces[i].col == col) return i;
    return -1;
}

/**
 * \fn static int isCellFree(GameState* s, int row, int col)
 * \brief Vérifie si une case est libre.
 * 
 * \param s État du jeu.
 * \param row Ligne.
 * \param col Colonne.
 * \return 1 si la case est libre, 0 sinon.
 */
static int isCellFree(GameState* s, int row, int col) {
    return findPieceAt(s, row, col) < 0;
}

/**
 * \fn static inline int in_bounds(int r, int c)
 * \brief Vérifie si une case est dans les limites du plateau.
 * 
 * \param r Ligne.
 * \param c Colonne.
 * \return 1 si la case est dans le plateau, 0 sinon.
 */
static inline int in_bounds(int r, int c) { return r >= 0 && r < 9 && c >= 0 && c < 9; }

/**
 * \fn static int can_specific_piece_reach_square(GameState* s, int piece_index, int tr, int tc)
 * \brief Vérifie si une pièce spécifique peut atteindre une case par un mouvement rectiligne.
 * 
 * \param s État du jeu.
 * \param piece_index Index de la pièce.
 * \param tr Ligne cible.
 * \param tc Colonne cible.
 * \return 1 si la pièce peut atteindre la case, 0 sinon.
 */
static int can_specific_piece_reach_square(GameState* s, int piece_index, int tr, int tc) {
    if (piece_index < 0 || piece_index >= s->piece_count) return 0;
    if (!in_bounds(tr, tc) || !isCellFree(s, tr, tc)) return 0;
    int r = s->pieces[piece_index].row;
    int c = s->pieces[piece_index].col;
    if (r == tr) {
        int dir = (tc > c) ? 1 : -1;
        for (int cc = c + dir; cc != tc; cc += dir) {
            if (!isCellFree(s, r, cc)) return 0;
        }
        return 1;
    } else if (c == tc) {
        int dir = (tr > r) ? 1 : -1;
        for (int rr = r + dir; rr != tr; rr += dir) {
            if (!isCellFree(s, rr, c)) return 0;
        }
        return 1;
    }
    return 0;
}

/**
 * \fn static int find_king_index(GameState* s, char color)
 * \brief Trouve l’index du roi d’une couleur.
 * 
 * \param s État du jeu.
 * \param color Couleur ('R' ou 'B').
 * \return Index du roi, ou -1 si non trouvé.
 */
static int find_king_index(GameState* s, char color) {
    for (int i = 0; i < s->piece_count; ++i)
        if (s->pieces[i].type == 'K' && s->pieces[i].color == color) return i;
    return -1;
}

/**
 * \fn static int can_color_reach_square(GameState* s, char color, int tr, int tc)
 * \brief Vérifie si une couleur peut atteindre une case par un déplacement rectiligne.
 * 
 * \param s État du jeu.
 * \param color Couleur du joueur.
 * \param tr Ligne cible.
 * \param tc Colonne cible.
 * \return 1 si une pièce peut atteindre la case, 0 sinon.
 */
static int can_color_reach_square(GameState* s, char color, int tr, int tc) {
    if (!in_bounds(tr, tc)) return 0;
    if (!isCellFree(s, tr, tc)) return 0; // doit être vide pour pouvoir y aller
    // Cherche une pièce de cette couleur qui peut atteindre (tr,tc)
    for (int i = 0; i < s->piece_count; ++i) {
        if (s->pieces[i].color != color) continue;
        int r = s->pieces[i].row, c = s->pieces[i].col;
        if (r == tr) {
            int dir = (tc > c) ? 1 : -1;
            int ok = 1;
            for (int cc = c + dir; cc != tc; cc += dir) {
                if (!isCellFree(s, r, cc)) { ok = 0; break; }
            }
            if (ok) return 1;
        } else if (c == tc) {
            int dir = (tr > r) ? 1 : -1;
            int ok = 1;
            for (int rr = r + dir; rr != tr; rr += dir) {
                if (!isCellFree(s, rr, c)) { ok = 0; break; }
            }
            if (ok) return 1;
        }
    }
    return 0;
}

/**
 * \fn static int ia_is_terminal(GameState* s)
 * \brief Vérifie si un état est terminal (roi capturé ou roi arrivé à la base adverse).
 * 
 * \param s État du jeu.
 * \return 1 si terminal, 0 sinon.
 */
static int is_square_linca_vulnerable(GameState* s, int r, int c, char victim_color) {
    char enemy = (victim_color == 'B') ? 'R' : 'B';
    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = { 0, 0,-1, 1};
    int enemy_king = find_king_index(s, enemy);
    int sever = 0;
    for (int d = 0; d < 4; ++d) {
        int r1 = r + dr[d], c1 = c + dc[d];
        int r2 = r + 2*dr[d], c2 = c + 2*dc[d];
        if (!in_bounds(r1, c1) || !in_bounds(r2, c2)) continue;
        if (!isCellFree(s, r1, c1)) continue; // l'adversaire doit pouvoir venir sur r1
        int idx2 = findPieceAt(s, r2, c2);
        if (idx2 >= 0 && s->pieces[idx2].color == enemy) {
            int base = (s->pieces[idx2].type == 'K') ? 2 : 1; // roi derrière = menace accrue
            // l'adversaire peut-il atteindre r1 ?
            if (can_color_reach_square(s, enemy, r1, c1)) {
                int level = base;
                if (enemy_king >= 0 && can_specific_piece_reach_square(s, enemy_king, r1, c1)) level = base + 1;
                if (level > sever) sever = level;
            }
        }
    }
    if (sever > 3) sever = 3;
    return sever;
}

/**
 * \fn static int is_square_seltou_vulnerable(GameState* s, int r, int c, char victim_color)
 * \brief Vérifie la vulnérabilité d’une case à une capture Seltou.
 * 
 * \param s État du jeu.
 * \param r Ligne.
 * \param c Colonne.
 * \param victim_color Couleur de la victime.
 * \return Niveau de sévérité (0–3).
 */
static int is_square_seltou_vulnerable(GameState* s, int r, int c, char victim_color) {
    char enemy = (victim_color == 'B') ? 'R' : 'B';
    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = { 0, 0,-1, 1};
    int enemy_king = find_king_index(s, enemy);
    int sever = 0;
    for (int d = 0; d < 4; ++d) {
        int r_adj = r + dr[d], c_adj = c + dc[d];
        if (!in_bounds(r_adj, c_adj)) continue;
        if (!isCellFree(s, r_adj, c_adj)) continue; // l'adversaire doit pouvoir se placer adjacent
        int r_back = r - dr[d], c_back = c - dc[d];
        if (in_bounds(r_back, c_back)) {
            int idx_back = findPieceAt(s, r_back, c_back);
            if (idx_back >= 0 && s->pieces[idx_back].color == victim_color) continue; // protégé -> pas de capture Seltou
        }
        if (can_color_reach_square(s, enemy, r_adj, c_adj)) {
            int level = 1;
            if (enemy_king >= 0 && can_specific_piece_reach_square(s, enemy_king, r_adj, c_adj)) level = 2;
            if (level > sever) sever = level;
        }
    }
    return sever;
}

/** \fn static inline int ia_find_after(GameState* gs, const Move* mmv, int row, int col)
 * \brief Trouve l’index d’une pièce après un coup, en tenant compte du déplacement.
 * 
 * \param gs État du jeu.
 * \param mmv Coup appliqué.
 * \param row Ligne.
 * \param col Colonne.
 * \return Index de la pièce, ou -1 si vide.
 */
static inline int ia_find_after(GameState* gs, const Move* mmv, int row, int col) {
    if (row == mmv->to_row && col == mmv->to_col) return mmv->piece_index;
    if (row == mmv->from_row && col == mmv->from_col) return -1;
    return findPieceAt(gs, row, col);
}

/** \fn static int ia_move_priority(GameState* s, const Move* mv)
 * \brief Calcule la priorité d’un coup pour le tri.
 * 
 * \param s État du jeu.
 * \param mv Coup à évaluer.
 * \return Priorité (plus élevé = meilleur).
 */
static void ia_check_linca_capture(GameState* s, int moved_index) {
    Piece* moved = &s->pieces[moved_index];
    int r = moved->row, c = moved->col;
    char ally = moved->color;
    char enemy = (ally == 'B') ? 'R' : 'B';
    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = { 0, 0,-1, 1};

    for (int d = 0; d < 4; ++d) {
        int r1 = r + dr[d], c1 = c + dc[d];
        int r2 = r + 2*dr[d], c2 = c + 2*dc[d];
        if (r2 < 0 || r2 > 8 || c2 < 0 || c2 > 8) continue;

        int idx1 = findPieceAt(s, r1, c1);
        int idx2 = findPieceAt(s, r2, c2);
        if (idx1 >= 0 && s->pieces[idx1].color == enemy &&
            idx2 >= 0 && s->pieces[idx2].color == ally) {

            // Roi capturé → suppression
            if (s->pieces[idx1].type == 'K') {
                for (int k = idx1; k < s->piece_count - 1; ++k) s->pieces[k] = s->pieces[k+1];
                --s->piece_count;
                return; // suffit
            }

            s->cell_control[r1][c1] = 0;
            for (int k = idx1; k < s->piece_count - 1; ++k) s->pieces[k] = s->pieces[k+1];
            --s->piece_count;
            // recommencer la boucle direction (au cas de chaînes)
            d = -1;
        }
    }
}

/**
 * \fn static void ia_check_seltou_capture(GameState* s, int moved_index, int old_row, int old_col)
 * \brief Vérifie et applique les captures de type Seltou après un déplacement.
 * 
 * \param s État du jeu.
 * \param moved_index Index de la pièce déplacée.
 * \param old_row Ancienne ligne.
 * \param old_col Ancienne colonne.
 */
static void ia_check_seltou_capture(GameState* s, int moved_index, int old_row, int old_col) {
    Piece* moved = &s->pieces[moved_index];
    char ally = moved->color;
    char enemy = (ally == 'B') ? 'R' : 'B';
    int r = moved->row, c = moved->col;

    int dr = 0, dc = 0;
    if (r != old_row && c != old_col) return;
    if (r == old_row && c == old_col) return;
    if (r != old_row) dr = (r > old_row) ? 1 : -1;
    if (c != old_col) dc = (c > old_col) ? 1 : -1;

    int enemy_row = r + dr, enemy_col = c + dc;
    if (enemy_row < 0 || enemy_row > 8 || enemy_col < 0 || enemy_col > 8) return;
    int idx_enemy = findPieceAt(s, enemy_row, enemy_col);
    if (idx_enemy < 0 || s->pieces[idx_enemy].color != enemy) return;

    int back_row = enemy_row + dr, back_col = enemy_col + dc;
    if (back_row >= 0 && back_row < 9 && back_col >= 0 && back_col < 9) {
        int idx_back = findPieceAt(s, back_row, back_col);
        if (idx_back >= 0 && s->pieces[idx_back].color == enemy) return; // protégé
    }

    // capture
    if (s->pieces[idx_enemy].type == 'K') {
        for (int k = idx_enemy; k < s->piece_count - 1; ++k) s->pieces[k] = s->pieces[k+1];
        --s->piece_count;
        return;
    }
    s->cell_control[enemy_row][enemy_col] = 0;
    for (int k = idx_enemy; k < s->piece_count - 1; ++k) s->pieces[k] = s->pieces[k+1];
    --s->piece_count;
}

/**
 * \fn static void ia_apply_move(GameState* s, const Move* mv)
 * \brief Applique un coup à l’état du jeu, y compris les captures.
 * 
 * \param s État du jeu.
 * \param mv Coup à appliquer.
 */
static void ia_apply_move(GameState* s, const Move* mv) {
    if (mv->piece_index < 0 || mv->piece_index >= s->piece_count) return;
    Piece* p = &s->pieces[mv->piece_index];
    int old_row = p->row, old_col = p->col;

    // Marquer les cases contrôlées (persistantes dans ce jeu)
    s->cell_control[old_row][old_col] = (p->color == 'B') ? 1 : 2;

    p->row = mv->to_row;
    p->col = mv->to_col;

    s->cell_control[p->row][p->col] = (p->color == 'B') ? 1 : 2;

    ia_check_linca_capture(s, mv->piece_index);
    int idx_now = findPieceAt(s, p->row, p->col);
    if (idx_now >= 0) ia_check_seltou_capture(s, idx_now, old_row, old_col);
}

/**
 * \fn static void ia_generate_moves(GameState* s, Move* list, int* count)
 * \brief Génère tous les coups valides pour le joueur courant.
 * 
 * \param s État du jeu.
 * \param list Tableau pour stocker les coups générés.
 * \param count Pointeur pour stocker le nombre de coups générés.
 */
static void ia_generate_moves(GameState* s, Move* list, int* count) {
    *count = 0;
    for (int i = 0; i < s->piece_count; ++i) {
        if (s->pieces[i].color != s->current_player) continue;
        int r = s->pieces[i].row, c = s->pieces[i].col;
        for (int cc = c + 1; cc < 9; ++cc) { if (!isCellFree(s, r, cc)) break; list[(*count)++] = (Move){i, r, c, r, cc}; }
        for (int cc = c - 1; cc >= 0; --cc){ if (!isCellFree(s, r, cc)) break; list[(*count)++] = (Move){i, r, c, r, cc}; }
        for (int rr = r + 1; rr < 9; ++rr) { if (!isCellFree(s, rr, c)) break; list[(*count)++] = (Move){i, r, c, rr, c}; }
        for (int rr = r - 1; rr >= 0; --rr){ if (!isCellFree(s, rr, c)) break; list[(*count)++] = (Move){i, r, c, rr, c}; }
    }
}

/** 
 * \brief Structure pour trier les coups par priorité
 */
/**
 * @brief Un mouvement avec sa priorité pour l'ordonnancement
 * 
 * Cette structure combine un mouvement avec un score de priorité
 * pour permettre l'ordonnancement des coups dans l'algorithme minimax.
 * Les coups sont triés par priorité décroissante avant d'être évalués.
 */
typedef struct {
    Move mv;    /**< Le mouvement à évaluer */
    int prio;   /**< Priorité du mouvement (plus grand = plus prioritaire) */
} ScoredMove;

/**
 * \fn static int cmp_scored_move_desc(const void* a, const void* b)
 * \brief Comparateur pour trier les coups par priorité décroissante.
 * 
 * \param a Premier coup.
 * \param b Deuxième coup.
 * \return Différence de priorité (b - a).
 */
static int cmp_scored_move_desc(const void* a, const void* b) {
    const ScoredMove* A = (const ScoredMove*)a;
    const ScoredMove* B = (const ScoredMove*)b;
    return (B->prio - A->prio);
}

/**
 * \fn static int is_king_capturable_next_turn(GameState* s, char king_color)
 * \brief Vérifie si le roi d’une couleur peut être capturé au prochain tour.
 * 
 * \param s État du jeu.
 * \param king_color Couleur du roi ('R' ou 'B').
 * \return 1 si le roi peut être capturé, 0 sinon.
 */
static int is_king_capturable_next_turn(GameState* s, char king_color) {
    char enemy = (king_color == 'B') ? 'R' : 'B';
    int king_index = find_king_index(s, king_color);
    if (king_index < 0) return 0; // Pas de roi, pas de capture possible

    int kr = s->pieces[king_index].row, kc = s->pieces[king_index].col;

    // Vérifier si le roi peut être capturé par Linca
    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = { 0, 0,-1, 1};

    for (int d = 0; d < 4; ++d) {
        int r1 = kr + dr[d], c1 = kc + dc[d];
        int r2 = kr + 2*dr[d], c2 = kc + 2*dc[d];

        // Vérifier si l'ennemi peut créer un sandwich
        if (in_bounds(r1, c1) && in_bounds(r2, c2)) {
            int idx2 = findPieceAt(s, r2, c2);
            // Si il y a déjà une pièce ennemie à r2, vérifier si l'ennemi peut placer une pièce à r1
            if (idx2 >= 0 && s->pieces[idx2].color == enemy) {
                if (can_color_reach_square(s, enemy, r1, c1)) {
                    return 1; // Le roi peut être capturé par Linca
                }
            }
            // Ou vérifier si l'ennemi peut créer le sandwich dans l'autre sens
            int idx1 = findPieceAt(s, r1, c1);
            if (idx1 >= 0 && s->pieces[idx1].color == enemy) {
                if (can_color_reach_square(s, enemy, r2, c2)) {
                    return 1; // Le roi peut être capturé par Linca
                }
            }
        }
    }

    // Vérifier si le roi peut être capturé par Seltou
    for (int d = 0; d < 4; ++d) {
        int r_adj = kr + dr[d], c_adj = kc + dc[d];
        if (in_bounds(r_adj, c_adj) && isCellFree(s, r_adj, c_adj)) {
            // Vérifier si l'arrière du roi est libre ou hors limites
            int r_back = kr - dr[d], c_back = kc - dc[d];
            int protected = 0;
            if (in_bounds(r_back, c_back)) {
                int idx_back = findPieceAt(s, r_back, c_back);
                if (idx_back >= 0 && s->pieces[idx_back].color == king_color) {
                    protected = 1;
                }
            }
            if (!protected && can_color_reach_square(s, enemy, r_adj, c_adj)) {
                return 1; // Le roi peut être capturé par Seltou
            }
        }
    }

    return 0;
}

/** \fn add_move_to_history(const Move* mv)
 * \brief Ajoute un coup à l'historique des coups.
 * 
 * \param mv Coup à ajouter.
 */
static void add_move_to_history(const Move* mv) {
    if (history_count < MAX_MOVE_HISTORY) {
        move_history[history_count] = *mv;
        history_count++;
    } else {
        // Décaler l'historique (FIFO)
        for (int i = 0; i < MAX_MOVE_HISTORY - 1; i++) {
            move_history[i] = move_history[i + 1];
        }
        move_history[MAX_MOVE_HISTORY - 1] = *mv;
    }
}

/** \fn static int is_move_recent(const Move* mv, int lookback)
 * \brief Vérifie si un coup similaire a été joué récemment.
 * 
 * \param mv Coup à vérifier.
 * \param lookback Nombre de coups récents à vérifier.
 * \return 1 si un coup similaire a été joué récemment, 0 sinon.
 */
static int is_move_recent(const Move* mv, int lookback) {
    if (lookback > history_count) lookback = history_count;

    for (int i = history_count - lookback; i < history_count; i++) {
        if (move_history[i].piece_index == mv->piece_index &&
            move_history[i].from_row == mv->from_row &&
            move_history[i].from_col == mv->from_col &&
            move_history[i].to_row == mv->to_row &&
            move_history[i].to_col == mv->to_col) {
            return 1;
        }
    }
    return 0;
}

/** \fn static int count_similar_recent_moves(const Move* mv, int lookback)
 * \brief Compte combien de fois un coup similaire a été joué récemment.
 * 
 * \param mv Coup à vérifier.
 * \param lookback Nombre de coups récents à vérifier.
 * \return Nombre de coups similaires joués récemment.
 */
static int count_similar_recent_moves(const Move* mv, int lookback) {
    if (lookback > history_count) lookback = history_count;

    int count = 0;
    for (int i = history_count - lookback; i < history_count; i++) {
        if (move_history[i].piece_index == mv->piece_index) {
            // Même direction de mouvement?
            int old_dr = move_history[i].to_row - move_history[i].from_row;
            int old_dc = move_history[i].to_col - move_history[i].from_col;
            int new_dr = mv->to_row - mv->from_row;
            int new_dc = mv->to_col - mv->from_col;

            // Normaliser les directions (-1, 0, 1)
            if (old_dr != 0) old_dr = (old_dr > 0) ? 1 : -1;
            if (old_dc != 0) old_dc = (old_dc > 0) ? 1 : -1;
            if (new_dr != 0) new_dr = (new_dr > 0) ? 1 : -1;
            if (new_dc != 0) new_dc = (new_dc > 0) ? 1 : -1;

            if (old_dr == new_dr && old_dc == new_dc) {
                count++;
            }
        }
    }
    return count;
}

/**
 * \brief 
 * Variables externes pour les scores (utilisées dans l’évaluation).
 */
extern int score_blue, score_red;

/**
 * \fn static int is_loop_detected(const Move* mv)
 * \brief Détecte si un coup crée une boucle de répétition.
 * 
 * \param mv Coup à vérifier.
 * \return 1 si une boucle est détectée, 0 sinon.
 */
static int is_loop_detected(const Move* mv) {
    if (history_count < 4) return 0; // Besoin d'au moins 4 coups pour voir 2 de mes coups

    // Vérifier si je répète le même coup depuis 3 tours
    // history[count-1] = dernier coup adversaire
    // history[count-2] = MON dernier coup
    // history[count-3] = avant-dernier coup adversaire
    // history[count-4] = MON avant-dernier coup

    Move* my_last = &move_history[history_count - 2];      // Mon dernier coup
    Move* my_before = &move_history[history_count - 4];    // Mon avant-dernier coup

    // Si le coup actuel == mon dernier coup == mon avant-dernier coup
    if (mv->piece_index == my_last->piece_index &&
        mv->from_row == my_last->from_row && mv->from_col == my_last->from_col &&
        mv->to_row == my_last->to_row && mv->to_col == my_last->to_col &&

        my_last->piece_index == my_before->piece_index &&
        my_last->from_row == my_before->from_row && my_last->from_col == my_before->from_col &&
        my_last->to_row == my_before->to_row && my_last->to_col == my_before->to_col) {

        return 1; // Même coup joué 3 fois consécutives
    }

    return 0;
}

/**
 * \fn static int ia_move_priority(GameState* s, const Move* mv)
 * \brief Calcule la priorité d’un coup pour le tri.
 * 
 * \param s État du jeu.
 * \param mv Coup à évaluer.
 * \return Priorité (plus élevé = meilleur).
 */
static int ia_move_priority(GameState* s, const Move* mv) {
    const Piece* pc = &s->pieces[mv->piece_index];
    int pr = 0;

    // 0) Anti-boucle SIMPLIFIÉ - bannir TOUTES les boucles
    if (is_loop_detected(mv)) {
        return -999999999; // Bannir TOUTES les boucles, peu importe le score
    }
    // 1) Coups gagnants absolus
    //    a) capture du roi adverse par l'effet du coup
    //    b) roi qui atteint sa cité

    // a) Détection de capture de roi (Linca/Seltou) après ce coup
    {
        int tr = mv->to_row, tc = mv->to_col;
        char ally = pc->color;
        char enemy = (ally == 'B') ? 'R' : 'B';
        const int dr[4] = {-1, 1, 0, 0};
        const int dc[4] = { 0, 0,-1, 1};
        // Linca
        for (int d = 0; d < 4; ++d) {
            int r1 = tr + dr[d], c1 = tc + dc[d];
            int r2 = tr + 2*dr[d], c2 = tc + 2*dc[d];
            if (r2 >= 0 && r2 < 9 && c2 >= 0 && c2 < 9) {
                int idx1 = ia_find_after(s, mv, r1, c1);
                int idx2 = ia_find_after(s, mv, r2, c2);
                if (idx1 >= 0 && s->pieces[idx1].color == enemy && s->pieces[idx1].type == 'K' &&
                    idx2 >= 0 && s->pieces[idx2].color == ally) {
                    return 10000000; // priorité absolue (capture roi)
                }
            }
        }
        // Seltou (uniquement dans la direction du mouvement)
        int mdr = 0, mdc = 0;
        if (mv->to_row != mv->from_row) mdr = (mv->to_row > mv->from_row) ? 1 : -1;
        if (mv->to_col != mv->from_col) mdc = (mv->to_col > mv->from_col) ? 1 : -1;
        if ((mdr == 0) != (mdc == 0)) {
            int er = tr + mdr, ec = tc + mdc;
            if (er >= 0 && er < 9 && ec >= 0 && ec < 9) {
                int idxe = ia_find_after(s, mv, er, ec);
                if (idxe >= 0 && s->pieces[idxe].color == enemy && s->pieces[idxe].type == 'K') {
                    int br = er + mdr, bc = ec + mdc;
                    if (br >= 0 && br < 9 && bc >= 0 && bc < 9) {
                        int idxb = ia_find_after(s, mv, br, bc);
                        if (!(idxb >= 0 && s->pieces[idxb].color == enemy)) {
                            return 10000000; // capture roi non protégée
                        }
                    } else {
                        // hors limite derrière = non protégé -> capture
                        return 10000000;
                    }
                }
            }
        }
    }

    // 1) Roi qui atteint la base adverse
    if (pc->type == 'K') {
        if ((pc->color == 'R' && mv->to_row == 0 && mv->to_col == 0) ||
            (pc->color == 'B' && mv->to_row == 8 && mv->to_col == 8)) {
            return 5000000;
        }
    }
    // 2) Amélioration de la distance du roi vers l’objectif
    if (pc->type == 'K') {
        int old_d = (pc->color == 'R')
                  ? (abs_i(pc->row - 0) + abs_i(pc->col - 0))
                  : (abs_i(pc->row - 8) + abs_i(pc->col - 8));
        int new_d = (pc->color == 'R')
                  ? (abs_i(mv->to_row - 0) + abs_i(mv->to_col - 0))
                  : (abs_i(mv->to_row - 8) + abs_i(mv->to_col - 8));
        pr += (old_d - new_d) * 50; // avancer le roi = mieux
    }
    // 3) Centralisation (vers 4,4)
    int center_old = abs_i(s->pieces[mv->piece_index].row - 4) + abs_i(s->pieces[mv->piece_index].col - 4);
    int center_new = abs_i(mv->to_row - 4) + abs_i(mv->to_col - 4);
    pr += (center_old - center_new) * 2;
    // 4) Longueur du déplacement
    pr += abs_i(mv->to_row - mv->from_row) + abs_i(mv->to_col - mv->from_col);


    // 5) Vérification critique: le roi peut-il être capturé après ce mouvement?
    {
        GameState t = *s;
        ia_apply_move(&t, mv);
        char my_color = pc->color;

        // Si ce mouvement expose notre roi à une capture immédiate, rejeter complètement le coup
        if (is_king_capturable_next_turn(&t, my_color)) {
            return -50000000; // Priorité extrêmement négative pour éviter la capture du roi
        }
    }

    // 6) Gestion des répétitions selon le score
    {
        char my_color = pc->color;
        int my_score = (my_color == 'B') ? score_blue : score_red;
        int enemy_score = (my_color == 'B') ? score_red : score_blue;
        int score_diff = my_score - enemy_score;

        // Si on est en retard ou à égalité, pénaliser fortement les répétitions
        if (score_diff <= 0) {
            // Pénalité pour coup exactement identique récent
            if (is_move_recent(mv, 4)) {
                pr -= 150000; // Forte pénalité pour éviter les boucles
            }

            // Pénalité pour coups similaires répétés
            int similar_count = count_similar_recent_moves(mv, 6);
            if (similar_count > 0) {
                pr -= similar_count * 8000; // Pénalité croissante
            }

            // Bonus pour l'exploration si on est en retard
            if (score_diff < 0) {
                // Favoriser les coups "nouveaux" qui n'ont pas été essayés récemment
                if (!is_move_recent(mv, 6) && count_similar_recent_moves(mv, 6) == 0) {
                    pr += 3000; // Bonus d'exploration
                }
            }
        }
    }

    // 7) Sécurité: éviter d'exposer la pièce (surtout le roi) à une capture immédiate (Linca/Seltou)
    {
        GameState t = *s;
        ia_apply_move(&t, mv);
        int idx = findPieceAt(&t, mv->to_row, mv->to_col);
        if (idx >= 0) {
            char my_color = t.pieces[idx].color;
            int r = t.pieces[idx].row, c = t.pieces[idx].col;
            int linca_v = is_square_linca_vulnerable(&t, r, c, my_color);   // 0..3
            int seltou_v = is_square_seltou_vulnerable(&t, r, c, my_color); // 0..2
            int penalty = 0;
            penalty += linca_v * 2200;   // Linca plus sévère, surtout si roi adverse impliqué
            penalty += seltou_v * 1100;
            if (t.pieces[idx].type == 'K') penalty *= 8; // beaucoup plus sévère pour le roi
            pr -= penalty;
        }
    }

    return pr;
}

/**
 * \fn static void ia_generate_sorted_moves(GameState* s, ScoredMove* out, int* count)
 * \brief Génère et trie les coups par priorité.
 * 
 * \param s État du jeu.
 * \param out Tableau pour stocker les coups triés.
 * \param count Pointeur pour stocker le nombre de coups générés.
 */
static void ia_generate_sorted_moves(GameState* s, ScoredMove* out, int* count) {
    Move tmp[300];
    ia_generate_moves(s, tmp, count);
    for (int i = 0; i < *count; ++i) {
        out[i].mv = tmp[i];
        out[i].prio = ia_move_priority(s, &tmp[i]);
    }
    qsort(out, *count, sizeof(ScoredMove), cmp_scored_move_desc);
}

/**
 * \fn static int ia_is_terminal(GameState* s)
 * \brief Vérifie si l’état du jeu est terminal (victoire/défaite).
 * 
 * \param s État du jeu.
 * \return 1 si terminal, 0 sinon.
 */
static int ia_is_terminal(GameState* s) {
    // Roi atteint la base adverse
    for (int i = 0; i < s->piece_count; ++i) {
        Piece *p = &s->pieces[i];
        if (p->type == 'K') {
            if ((p->color == 'R' && p->row == 0 && p->col == 0) ||
                (p->color == 'B' && p->row == 8 && p->col == 8)) return 1;
        }
    }
    // Roi capturé
    int r_king = 0, b_king = 0;
    for (int i = 0; i < s->piece_count; ++i) if (s->pieces[i].type == 'K') {
        if (s->pieces[i].color == 'R') r_king = 1; else b_king = 1;
    }
    return (r_king == 0 || b_king == 0);
}

/**
 * \fn static int ia_count_mobility(GameState* s, char color)
 * \brief Compte la mobilité (nombre de cases accessibles) pour une couleur.
 * 
 * \param s État du jeu.
 * \param color Couleur ('R' ou 'B').
 * \return Nombre de cases accessibles.
 */
static int ia_count_mobility(GameState* s, char color) {
    int mv = 0;
    for (int i = 0; i < s->piece_count; ++i) {
        if (s->pieces[i].color != color) continue;
        int r = s->pieces[i].row, c = s->pieces[i].col;
        for (int cc = c + 1; cc < 9; ++cc) { if (isCellFree(s, r, cc)) mv++; else break; }
        for (int cc = c - 1; cc >= 0; --cc){ if (isCellFree(s, r, cc)) mv++; else break; }
        for (int rr = r + 1; rr < 9; ++rr) { if (isCellFree(s, rr, c)) mv++; else break; }
        for (int rr = r - 1; rr >= 0; --rr){ if (isCellFree(s, rr, c)) mv++; else break; }
    }
    return mv;
}

/**
 * \fn static int ia_eval(GameState* s, char perspective)
 * \brief Évalue l’état du jeu pour une couleur donnée.
 * 
 * \param s État du jeu.
 * \param perspective Couleur de la perspective ('R' ou 'B').
 * \return Score (positif = avantage pour la couleur, négatif = désavantage).
 */
static int ia_eval(GameState* s, char perspective) {
    int score = 0;
    int r_kr = -1, r_kc = -1;
    int b_kr = -1, b_kc = -1;
    int red_pawns = 0, blue_pawns = 0;

    for (int i = 0; i < s->piece_count; ++i) {
        Piece *pc = &s->pieces[i];
        if (pc->type == 'K') {
            if (pc->color == 'R') { r_kr = pc->row; r_kc = pc->col; }
            else { b_kr = pc->row; b_kc = pc->col; }
        } else {
            if (pc->color == 'R') red_pawns++; else blue_pawns++;
        }
    }
    if (r_kr < 0) return (perspective == 'B') ? 30000 : -30000;
    if (b_kr < 0) return (perspective == 'R') ? 30000 : -30000;

    // Matériel
    score += (red_pawns - blue_pawns) * 100;

    // Objectifs des rois
    score += (18 - (abs_i(r_kr - 0) + abs_i(r_kc - 0))) * 8;
    score -= (18 - (abs_i(b_kr - 8) + abs_i(b_kc - 8))) * 8;

    // Mobilité
    score += (ia_count_mobility(s, 'R') - ia_count_mobility(s, 'B')) * 2;

    // Contrôle de cases persistantes
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c) {
            if (s->cell_control[r][c] == 2) score += 1;
            else if (s->cell_control[r][c] == 1) score -= 1;
        }

    return (perspective == 'R') ? score : -score;
}

/**
 * \fn int isGameOver(GameState* jeu)
 * \brief Vérifie si le jeu est terminé.
 * 
 * \param jeu État du jeu.
 * \return 1 si le jeu est terminé, 0 sinon.
 */
int isGameOver(GameState* jeu) { return ia_is_terminal(jeu); }

/**
 * \fn int evaluation(GameState* jeu, char evaluating_player)
 * \brief Évalue l’état du jeu pour une couleur donnée.
 * 
 * \param jeu État du jeu.
 * \param evaluating_player Couleur de la perspective ('R' ou 'B').
 * \return Score (positif = avantage pour la couleur, négatif = désavantage).
 */
int evaluation(GameState* jeu, char evaluating_player) { return ia_eval(jeu, evaluating_player); }

/**
 * \fn static int minimaxIA(GameState* jeu, int profondeur, char maximizing_player, int alpha, int beta)
 * \brief Algorithme Minimax avec élagage alpha-bêta.
 * 
 * \param jeu État du jeu.
 * \param profondeur Profondeur de recherche.
 * \param maximizing_player Couleur du joueur maximisant ('R' ou 'B').
 * \param alpha Valeur alpha pour l'élagage.
 * \param beta Valeur beta pour l'élagage.
 * \return Score évalué.
 */
int minimaxIA(GameState* jeu, int profondeur, char maximizing_player, int alpha, int beta) {
    if (profondeur == 0 || ia_is_terminal(jeu)) {
        // Donner un score extrême en cas de terminal (victoire/défaite immédiate)
        // Détecter le vainqueur
        int red_king = -1, blue_king = -1;
        for (int i = 0; i < jeu->piece_count; ++i) if (jeu->pieces[i].type == 'K') {
            if (jeu->pieces[i].color == 'R') red_king = i; else blue_king = i;
        }
        int red_wins = 0, blue_wins = 0;
        if (red_king >= 0 && jeu->pieces[red_king].row == 0 && jeu->pieces[red_king].col == 0) red_wins = 1;
        if (blue_king >= 0 && jeu->pieces[blue_king].row == 8 && jeu->pieces[blue_king].col == 8) blue_wins = 1;
        if (red_king < 0) blue_wins = 1;
        if (blue_king < 0) red_wins = 1;
        if (red_wins || blue_wins) {
            int winner = red_wins ? 'R' : 'B';
            return (winner == maximizing_player) ? 30000000 : -30000000;
        }
        return ia_eval(jeu, maximizing_player);
    }
    ScoredMove moves[300];
    int n; ia_generate_sorted_moves(jeu, moves, &n);
    if (n == 0) return (jeu->current_player == maximizing_player) ? -20000 : 20000;

    int is_max = (jeu->current_player == maximizing_player);
    int best = is_max ? -100000000 : 100000000;

    for (int i = 0; i < n; ++i) {
        GameState backup = *jeu;
        ia_apply_move(jeu, &moves[i].mv);
        jeu->current_player = (jeu->current_player == 'R') ? 'B' : 'R';

        int v = minimaxIA(jeu, profondeur - 1, maximizing_player, alpha, beta);
        *jeu = backup;

        if (is_max) {
            if (v > best) best = v;
            if (v > alpha) alpha = v;
        } else {
            if (v < best) best = v;
            if (v < beta) beta = v;
        }
        if (beta <= alpha) break;
    }
    return best;
}

/**
 * \fn void trouverMeilleurCoupIA(GameState* jeu, Move* best_move, int profondeur)
 * \brief Trouve le meilleur coup pour l’IA en utilisant Minimax.
 * 
 * \param jeu État du jeu.
 * \param best_move Pointeur pour stocker le meilleur coup trouvé.
 * \param profondeur Profondeur de recherche.
 */
void trouverMeilleurCoupIA(GameState* jeu, Move* best_move, int profondeur) {
    ScoredMove moves[300];
    int n; ia_generate_sorted_moves(jeu, moves, &n);
    best_move->piece_index = -1;
    if (n == 0) return;

    int alpha = -100000000, beta = 100000000;
    int best_val = -100000000;
    char maximizing = jeu->current_player;

    for (int i = 0; i < n; ++i) {
        // VÉRIFICATION ANTI-BOUCLE dans le minimax aussi !
        if (is_loop_detected(&moves[i].mv)) {
            continue; // Ignorer complètement ce coup
        }

        GameState backup = *jeu;
        ia_apply_move(jeu, &moves[i].mv);
        jeu->current_player = (jeu->current_player == 'R') ? 'B' : 'R';
        int v = minimaxIA(jeu, profondeur - 1, maximizing, alpha, beta);
        *jeu = backup;

        if (v > best_val) {
            best_val = v;
            *best_move = moves[i].mv;
        }
        if (v > alpha) alpha = v;
        if (beta <= alpha) break;
    }

    // Ajouter le meilleur coup à l'historique pour la détection de répétitions
    if (best_move->piece_index >= 0) {
        add_move_to_history(best_move);
    }
}

/**
 * \fn  void reset_move_history(void)
 * \brief Réinitialise l'historique des coups.
 */
void reset_move_history(void) {
    history_count = 0;
}

/**
 * \fn GameState createGameStateFromCurrent(void)
 * \brief Crée une copie de l’état actuel du jeu.
 * 
 * \return Copie de l’état actuel du jeu.
 */
GameState createGameStateFromCurrent(void) {
    GameState st;
    st.piece_count = piece_count;
    for (int i = 0; i < piece_count; ++i) st.pieces[i] = pieces[i];
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c)
            st.cell_control[r][c] = cell_control[r][c];
    st.current_player = current_turn;
    st.turn_number = turn_number;
    return st;
}
