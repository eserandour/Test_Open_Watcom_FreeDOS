/* =========================================================
   SCENE2.C — Scène : démonstration palette VGA
   =========================================================
   Affiche une grille de 16x16 carrés, un par couleur de
   palette, et fait évoluer la palette en plusieurs phases.

   Phases et durées :
   1.  3 s : fade-in progressif depuis le noir
   2.  1 s : affichage statique (palette par défaut)
   3.  5 s : cycle de palette vers la droite
   4.  5 s : cycle de palette vers la gauche
   5.  3 s : interpolation (lerp) vers pinkPalette
   Puis    : transition vers SCENE_3

   Total : environ 17 secondes.

   Principe du cycle de palette :
   On ne redessine pas les pixels, on déplace seulement
   les couleurs dans le DAC VGA. Les pixels gardent leurs
   index (0-255) mais les couleurs associées à ces index
   changent, créant un effet de mouvement sans recalcul.
   ========================================================= */

#include "timer.h"    /* readTimer, elapsedTimeMs, TARGET_HZ */
#include "video.h"    /* clearScreen, flip, SCREEN_*        */
#include "palette.h"  /* toutes les fonctions palette       */
#include "graphics.h" /* drawRectFill                       */
#include "scene.h"    /* sceneStart, setScene, SCENE_3    */

/* =========================================================
   FONCTION LOCALE : grille de palette
   ========================================================= */

/* Dessine la palette courante sous forme de grille 16x16.
   Chaque cellule est un carré de 10x10 pixels de couleur
   uniforme, avec 2 pixels d'espacement entre les cellules.
   La grille est centrée sur l'écran.
   static = visible uniquement dans ce fichier. */
static void drawPaletteGrid(void)
{
    const int gridSize = 16;      /* 16 colonnes et 16 lignes  */
    const int cellSize = 10;      /* taille d'une cellule (px) */
    const int spacing  = 2;       /* espace entre cellules (px)*/
    const int step     = cellSize + spacing;  /* pas entre deux cellules */

    /* Taille totale de la grille en pixels. */
    const int gridW = gridSize * cellSize + (gridSize - 1) * spacing;
    const int gridH = gridSize * cellSize + (gridSize - 1) * spacing;

    /* Offset de centrage. */
    const int offsetX = (SCREEN_WIDTH  - gridW) / 2;
    const int offsetY = (SCREEN_HEIGHT - gridH) / 2;

    int x, y;
    int i = 0;   /* index de couleur (0 à 255) */

    /* Parcourir les 16*16 = 256 cellules.
       i s'incrémente de gauche à droite, de haut en bas. */
    for (y = 0; y < gridSize; y++)
        for (x = 0; x < gridSize; x++)
        {
            int px = offsetX + x * step;   /* coin gauche de la cellule */
            int py = offsetY + y * step;   /* coin haut de la cellule   */

            /* Dessiner un carré plein de la couleur i.
               La couleur réelle dépend de la palette active
               dans le DAC VGA, pas des composantes RGB
               stockées dans workingPalette. */
            drawRectFill(px, py, px + cellSize - 1, py + cellSize - 1,
                         (unsigned char)i++);
        }
}

/* =========================================================
   SCÈNE PRINCIPALE
   ========================================================= */

void scene2(void)
{
    /* Variables statiques : persistent entre les appels. */
    static unsigned long lastRender         = 0UL;
    static int           initialized        = 0;
    static int           phase5_initialized = 0;   /* flag pour la phase 5 */

    /* -------------------------------------------------
       Durées de chaque phase (ms)
       ------------------------------------------------- */
    const unsigned long D1 = 3000UL;   /* fade-in          */
    const unsigned long D2 = 1000UL;   /* statique         */
    const unsigned long D3 = 5000UL;   /* cycle droite     */
    const unsigned long D4 = 5000UL;   /* cycle gauche     */
    const unsigned long D5 = 3000UL;   /* lerp → pink      */

    /* Seuils cumulés : à quel moment (depuis sceneStart)
       chaque phase se termine. */
    const unsigned long T1 = D1;
    const unsigned long T2 = T1 + D2;
    const unsigned long T3 = T2 + D3;
    const unsigned long T4 = T3 + D4;
    const unsigned long T5 = T4 + D5;

    /* Intervalle de rafraîchissement pour les phases
       animées (cycle, lerp) : 25 ms ≈ 40 Hz. */
    const unsigned long render_interval_ms = 25UL;

    unsigned long now     = readTimer();
    unsigned long elapsed = elapsedTimeMs(sceneStart, now);

    /* -------------------------------------------------------
       Initialisation (une seule fois)
       ------------------------------------------------------- */
    if (!initialized)
    {
        initialized = 1;

        /* Partir de la palette par défaut comme base. */
        copyPalette(workingPalette, defaultPalette);

        /* Effacer l'écran en noir et afficher immédiatement
           pour éviter d'afficher les restes de la scène
           précédente pendant le fade-in. */
        clearScreen(0);
        flip();
    }

    /* -------------------------------------------------------
       Phase 1 : Fade-in (0 → D1 ms)
       Interpole la luminosité de 0% à 100% en appliquant
       un facteur t croissant à la palette.
       ------------------------------------------------------- */
    if (elapsed < T1)
    {
        /* t = 0.0 au début de la phase, 1.0 à la fin. */
        float t = (float)elapsed / (float)D1;

        /* fadePalette envoie la palette au DAC avec chaque
           composante multipliée par t (sans modifier
           workingPalette en RAM). */
        fadePalette(workingPalette, t);
        drawPaletteGrid();
        flip();
        return;
    }

    /* -------------------------------------------------------
       Phase 2 : Affichage statique (T1 → T2 ms)
       La palette est à pleine intensité, rien ne change.
       ------------------------------------------------------- */
    if (elapsed < T2)
    {
        drawPaletteGrid();
        flip();
        return;
    }

    /* -------------------------------------------------------
       Phase 3 : Cycle vers la droite (T2 → T3 ms)
       Décale toutes les couleurs d'un cran vers la droite
       dans le DAC à intervalle régulier. Les pixels gardent
       leurs index mais voient leurs couleurs changer.
       ------------------------------------------------------- */
    if (elapsed < T3)
    {
        if (elapsedTimeMs(lastRender, now) >= render_interval_ms)
        {
            cyclePaletteRight(workingPalette, 0, 255);
            drawPaletteGrid();
            flip();
            lastRender = now;
        }
        return;
    }

    /* -------------------------------------------------------
       Phase 4 : Cycle vers la gauche (T3 → T4 ms)
       Même principe mais dans le sens inverse.
       ------------------------------------------------------- */
    if (elapsed < T4)
    {
        if (elapsedTimeMs(lastRender, now) >= render_interval_ms)
        {
            cyclePaletteLeft(workingPalette, 0, 255);
            drawPaletteGrid();
            flip();
            lastRender = now;
        }
        return;
    }

    /* -------------------------------------------------------
       Phase 5 : Lerp vers pinkPalette (T4 → T5 ms)
       Interpolation linéaire entre la palette de travail
       actuelle (après les cycles) et pinkPalette.
       On sauvegarde paletteA une seule fois au début de
       la phase pour que l'interpolation parte toujours du
       même point (et non de la palette en cours de lerp).
       ------------------------------------------------------- */
    if (elapsed < T5)
    {
        unsigned long fadeElapsed = elapsed - T4;   /* temps dans cette phase */
        float t = (float)fadeElapsed / (float)D5;   /* 0.0 → 1.0             */

        /* Sauvegarder la palette de départ une seule fois. */
        if (!phase5_initialized)
        {
            copyPalette(paletteA, workingPalette);  /* état actuel → source  */
            copyPalette(paletteB, pinkPalette);     /* cible                 */
            phase5_initialized = 1;
        }

        /* Calculer la palette interpolée et l'envoyer au DAC. */
        lerpPalette(workingPalette, paletteA, paletteB, t);
        setPalette(workingPalette);
        drawPaletteGrid();
        flip();
        return;
    }

    /* -------------------------------------------------------
       Fin de scène : transition vers SCENE_3
       Remettre les flags à 0 pour permettre une réactivation
       propre si on revient sur cette scène plus tard.
       ------------------------------------------------------- */
    initialized        = 0;
    phase5_initialized = 0;
    setScene(SCENE_3);
}
