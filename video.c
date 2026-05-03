/* =========================================================
   VIDEO.C — Mode 13h, backbuffer, curseur
   ========================================================= */

#include <malloc.h>   /* _fmalloc, _ffree, _fmemset */
#include <string.h>   /* movedata                   */
#include <dos.h>      /* FP_SEG, FP_OFF             */
#include <conio.h>    /* inp (lecture port 0x3DA)   */
#include "video.h"

/* ---------------------------------------------------------
   Backbuffer global
   --------------------------------------------------------- */

/* Définition du pointeur backbuffer (déclaré extern dans
   video.h, défini ici une seule fois dans tout le projet).
   Initialisé à NULL : indique que le buffer n'est pas
   encore alloué. */
unsigned char far *backbuffer = NULL;

/* =========================================================
   BACKBUFFER
   ========================================================= */

/* Alloue le backbuffer en mémoire far (heap étendu).
   _fmalloc alloue au-delà du segment de données courant,
   ce qui est nécessaire pour 64000 octets en modèle large.
   Retourne 1 si succès, 0 si échec. */
int initBackbuffer(void)
{
    backbuffer = (unsigned char far *)_fmalloc(BACKBUFFER_SIZE);
    if (!backbuffer)
        return 0;   /* allocation échouée : mémoire insuffisante */

    /* Initialiser tous les pixels à 0 (noir) pour éviter
       d'afficher des données aléatoires de la mémoire. */
    _fmemset(backbuffer, 0, BACKBUFFER_SIZE);
    return 1;
}

/* Libère le backbuffer et invalide le pointeur.
   Vérification préalable : évite un double free si
   freeBackbuffer() est appelé deux fois par erreur. */
void freeBackbuffer(void)
{
    if (backbuffer)
    {
        _ffree(backbuffer);
        backbuffer = NULL;   /* évite un pointeur invalide */
    }
}

/* Copie le contenu du backbuffer vers la VRAM VGA.
   movedata(srcSeg, srcOff, dstSeg, dstOff, count) copie
   count octets d'un segment:offset source vers un
   segment:offset destination, en gérant correctement les
   pointeurs far 16 bits.
   FP_SEG et FP_OFF extraient le segment et l'offset d'un
   pointeur far. */
void flip(void)
{
    movedata(
        FP_SEG(backbuffer), FP_OFF(backbuffer),  /* source : backbuffer en RAM  */
        VGA_SEG,            VGA_OFF,             /* destination : VRAM 0xA0000 */
        BACKBUFFER_SIZE                          /* 64000 octets               */
    );
}

/* =========================================================
   MODE VIDÉO ET CURSEUR
   ========================================================= */

/* Change le mode vidéo via l'interruption BIOS 10h.
   AH = 00h : fonction "Set Video Mode"
   AL = mode : 0x13 pour 320x200x256, 0x03 pour texte 80x25 */
void setVideoMode(unsigned char mode)
{
    _asm {
        mov ah, 00h   /* fonction BIOS : changement de mode */
        mov al, mode  /* numéro du mode vidéo               */
        int 10h       /* appel BIOS vidéo                   */
    }
}

/* Cache le curseur texte via l'interruption BIOS 10h.
   AH = 01h : fonction "Set Cursor Shape"
   CX = 1400h : bits 5-0 de CH = 0x14 (curseur invisible).
   Quand le bit 5 de CH est à 1, le curseur est masqué. */
void cursorOff(void)
{
    _asm {
        mov ah, 01h     /* fonction BIOS : forme du curseur  */
        mov cx, 1400h   /* 0x14 en CH = curseur invisible    */
        int 10h
    }
}

/* Restaure le curseur texte standard via BIOS 10h.
   CX = 0607h : CH=06 (ligne de début), CL=07 (ligne de fin)
   correspond à un curseur underline standard en mode texte. */
void cursorOn(void)
{
    _asm {
        mov ah, 01h     /* fonction BIOS : forme du curseur  */
        mov cx, 0607h   /* curseur underline standard        */
        int 10h
    }
}

/* =========================================================
   SYNCHRONISATION VERTICALE
   ========================================================= */

/* Attend le prochain retrace vertical du moniteur.
   Le registre d'état VGA (port 0x3DA) indique :
     bit 3 = 1 : retrace vertical en cours
     bit 3 = 0 : affichage actif (pas de retrace)

   Algorithme en deux temps :
   1. Attendre la FIN du retrace en cours (si on est
      déjà dedans, évite de rater le prochain).
   2. Attendre le DÉBUT du prochain retrace.

   On peut alors faire flip() ou changer la palette en
   toute sécurité sans provoquer de tearing visible. */
void waitVRetrace(void)
{
    while  (inp(0x3DA) & 0x08);    /* attendre fin du retrace en cours  */
    while (!(inp(0x3DA) & 0x08));  /* attendre début du prochain retrace */
}
