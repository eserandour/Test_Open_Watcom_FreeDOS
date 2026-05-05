#ifndef IMAGE_H
#define IMAGE_H

/* =========================================================
   IMAGE.H — Chargement et affichage d'images en mode 13h
   =========================================================
   Format RAW + PAL (deux fichiers séparés) :

   .raw — 64000 octets, un octet par pixel.
          Chaque octet est un index (0-255) dans la palette
          VGA. Les pixels sont stockés ligne par ligne, de
          gauche à droite, haut vers bas — exactement comme
          la VRAM en mode 13h. Aucun décodage nécessaire :
          on copie directement dans le backbuffer.

   .pal — 768 octets, 256 entrées × 3 composantes R, G, B.
          Chaque composante est sur 6 bits (valeur 0-63),
          format natif du DAC VGA. Pas de conversion.

   Flux typique d'utilisation :
     loadImage("img.pal", "img.raw");   -> palette + pixels
     flip();                            -> visible a l'ecran

   Pour composer plusieurs images sans relire le disque :
     unsigned char far *buf = _fmalloc(BACKBUFFER_SIZE);
     loadImageToBuffer("img.raw", buf); -> pixels en far heap
     drawImageRegion(0, 0, 320, 100, buf); -> moitie haute
   ========================================================= */

/* ---------------------------------------------------------
   Codes de retour
   --------------------------------------------------------- */

#define IMG_OK          0   /* succes                        */
#define IMG_ERR_PAL     1   /* impossible d'ouvrir .pal      */
#define IMG_ERR_RAW     2   /* impossible d'ouvrir .raw      */
#define IMG_ERR_READ    3   /* lecture incomplete            */

/* ---------------------------------------------------------
   Chargement — fonctions de base
   --------------------------------------------------------- */

/* Charge la palette (.pal) dans workingPalette + DAC VGA,
   puis charge les pixels (.raw) dans le backbuffer.
   C'est la fonction principale pour afficher une image.
   Retourne IMG_OK ou un code IMG_ERR_*. */
int loadImage(const char *palFile, const char *rawFile);

/* Charge uniquement les pixels (.raw) dans le backbuffer,
   sans modifier la palette courante.
   Utile quand plusieurs images partagent la meme palette,
   ou pour superposer une image sur un fond existant.
   Retourne IMG_OK ou IMG_ERR_RAW / IMG_ERR_READ. */
int loadImageRaw(const char *rawFile);

/* Charge uniquement la palette (.pal) dans workingPalette
   et l'envoie au DAC, sans toucher au backbuffer.
   Utile pour changer de palette sans recharger l'image,
   par exemple pour un effet de palette swap.
   Retourne IMG_OK ou IMG_ERR_PAL / IMG_ERR_READ. */
int loadImagePal(const char *palFile);

/* ---------------------------------------------------------
   Chargement en far heap
   --------------------------------------------------------- */

/* Charge les pixels (.raw) dans un buffer far fourni par
   l'appelant, sans toucher au backbuffer ni a la palette.

   L'appelant est responsable d'allouer buf avec _fmalloc :
     unsigned char far *buf = _fmalloc(BACKBUFFER_SIZE);

   Interet : charger plusieurs images en far heap au
   demarrage de la scene, puis les afficher sans acces
   disque via drawImageRegion(). Les transferts RAM->RAM
   sont bien plus rapides que les lectures disque.

   Retourne IMG_OK ou IMG_ERR_RAW / IMG_ERR_READ. */
int loadImageToBuffer(const char *rawFile,
                      unsigned char far *buf);

/* ---------------------------------------------------------
   Affichage partiel
   --------------------------------------------------------- */

/* Copie une region rectangulaire depuis un buffer source
   (far) vers le backbuffer, a la position (dstX, dstY).

   Parametres :
     dstX, dstY : coin superieur gauche dans le backbuffer
     w, h       : largeur et hauteur de la region en pixels
     src        : buffer source 320x200 (far heap ou autre)

   Le buffer source est toujours interprete comme une image
   320x200 complete : src + y*320 + x donne le pixel (x,y).
   Seule la region (0,0)-(w-1,h-1) est copiee vers (dstX,dstY).

   Pas de clipping : l'appelant doit s'assurer que la region
   destination reste dans les bornes 320x200 du backbuffer.

   Utile pour :
   - Afficher une sous-region d'une image (sprite, vignette)
   - Composer plusieurs images dans le backbuffer
   - Reveler une image progressivement (wipe, bande par bande) */
void drawImageRegion(int dstX, int dstY, int w, int h,
                     unsigned char far *src);

#endif /* IMAGE_H */
