/* =========================================================
   SCENE4.C — Scène : affichage d'une image RAW+PAL
   =========================================================
   Affiche images/image.raw avec sa palette images/image.pal
   pendant 10 secondes, avec fondu en entree et en sortie.

   Machine a etats :
   STATE_FADEIN  : fade noir -> image en FADE_MS ms
   STATE_DISPLAY : image pleine luminosite pendant DISPLAY_MS
   STATE_FADEOUT : fade image -> noir en FADE_MS ms
   Puis shutdown() + exit().
   ========================================================= */

#include <stdlib.h>   /* exit                              */
#include "timer.h"    /* readTimer, elapsedTimeMs          */
#include "video.h"    /* flip                              */
#include "palette.h"  /* workingPalette, fadePalette,
                         generateBlackPalette, copyPalette,
                         setPalette, paletteA              */
#include "image.h"    /* loadImage, IMG_OK                 */
#include "scene.h"    /* sceneStart                        */

/* Prototype de shutdown() defini dans main.c. */
void shutdown(void);

/* ---------------------------------------------------------
   Durees
   --------------------------------------------------------- */

#define FADE_MS     1000UL   /* duree du fondu en/sortie (ms) */
#define DISPLAY_MS  8000UL   /* duree d'affichage plein (ms)  */

/* ---------------------------------------------------------
   Etats internes
   --------------------------------------------------------- */

typedef enum {
    STATE_FADEIN,    /* fondu entrant : noir -> image      */
    STATE_DISPLAY,   /* image a pleine luminosite          */
    STATE_FADEOUT    /* fondu sortant : image -> noir      */
} State;

/* =========================================================
   SCENE PRINCIPALE
   ========================================================= */

void scene4(void)
{
    static int           initialized = 0;
    static State         state       = STATE_FADEIN;
    static unsigned long stateStart  = 0;

    unsigned long now     = readTimer();
    unsigned long elapsed;
    float         t;

    /* -------------------------------------------------------
       Initialisation : chargement de l'image
       -------------------------------------------------------
       On charge les pixels dans le backbuffer et la palette
       dans paletteA (via workingPalette puis copie), mais on
       envoie au DAC une palette entierement noire pour que
       l'image soit invisible au moment du flip(). Le fade-in
       revelera ensuite progressivement les vraies couleurs.
       ------------------------------------------------------- */
    if (!initialized)
    {
        int err = loadImage("images\\freedos.pal", "images\\freedos.raw");
        if (err != IMG_OK) { shutdown(); exit(1); }

        /* Sauvegarder la palette de l'image dans paletteA
           avant de noircir le DAC. loadImage() a rempli
           workingPalette avec les couleurs du fichier .pal
           et les a envoyees au DAC — on copie maintenant
           cette palette avant de l'ecraser. */
        copyPalette(paletteA, workingPalette);

        /* Mettre le DAC entierement a zero (tout noir) pour
           que l'image soit invisible quand flip() copie le
           backbuffer vers la VRAM. Le fondu partira de la. */
        generateBlackPalette(workingPalette);
        setPalette(workingPalette);

        /* Afficher le backbuffer (image chargee, palette noire
           => ecran noir. Les pixels sont en VRAM mais
           invisibles tant que la palette est noire.) */
        flip();

        stateStart  = readTimer();
        state       = STATE_FADEIN;
        initialized = 1;
    }

    elapsed = elapsedTimeMs(stateStart, now);

    switch (state)
    {
        /* ---------------------------------------------------
           Fondu entrant : luminosite 0 -> 1 sur FADE_MS ms
           ---------------------------------------------------
           fadePalette(paletteA, t) envoie paletteA au DAC
           avec chaque composante multipliee par t, sans
           modifier paletteA en RAM.
           t=0.0 : DAC tout noir (debut du fondu)
           t=1.0 : DAC = paletteA (image a plein eclat)
           --------------------------------------------------- */
        case STATE_FADEIN:
            t = (float)elapsed / (float)FADE_MS;
            if (t > 1.0f) t = 1.0f;
            fadePalette(paletteA, t);
            if (elapsed >= FADE_MS)
            {
                setPalette(paletteA);   /* fixer la palette exacte */
                stateStart = readTimer();
                state      = STATE_DISPLAY;
            }
            break;

        /* ---------------------------------------------------
           Affichage plein : image stable pendant DISPLAY_MS
           --------------------------------------------------- */
        case STATE_DISPLAY:
            if (elapsed >= DISPLAY_MS)
            {
                stateStart = readTimer();
                state      = STATE_FADEOUT;
            }
            break;

        /* ---------------------------------------------------
           Fondu sortant : luminosite 1 -> 0 sur FADE_MS ms
           ---------------------------------------------------
           Meme principe que le fade-in mais t decroit de 1
           vers 0 : l'image s'assombrit progressivement.
           --------------------------------------------------- */
        case STATE_FADEOUT:
            t = 1.0f - (float)elapsed / (float)FADE_MS;
            if (t < 0.0f) t = 0.0f;
            fadePalette(paletteA, t);
            if (elapsed >= FADE_MS)
            {
                initialized = 0;
                shutdown();
                exit(0);
            }
            break;
    }
}
