/* =========================================================
   IMAGE.C — Chargement d'images RAW/PAL en mode 13h
   =========================================================
   Environnement : Open Watcom 1.9, FreeDOS 1.4
   Mode video    : 13h (320x200, 256 couleurs, 1 octet/pixel)

   Organisation des fonctions :

     readPalette        (interne) lecture brute du .pal
     loadImagePal       palette seule -> DAC

     loadImageRaw       .raw entier -> backbuffer, opaque
     loadImageRawKey    .raw entier -> backbuffer, colorKey

     loadScreenRaw      raccourci 320x200 en (0,0)

     loadImageZoneRaw   zone d'un .raw -> backbuffer, opaque
     loadImageZoneRawKey zone d'un .raw -> backbuffer, colorKey

     drawImage          palette + .raw -> backbuffer
     drawScreen         palette + .raw 320x200 -> backbuffer
   ========================================================= */

#include <stdio.h>    /* FILE, fopen, fread, fseek, fclose  */
#include <string.h>   /* _fmemcpy                           */
#include "video.h"    /* backbuffer, SCREEN_WIDTH,
                         SCREEN_HEIGHT, OFFSET             */
#include "palette.h"  /* workingPalette, setPalette, Color  */
#include "image.h"


/* =========================================================
   readPalette — lecture interne du fichier .pal
   =========================================================
   Lit 768 octets depuis f et remplit workingPalette, puis
   envoie immediatement la palette au DAC VGA.

   Format .pal : 256 entrees x 3 octets (R, G, B).
   Chaque composante est sur 6 bits (0-63), format natif
   du DAC VGA (pas de conversion necessaire).

   Pourquoi un buffer intermediaire ?
   La structure Color peut contenir du padding selon
   l'alignement choisi par le compilateur. On lit dans un
   tableau d'octets contigu garantissant l'absence de trous,
   puis on affecte champ par champ dans la structure.

   Retourne 1 si succes, 0 si lecture incomplete. */
static int readPalette(FILE *f)
{
    unsigned char buf[768];
    int i;

    if (fread(buf, 1, 768, f) != 768)
        return 0;

    for (i = 0; i < 256; i++)
    {
        workingPalette[i].r = buf[i * 3];
        workingPalette[i].g = buf[i * 3 + 1];
        workingPalette[i].b = buf[i * 3 + 2];
    }

    setPalette(workingPalette);
    return 1;
}


/* =========================================================
   loadImagePal — charge la palette seule
   =========================================================
   Ouvre le .pal, appelle readPalette() qui remplit
   workingPalette et envoie la palette au DAC VGA.
   Ne touche pas au backbuffer.

   Retourne IMG_OK ou IMG_ERR_PAL / IMG_ERR_READ. */
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
   loadImageRaw — .raw entier -> backbuffer, opaque
   =========================================================
   Lit le .raw ligne par ligne (srcW octets par ligne) et
   copie chaque ligne dans le backbuffer a (dstX, dstY+row)
   via _fmemcpy.

   Stride backbuffer : toujours SCREEN_WIDTH (320), pas srcW.
   Seules les srcW colonnes et srcH lignes cibles sont
   ecrites ; le reste du backbuffer est laisse intact.

   buf[320] sur la pile : suffisant car srcW <= 320.

   Contraintes : dstX + srcW <= 320, dstY + srcH <= 200.
   Retourne IMG_OK ou IMG_ERR_RAW / IMG_ERR_READ. */
int loadImageRaw(const char *rawFile,
                 int srcW, int srcH,
                 int dstX, int dstY)
{
    FILE *f;
    unsigned char far *dst;
    unsigned char buf[320];
    int row;

    f = fopen(rawFile, "rb");
    if (!f) return IMG_ERR_RAW;

    dst = backbuffer + OFFSET(dstX, dstY);

    for (row = 0; row < srcH; row++)
    {
        if (fread(buf, 1, (size_t)srcW, f) != (size_t)srcW)
            { fclose(f); return IMG_ERR_READ; }

        _fmemcpy(dst, buf, (size_t)srcW);

        dst += SCREEN_WIDTH;   /* ligne suivante dans le backbuffer */
    }

    fclose(f);
    return IMG_OK;
}


/* =========================================================
   loadImageRawKey — .raw entier -> backbuffer avec colorKey
   =========================================================
   Identique a loadImageRaw mais ecrit les pixels un par un,
   ce qui permet de sauter ceux dont l'index vaut colorKey.

   colorKey <  0 : bascule en _fmemcpy (opaque), equivalent
                   exact a loadImageRaw.
   colorKey >= 0 : pixels d'index colorKey non ecrits
                   (le backbuffer sous-jacent est conserve).

   Retourne IMG_OK ou IMG_ERR_RAW / IMG_ERR_READ. */
int loadImageRawKey(const char *rawFile,
                    int srcW, int srcH,
                    int dstX, int dstY,
                    int colorKey)
{
    FILE *f;
    unsigned char far *dst;
    unsigned char buf[320];
    unsigned char ck;
    int row, col;

    f = fopen(rawFile, "rb");
    if (!f) return IMG_ERR_RAW;

    dst = backbuffer + OFFSET(dstX, dstY);
    ck  = (unsigned char)colorKey;   /* cast une seule fois */

    for (row = 0; row < srcH; row++)
    {
        if (fread(buf, 1, (size_t)srcW, f) != (size_t)srcW)
            { fclose(f); return IMG_ERR_READ; }

        if (colorKey < 0)
        {
            _fmemcpy(dst, buf, (size_t)srcW);   /* opaque */
        }
        else
        {
            for (col = 0; col < srcW; col++)
                if (buf[col] != ck)
                    dst[col] = buf[col];         /* transparent */
        }

        dst += SCREEN_WIDTH;
    }

    fclose(f);
    return IMG_OK;
}


/* =========================================================
   loadScreenRaw — raccourci .raw 320x200 en (0,0)
   =========================================================
   Cas le plus courant : image plein ecran chargee depuis
   le coin superieur gauche.
   Equivalent a : loadImageRaw(rawFile, 320, 200, 0, 0).
   Retourne IMG_OK ou IMG_ERR_RAW / IMG_ERR_READ. */
int loadScreenRaw(const char *rawFile)
{
    return loadImageRaw(rawFile, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
}


/* =========================================================
   loadImageZoneRaw — zone d'un .raw -> backbuffer, opaque
   =========================================================
   Extrait le rectangle (srcX, srcY, zoneW, zoneH) depuis
   un .raw de largeur imgW et le copie dans le backbuffer
   en (dstX, dstY).

   Principe de lecture :
     1. fseek au premier pixel de la zone :
          offset = srcY * imgW + srcX
     2. Pour chaque ligne de la zone :
          a. lire zoneW octets dans buf
          b. _fmemcpy vers le backbuffer
          c. fseek de (imgW - zoneW) pour sauter les
             colonnes hors-zone et atterrir au debut
             de la ligne suivante dans le .raw
          d. avancer de SCREEN_WIDTH dans le backbuffer

   Contraintes :
     srcX + zoneW <= imgW
     dstX + zoneW <= 320
     dstY + zoneH <= 200

   Retourne IMG_OK ou IMG_ERR_RAW / IMG_ERR_READ. */
int loadImageZoneRaw(const char *rawFile,
                     int imgW,
                     int srcX, int srcY,
                     int zoneW, int zoneH,
                     int dstX, int dstY)
{
    FILE *f;
    unsigned char far *dst;
    unsigned char buf[320];
    long offset;
    int row;

    f = fopen(rawFile, "rb");
    if (!f) return IMG_ERR_RAW;

    offset = (long)srcY * imgW + srcX;
    if (fseek(f, offset, SEEK_SET) != 0)
        { fclose(f); return IMG_ERR_READ; }

    dst = backbuffer + OFFSET(dstX, dstY);

    for (row = 0; row < zoneH; row++)
    {
        if (fread(buf, 1, (size_t)zoneW, f) != (size_t)zoneW)
            { fclose(f); return IMG_ERR_READ; }

        _fmemcpy(dst, buf, (size_t)zoneW);

        dst += SCREEN_WIDTH;

        /* Sauter les colonnes hors-zone pour la ligne suivante.
           Le fseek final (apres la derniere ligne) est omis :
           le fichier est ferme immediatement apres. */
        if (row < zoneH - 1)
        {
            if (fseek(f, (long)(imgW - zoneW), SEEK_CUR) != 0)
                { fclose(f); return IMG_ERR_READ; }
        }
    }

    fclose(f);
    return IMG_OK;
}


/* =========================================================
   loadImageZoneRawKey — zone d'un .raw -> backbuffer, colorKey
   =========================================================
   Identique a loadImageZoneRaw mais avec gestion de la
   transparence : les pixels sont ecrits un par un, ce qui
   permet de sauter ceux dont l'index vaut colorKey.

   colorKey <  0 : bascule en _fmemcpy (opaque), equivalent
                   exact a loadImageZoneRaw.
   colorKey >= 0 : pixels d'index colorKey non ecrits.

   Cas d'usage typique : extraire un glyphe d'une feuille
   de sprites et l'afficher sur un fond existant sans
   ecraser les pixels de fond du glyphe.

   Retourne IMG_OK ou IMG_ERR_RAW / IMG_ERR_READ. */
int loadImageZoneRawKey(const char *rawFile,
                        int imgW,
                        int srcX, int srcY,
                        int zoneW, int zoneH,
                        int dstX, int dstY,
                        int colorKey)
{
    FILE *f;
    unsigned char far *dst;
    unsigned char buf[320];
    unsigned char ck;
    long offset;
    int row, col;

    f = fopen(rawFile, "rb");
    if (!f) return IMG_ERR_RAW;

    offset = (long)srcY * imgW + srcX;
    if (fseek(f, offset, SEEK_SET) != 0)
        { fclose(f); return IMG_ERR_READ; }

    dst = backbuffer + OFFSET(dstX, dstY);
    ck  = (unsigned char)colorKey;

    for (row = 0; row < zoneH; row++)
    {
        if (fread(buf, 1, (size_t)zoneW, f) != (size_t)zoneW)
            { fclose(f); return IMG_ERR_READ; }

        if (colorKey < 0)
        {
            _fmemcpy(dst, buf, (size_t)zoneW);   /* opaque */
        }
        else
        {
            for (col = 0; col < zoneW; col++)
                if (buf[col] != ck)
                    dst[col] = buf[col];          /* transparent */
        }

        dst += SCREEN_WIDTH;

        if (row < zoneH - 1)
        {
            if (fseek(f, (long)(imgW - zoneW), SEEK_CUR) != 0)
                { fclose(f); return IMG_ERR_READ; }
        }
    }

    fclose(f);
    return IMG_OK;
}


/* =========================================================
   drawImage — palette + .raw -> backbuffer en (dstX, dstY)
   =========================================================
   Combine loadImagePal et loadImageRaw en un seul appel.
   Utile quand une image a sa propre palette et qu'on veut
   l'afficher a une position precise.
   Retourne IMG_OK ou un code IMG_ERR_*. */
int drawImage(const char *palFile, const char *rawFile,
              int srcW, int srcH,
              int dstX, int dstY)
{
    int r;

    r = loadImagePal(palFile);
    if (r != IMG_OK) return r;

    return loadImageRaw(rawFile, srcW, srcH, dstX, dstY);
}


/* =========================================================
   drawScreen — palette + .raw 320x200 -> backbuffer en (0,0)
   =========================================================
   Combine loadImagePal et loadScreenRaw en un seul appel.
   Raccourci pour afficher une image plein ecran avec sa
   palette en une ligne de code.
   Retourne IMG_OK ou un code IMG_ERR_*. */
int drawScreen(const char *palFile, const char *rawFile)
{
    int r;

    r = loadImagePal(palFile);
    if (r != IMG_OK) return r;

    return loadScreenRaw(rawFile);
}
