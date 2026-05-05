/* =========================================================
   IMAGE.C — Chargement et affichage d'images en mode 13h
   =========================================================
   Environnement : Open Watcom 1.9, FreeDOS 1.4
   Mode video    : 13h (320x200, 256 couleurs)

   Dependances :
   - video.h   : backbuffer (far *), BACKBUFFER_SIZE,
                 SCREEN_WIDTH, OFFSET(x,y)
   - palette.h : workingPalette, setPalette, Color
   ========================================================= */

#include <stdio.h>    /* FILE, fopen, fread, fclose         */
#include <string.h>   /* _fmemcpy                           */
#include "video.h"    /* backbuffer, BACKBUFFER_SIZE,
                         SCREEN_WIDTH, OFFSET               */
#include "palette.h"  /* workingPalette, setPalette, Color  */
#include "image.h"

/* =========================================================
   FONCTION INTERNE : readPalette
   =========================================================
   Lit 768 octets depuis f (256 entrees × 3 composantes RGB
   sur 6 bits) et remplit workingPalette, puis envoie la
   palette au DAC VGA.

   Pourquoi un buffer intermediaire ?
   fread() lit des octets bruts. La structure Color contient
   trois unsigned char (r, g, b) mais le compilateur peut
   inserer du padding entre les champs selon l'alignement.
   On lit donc dans un tableau d'octets contigu, puis on
   affecte champ par champ pour etre independant du padding.

   Retourne 1 si succes, 0 si lecture incomplete. */
static int readPalette(FILE *f)
{
    unsigned char buf[768];   /* 256 * 3 octets, contigu    */
    int i;

    if (fread(buf, 1, 768, f) != 768)
        return 0;

    /* Recopier octet par octet dans la structure Color.
       buf[i*3]   = composante rouge  de la couleur i
       buf[i*3+1] = composante verte  de la couleur i
       buf[i*3+2] = composante bleue  de la couleur i
       Toutes sur 6 bits (0-63), format natif du DAC VGA. */
    for (i = 0; i < 256; i++)
    {
        workingPalette[i].r = buf[i * 3];
        workingPalette[i].g = buf[i * 3 + 1];
        workingPalette[i].b = buf[i * 3 + 2];
    }

    /* Envoyer la palette remplie au DAC VGA.
       A partir de cet instant les couleurs changent a l'ecran,
       meme si le backbuffer n'a pas encore ete mis a jour. */
    setPalette(workingPalette);
    return 1;
}

/* =========================================================
   FONCTION INTERNE : readPixelsTo
   =========================================================
   Lit BACKBUFFER_SIZE octets (64000) depuis f vers le
   buffer far pointe par dst.

   Le parametre dst peut etre :
   - backbuffer  : chargement direct pour affichage immediat
   - un buffer far alloue par l'appelant : stockage en heap

   Retourne 1 si succes, 0 si lecture incomplete. */
static int readPixelsTo(FILE *f, unsigned char far *dst)
{
    /* fread() avec un pointeur far fonctionne correctement
       en modele memoire large (-ml) sous Open Watcom :
       le compilateur genere les instructions far adequates
       pour ecrire au-dela du segment de donnees courant. */
    if (fread(dst, 1, BACKBUFFER_SIZE, f) != BACKBUFFER_SIZE)
        return 0;

    return 1;
}

/* =========================================================
   loadImage — palette + raw (deux fichiers)
   =========================================================
   Sequence :
   1. Ouvrir .pal, lire la palette, mettre a jour le DAC.
   2. Ouvrir .raw, lire les 64000 pixels dans le backbuffer.
   L'image est visible apres un appel a flip(). */
int loadImage(const char *palFile, const char *rawFile)
{
    FILE *f;

    f = fopen(palFile, "rb");
    if (!f) return IMG_ERR_PAL;
    if (!readPalette(f)) { fclose(f); return IMG_ERR_READ; }
    fclose(f);

    f = fopen(rawFile, "rb");
    if (!f) return IMG_ERR_RAW;
    if (!readPixelsTo(f, backbuffer)) { fclose(f); return IMG_ERR_READ; }
    fclose(f);

    return IMG_OK;
}

/* =========================================================
   loadImageRaw — pixels seuls dans le backbuffer
   =========================================================
   Charge les pixels sans toucher a la palette.
   Cas d'usage : plusieurs images partagent la meme palette
   (chargee une seule fois avec loadImagePal), on ne charge
   que les pixels pour chaque image suivante. */
int loadImageRaw(const char *rawFile)
{
    FILE *f;

    f = fopen(rawFile, "rb");
    if (!f) return IMG_ERR_RAW;
    if (!readPixelsTo(f, backbuffer)) { fclose(f); return IMG_ERR_READ; }
    fclose(f);

    return IMG_OK;
}

/* =========================================================
   loadImagePal — palette seule
   =========================================================
   Charge et applique une palette sans toucher au backbuffer.
   Cas d'usage : palette swap (changer l'ambiance d'une scene
   sans recharger les pixels), ou pre-charger la palette
   avant de remplir le backbuffer autrement. */
int loadImagePal(const char *palFile)
{
    FILE *f;

    f = fopen(palFile, "rb");
    if (!f) return IMG_ERR_PAL;
    if (!readPalette(f)) { fclose(f); return IMG_ERR_READ; }
    fclose(f);

    return IMG_OK;
}

/* =========================================================
   loadImageToBuffer — pixels dans un buffer far arbitraire
   =========================================================
   Charge les pixels dans buf au lieu du backbuffer.
   buf doit etre alloue par l'appelant avec _fmalloc :

     unsigned char far *img = _fmalloc(BACKBUFFER_SIZE);
     if (!img) { ... erreur memoire ... }
     loadImageToBuffer("scene.raw", img);

   Interet principal : pre-charger plusieurs images en far
   heap au debut de la scene. Les afficher ensuite avec
   drawImageRegion() est un simple transfert RAM->RAM,
   beaucoup plus rapide qu'une lecture disque. */
int loadImageToBuffer(const char *rawFile, unsigned char far *buf)
{
    FILE *f;

    f = fopen(rawFile, "rb");
    if (!f) return IMG_ERR_RAW;
    if (!readPixelsTo(f, buf)) { fclose(f); return IMG_ERR_READ; }
    fclose(f);

    return IMG_OK;
}

/* =========================================================
   drawImageRegion — copie une region d'un buffer vers le
                     backbuffer
   =========================================================
   Copie un rectangle de (w x h) pixels depuis src vers le
   backbuffer a la position (dstX, dstY).

   Fonctionnement ligne par ligne :
   En mode 13h, les pixels sont lineaires en memoire :
   le pixel (x, y) se trouve a l'offset y*320 + x.
   On ne peut donc pas copier toute la region en un seul
   _fmemcpy — les lignes de src et de dst ne sont pas
   contiguës dans la memoire globale (elles sont separees
   par 320 octets chacune).
   On copie donc une ligne a la fois avec _fmemcpy, en
   avancant de 320 octets dans chaque buffer a chaque tour.

   Exemple : copier la moitie gauche d'une image en haut
   a droite de l'ecran :
     drawImageRegion(160, 0, 160, 100, monImage);

   Pas de clipping : s'assurer que dstX+w <= 320
                                  et dstY+h <= 200. */
void drawImageRegion(int dstX, int dstY, int w, int h,
                     unsigned char far *src)
{
    int row;

    /* Pointeur far vers le premier pixel destination
       dans le backbuffer : ligne dstY, colonne dstX.
       OFFSET(x, y) = y*320 + x (defini dans video.h). */
    unsigned char far *dst = backbuffer + OFFSET(dstX, dstY);

    /* Pointeur far vers le premier pixel source dans src.
       On commence toujours en (0, 0) dans le buffer source :
       drawImageRegion copie le coin superieur gauche de src.
       Si vous voulez copier une autre region de src, decalez
       src avant l'appel : src + OFFSET(srcX, srcY). */
    unsigned char far *s = src;

    for (row = 0; row < h; row++)
    {
        /* Copier une ligne de w pixels.
           _fmemcpy est la variante far de memcpy : elle
           accepte des pointeurs far (segment:offset 32 bits)
           et gere correctement les copies inter-segments. */
        _fmemcpy(dst, s, (size_t)w);

        /* Avancer d'une ligne dans chaque buffer.
           Une ligne = SCREEN_WIDTH = 320 octets en mode 13h. */
        dst += SCREEN_WIDTH;
        s   += SCREEN_WIDTH;
    }
}
