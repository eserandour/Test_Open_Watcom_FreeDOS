/* =========================================================
   GRAPHICS.C — Primitives de dessin 2D en mode 13h
   ========================================================= */

#include <stdlib.h>   /* abs                               */
#include <string.h>   /* _fmemset                         */
#include "video.h"    /* backbuffer, OFFSET, SCREEN_*      */
#include "graphics.h"

/* =========================================================
   EFFACEMENT
   ========================================================= */

/* Remplit le backbuffer entier avec une seule couleur.
   _fmemset est la version far de memset : nécessaire car
   backbuffer est un pointeur far (segment:offset 32 bits). */
void clearScreen(unsigned char color)
{
    _fmemset(backbuffer, color, BACKBUFFER_SIZE);
}

/* =========================================================
   PIXEL
   ========================================================= */

/* Écrit un pixel à l'offset calculé par la macro OFFSET.
   OFFSET(x, y) = y*320 + x, optimisé avec des shifts. */
void putPixel(int x, int y, unsigned char color)
{
    backbuffer[OFFSET(x, y)] = color;
}

/* Lit la couleur (index palette) d'un pixel du backbuffer. */
unsigned char getPixel(int x, int y)
{
    return backbuffer[OFFSET(x, y)];
}

/* =========================================================
   LIGNE — Algorithme de Bresenham
   =========================================================
   Principe : on avance pas à pas de (x1,y1) vers (x2,y2).
   À chaque étape on choisit le pixel le plus proche de la
   droite idéale en utilisant une variable d'erreur entière.
   Pas de virgule flottante, pas de division : très rapide.
   ========================================================= */

void drawLine(int x1, int y1, int x2, int y2, unsigned char color)
{
    int dx  = abs(x2 - x1);    /* distance horizontale        */
    int dy  = abs(y2 - y1);    /* distance verticale          */
    int sx  = (x1 < x2) ? 1 : -1;  /* sens horizontal (+1 ou -1) */
    int sy  = (y1 < y2) ? 1 : -1;  /* sens vertical   (+1 ou -1) */
    int err = dx - dy;          /* accumulateur d'erreur       */
    int e2;                     /* erreur doublée (2 * err)    */

    while (1)
    {
        putPixel(x1, y1, color);

        /* Condition d'arrêt : on a atteint le point final. */
        if (x1 == x2 && y1 == y2) break;

        /* e2 = 2 * err, calculé par décalage binaire.
           On compare e2 à -dy et dx pour décider si on
           avance horizontalement, verticalement, ou les deux
           (cas diagonal). */
        e2 = err << 1;
        if (e2 > -dy) { err -= dy; x1 += sx; }  /* avancer en X */
        if (e2 <  dx) { err += dx; y1 += sy; }  /* avancer en Y */
    }
}

/* =========================================================
   RECTANGLE
   ========================================================= */

/* Contour : 4 segments reliant les 4 coins. */
void drawRect(int x1, int y1, int x2, int y2, unsigned char color)
{
    drawLine(x1, y1, x2, y1, color);   /* bord haut   */
    drawLine(x2, y1, x2, y2, color);   /* bord droit  */
    drawLine(x2, y2, x1, y2, color);   /* bord bas    */
    drawLine(x1, y2, x1, y1, color);   /* bord gauche */
}

/* Rectangle plein : remplissage ligne par ligne.
   Pour chaque ligne y, on calcule l'offset du pixel (x1,y)
   et on remplit x2-x1+1 octets avec _fmemset.
   Beaucoup plus rapide que putPixel en boucle. */
void drawRectFill(int x1, int y1, int x2, int y2, unsigned char color)
{
    int y;
    for (y = y1; y <= y2; y++)
        _fmemset(backbuffer + OFFSET(x1, y), color, x2 - x1 + 1);
}

/* =========================================================
   POLYGONE
   ========================================================= */

/* Contour : on trace un segment entre chaque paire de
   sommets consécutifs, puis on ferme le polygone en reliant
   le dernier sommet au premier. */
void drawPolygon(int *pts, int n, unsigned char color)
{
    int i;
    if (n < 2) return;
    for (i = 0; i < n - 1; i++)
        drawLine(pts[i*2], pts[i*2+1], pts[i*2+2], pts[i*2+3], color);

    /* Fermeture : dernier sommet → premier sommet. */
    drawLine(pts[(n-1)*2], pts[(n-1)*2+1], pts[0], pts[1], color);
}

/* =========================================================
   POLYGONE PLEIN — Scanline filling
   =========================================================
   Principe (algorithme "even-odd rule") :
   Pour chaque ligne horizontale (scanline y) :
   1. Trouver toutes les intersections avec les arêtes.
   2. Trier ces intersections par ordre croissant de X.
   3. Remplir les pixels entre les paires (x0,x1), (x2,x3)...
   ========================================================= */

void drawPolygonFill(int *pts, int n, unsigned char color)
{
    int i, j;
    int y, ymin, ymax;
    int x1, y1, x2, y2;
    int intersections[SCREEN_WIDTH];  /* liste des X d'intersection */
    int count, tmp, dx, dy, x;

    if (n < 3) return;   /* il faut au moins un triangle */

    /* Trouver les bornes verticales du polygone. */
    ymin = pts[1]; ymax = pts[1];
    for (i = 1; i < n; i++)
    {
        if (pts[i*2+1] < ymin) ymin = pts[i*2+1];
        if (pts[i*2+1] > ymax) ymax = pts[i*2+1];
    }

    /* Clipping vertical : ne traiter que les lignes visibles. */
    if (ymin < 0)              ymin = 0;
    if (ymax >= SCREEN_HEIGHT) ymax = SCREEN_HEIGHT - 1;

    /* Parcourir chaque scanline dans les bornes verticales. */
    for (y = ymin; y <= ymax; y++)
    {
        count = 0;   /* nombre d'intersections trouvées */

        /* Parcourir chaque arête du polygone.
           j = indice du sommet suivant (avec bouclage). */
        for (i = 0; i < n; i++)
        {
            j  = (i + 1) % n;
            x1 = pts[i*2];   y1 = pts[i*2+1];
            x2 = pts[j*2];   y2 = pts[j*2+1];

            /* L'arête croise la scanline si y est strictement
               entre y1 et y2 (une extrémité incluse, l'autre
               exclue pour éviter de compter deux fois un
               sommet partagé par deux arêtes). */
            if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y))
            {
                /* Interpolation linéaire entière pour trouver
                   le X d'intersection :
                   x = x1 + (x2-x1) * (y-y1) / (y2-y1) */
                dy = y2 - y1;
                dx = x2 - x1;
                intersections[count++] = x1 + (dx * (y - y1)) / dy;
            }
        }

        /* Tri par insertion des X d'intersection.
           Le nombre d'intersections est toujours petit
           (en pratique 2 ou 4), donc le tri insertion
           est plus efficace qu'un tri rapide ici. */
        for (i = 1; i < count; i++)
        {
            tmp = intersections[i];
            j   = i - 1;
            while (j >= 0 && intersections[j] > tmp)
            {
                intersections[j+1] = intersections[j];
                j--;
            }
            intersections[j+1] = tmp;
        }

        /* Remplissage par paires d'intersections.
           Entre x[0] et x[1] : intérieur. Entre x[1] et
           x[2] : extérieur. Entre x[2] et x[3] : intérieur.
           etc. (règle pair-impair). */
        for (i = 0; i + 1 < count; i += 2)
        {
            int xstart = intersections[i];
            int xend   = intersections[i+1];
            int len;

            /* Clipping horizontal. */
            if (xstart < 0)             xstart = 0;
            if (xend   >= SCREEN_WIDTH) xend   = SCREEN_WIDTH - 1;

            len = xend - xstart;
            if (len > 0)
                _fmemset(backbuffer + OFFSET(xstart, y), color, len);
        }
    }
}

/* =========================================================
   CERCLE — Algorithme de Bresenham (mid-point circle)
   =========================================================
   Le cercle possède 8 axes de symétrie. On calcule les
   points d'un seul octant (x de 0 à r/√2) et on en déduit
   les 7 autres par symétrie.

   Variable de décision d :
   d < 0 : on reste sur la même ligne  → d += 4x + 6
   d ≥ 0 : on descend d'une ligne      → d += 4(x-y) + 10, y--
   ========================================================= */

void drawCircle(int xc, int yc, int r, unsigned char color)
{
    int x = 0;          /* point de départ de l'octant     */
    int y = r;          /* on commence en haut du cercle   */
    int d = 3 - 2 * r;  /* valeur initiale de la décision  */

    while (x <= y)
    {
        /* Dessiner les 8 points symétriques avec clipping.
           Chaque paire de if vérifie que le point est
           dans les bornes de l'écran avant de dessiner. */

        if (xc+x>=0 && xc+x<SCREEN_WIDTH) {
            if (yc+y>=0 && yc+y<SCREEN_HEIGHT) putPixel(xc+x, yc+y, color);
            if (yc-y>=0 && yc-y<SCREEN_HEIGHT) putPixel(xc+x, yc-y, color);
        }
        if (xc-x>=0 && xc-x<SCREEN_WIDTH) {
            if (yc+y>=0 && yc+y<SCREEN_HEIGHT) putPixel(xc-x, yc+y, color);
            if (yc-y>=0 && yc-y<SCREEN_HEIGHT) putPixel(xc-x, yc-y, color);
        }
        if (xc+y>=0 && xc+y<SCREEN_WIDTH) {
            if (yc+x>=0 && yc+x<SCREEN_HEIGHT) putPixel(xc+y, yc+x, color);
            if (yc-x>=0 && yc-x<SCREEN_HEIGHT) putPixel(xc+y, yc-x, color);
        }
        if (xc-y>=0 && xc-y<SCREEN_WIDTH) {
            if (yc+x>=0 && yc+x<SCREEN_HEIGHT) putPixel(xc-y, yc+x, color);
            if (yc-x>=0 && yc-x<SCREEN_HEIGHT) putPixel(xc-y, yc-x, color);
        }

        /* Mise à jour de la variable de décision. */
        if (d < 0) d += 4 * x + 6;
        else      { d += 4 * (x - y) + 10; y--; }
        x++;
    }
}

/* =========================================================
   CERCLE PLEIN
   =========================================================
   Même algorithme que drawCircle, mais au lieu de 8 points
   isolés on trace des lignes horizontales entre les points
   symétriques, ce qui remplit le disque.
   xs = x de départ de la ligne, xl = longueur de la ligne.
   ========================================================= */

void drawCircleFill(int xc, int yc, int r, unsigned char color)
{
    int x = 0, y = r;
    int d = 3 - 2 * r;
    int xs, xl;   /* x start et x length de la ligne courante */

    while (x <= y)
    {
        /* Ligne horizontale centrée en (xc, yc-y)
           de xc-x à xc+x (largeur = 2x+1). */
        if (yc-y>=0 && yc-y<SCREEN_HEIGHT) {
            xs = xc - x; xl = 2*x + 1;
            if (xs < 0) { xl += xs; xs = 0; }            /* clip gauche */
            if (xs + xl > SCREEN_WIDTH) xl = SCREEN_WIDTH - xs; /* clip droite */
            if (xl > 0) _fmemset(backbuffer+OFFSET(xs, yc-y), color, xl);
        }

        /* Ligne horizontale centrée en (xc, yc+y). */
        if (yc+y>=0 && yc+y<SCREEN_HEIGHT) {
            xs = xc - x; xl = 2*x + 1;
            if (xs < 0) { xl += xs; xs = 0; }
            if (xs + xl > SCREEN_WIDTH) xl = SCREEN_WIDTH - xs;
            if (xl > 0) _fmemset(backbuffer+OFFSET(xs, yc+y), color, xl);
        }

        /* Ligne horizontale centrée en (xc, yc-x)
           de xc-y à xc+y (largeur = 2y+1). */
        if (yc-x>=0 && yc-x<SCREEN_HEIGHT) {
            xs = xc - y; xl = 2*y + 1;
            if (xs < 0) { xl += xs; xs = 0; }
            if (xs + xl > SCREEN_WIDTH) xl = SCREEN_WIDTH - xs;
            if (xl > 0) _fmemset(backbuffer+OFFSET(xs, yc-x), color, xl);
        }

        /* Ligne horizontale centrée en (xc, yc+x). */
        if (yc+x>=0 && yc+x<SCREEN_HEIGHT) {
            xs = xc - y; xl = 2*y + 1;
            if (xs < 0) { xl += xs; xs = 0; }
            if (xs + xl > SCREEN_WIDTH) xl = SCREEN_WIDTH - xs;
            if (xl > 0) _fmemset(backbuffer+OFFSET(xs, yc+x), color, xl);
        }

        /* Mise à jour de la variable de décision. */
        if (d < 0) d += 4 * x + 6;
        else      { d += 4 * (x - y) + 10; y--; }
        x++;
    }
}
