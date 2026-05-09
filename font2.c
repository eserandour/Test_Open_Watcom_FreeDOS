/* =========================================================
   FONT2.C — Affichage de texte par feuille de sprites
   =========================================================
   Environnement : Open Watcom 1.9, FreeDOS 1.4
   Mode video    : 13h (320x200, 256 couleurs, 1 octet/pixel)

   Delegue l'extraction de chaque glyphe a
   loadImageZoneRawKey() (image.c), qui gere a la fois la
   lecture de la zone dans le .raw et la transparence par
   cle de couleur.

   Voir font2.h pour le detail du format de la feuille,
   le calcul des positions et la convention colorKey.
   ========================================================= */

#include "video.h"   /* SCREEN_WIDTH                        */
#include "image.h"   /* loadImageZoneRawKey                 */
#include "font2.h"

/* Chemin du fichier .raw de la feuille de sprites.
   Relatif au repertoire de lancement du .exe. */
#define FONT2_RAW "images\\font.raw"


/* =========================================================
   drawText2
   =========================================================
   Parcourt la chaine text caractere par caractere.
   Pour chaque caractere dans la plage supportee :
     1. Calcule sa position (srcX, srcY) dans la feuille.
     2. Appelle loadImageZoneRawKey() pour extraire le glyphe
        32x32 et le copier dans le backbuffer en
        (x + i * FONT2_CHAR_W, y).

   Les caracteres hors plage sont ignores mais comptent
   dans l'espacement (le curseur avance quand meme). */
void drawText2(const char *text, int x, int y, int colorKey)
{
    int i, idx, col, row, srcX, srcY;

    for (i = 0; text[i] != '\0'; i++)
    {
        unsigned char c = (unsigned char)text[i];

        if (c < FONT2_FIRST_CHAR || c > FONT2_LAST_CHAR)
            continue;

        /* Position du glyphe dans la grille */
        idx  = c - FONT2_FIRST_CHAR;
        col  = idx % FONT2_COLS;
        row  = idx / FONT2_COLS;
        srcX = col * FONT2_CHAR_W;
        srcY = row * FONT2_CHAR_H;

        loadImageZoneRawKey(FONT2_RAW, FONT2_SHEET_W,
                            srcX, srcY,
                            FONT2_CHAR_W, FONT2_CHAR_H,
                            x + i * FONT2_CHAR_W, y,
                            colorKey);
    }
}


/* =========================================================
   drawTextCentered2
   =========================================================
   Calcule la largeur totale du texte en pixels :
     largeur = len * FONT2_CHAR_W
   Puis derive le x de depart pour centrer sur SCREEN_WIDTH :
     x = (SCREEN_WIDTH - largeur) / 2
   Si la chaine depasse l'ecran (x < 0), on la cale a gauche.
   Appelle ensuite drawText2 avec ce x calcule. */
void drawTextCentered2(const char *text, int y, int colorKey)
{
    int len = 0;
    int x;

    while (text[len] != '\0') len++;

    x = (SCREEN_WIDTH - len * FONT2_CHAR_W) / 2;
    if (x < 0) x = 0;

    drawText2(text, x, y, colorKey);
}
