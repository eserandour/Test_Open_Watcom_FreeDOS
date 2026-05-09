#ifndef FONT2_H
#define FONT2_H

/* =========================================================
   FONT2.H — Affichage de texte par feuille de sprites
   =========================================================
   Environnement : Open Watcom 1.9, FreeDOS 1.4
   Mode video    : 13h (320x200, 256 couleurs, 1 octet/pixel)

   PRINCIPE
   --------
   Les glyphes sont stockes dans une feuille de sprites .raw
   (images\font.raw), organisee en grille reguliere :

     Feuille : 320 x 192 pixels
     Grille  : 6 lignes x 10 colonnes
     Glyphe  : 32 x 32 pixels
     Plage   : 0x20 (espace) a 0x5B ([)

   Pour afficher le caractere c, on calcule sa position dans
   la grille :
     idx  = c - FONT2_FIRST_CHAR   (0 = espace, 1 = '!', ...)
     col  = idx % FONT2_COLS        (colonne 0..9)
     row  = idx / FONT2_COLS        (ligne   0..5)
     srcX = col * FONT2_CHAR_W      (pixel X dans le .raw)
     srcY = row * FONT2_CHAR_H      (pixel Y dans le .raw)

   Puis on extrait cette zone via loadImageZoneRawKey().

   TRANSPARENCE
   ------------
   colorKey <  0 : copie opaque, le fond du glyphe ecrase
                   le backbuffer.
   colorKey >= 0 : les pixels d'index colorKey ne sont pas
                   ecrits. Permet de superposer le texte sur
                   un fond existant.
   FONT2_BG (127) est l'index de fond de la feuille fournie.

   NOMMAGE
   -------
   Les fonctions sont suffixees "2" pour eviter le conflit
   de symboles avec drawText / drawTextCentered de font.c
   qui ont des signatures differentes.

   UTILISATION
   -----------
     loadImagePal("images\\font.pal");
     clearScreen(0);
     drawText2("HELLO", 80, 84, FONT2_BG);
     drawTextCentered2("WORLD", 120, FONT2_BG);
     flip();
   ========================================================= */


/* ---------------------------------------------------------
   Dimensions de la feuille et des glyphes
   --------------------------------------------------------- */

#define FONT2_SHEET_W    320   /* largeur totale du .raw (px) */
#define FONT2_SHEET_H    192   /* hauteur totale du .raw (px) */
#define FONT2_CHAR_W      32   /* largeur d'un glyphe (px)    */
#define FONT2_CHAR_H      32   /* hauteur d'un glyphe (px)    */
#define FONT2_COLS        10   /* colonnes dans la grille     */

/* ---------------------------------------------------------
   Plage de caracteres supportes
   --------------------------------------------------------- */

#define FONT2_FIRST_CHAR 0x20  /* espace — premier glyphe     */
#define FONT2_LAST_CHAR  0x5B  /* [     — dernier glyphe      */

/* ---------------------------------------------------------
   Index de fond des glyphes dans la palette font.pal
   --------------------------------------------------------- */

#define FONT2_BG         127   /* a passer comme colorKey pour
                                  la transparence du fond     */


/* ---------------------------------------------------------
   Fonctions
   --------------------------------------------------------- */

/* Dessine text a partir du pixel (x, y) dans le backbuffer.
   Les caracteres hors plage [FONT2_FIRST_CHAR, FONT2_LAST_CHAR]
   sont ignores (la position avance quand meme d'un glyphe).
   colorKey <  0 : copie opaque.
   colorKey >= 0 : pixels de cet index non ecrits. */
void drawText2(const char *text, int x, int y, int colorKey);

/* Dessine text centre horizontalement sur SCREEN_WIDTH (320)
   a la hauteur y dans le backbuffer.
   Si la chaine depasse 320 px, elle est callee a gauche (x=0).
   colorKey : meme convention que drawText2. */
void drawTextCentered2(const char *text, int y, int colorKey);

#endif /* FONT2_H */
