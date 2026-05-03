/* =========================================================
   SCRANDOM.C — Scène : pixels aléatoires
   =========================================================
   Durée totale : 5 secondes, puis passage à SCENE_2.

   Affiche des pixels de couleurs aléatoires en remplissant
   le backbuffer avec un générateur congruentiel linéaire
   (LCG). Les pixels sont écrits par paires (mots 16 bits)
   pour aller deux fois plus vite qu'octet par octet.

   Le backbuffer est rafraîchi toutes les 100 ms pour
   que le bruit change à une vitesse perceptible mais pas
   trop rapide (environ 10 images par seconde).
   ========================================================= */

#include <time.h>     /* time (initialisation LCG)         */
#include "timer.h"    /* readTimer, elapsedTimeMs, TARGET_HZ */
#include "video.h"    /* backbuffer, flip                  */
#include "palette.h"  /* workingPalette, defaultPalette,
                         copyPalette, setPalette            */
#include "scene.h"    /* sceneStart, setScene, SCENE_2 */

void scene1(void)
{
    /* Variables statiques : persistent entre les appels.
       Remise à zéro via initialized = 0 lors du setScene(). */
    static unsigned long lastRender  = 0;   /* timestamp du dernier flip    */
    static int           initialized = 0;   /* flag d'initialisation        */
    static unsigned long lcg_state   = 0;   /* état courant du générateur   */

    /* Intervalle de rafraîchissement de l'image (ms). */
    const unsigned long render_interval_ms = 100UL;

    /* Durée totale de la scène (ms). */
    const unsigned long scene_ms = 5000UL;

    unsigned long now = readTimer();

    /* Pointeur mot (16 bits) vers le backbuffer.
       Écrire 2 octets à la fois (un mot) est plus rapide
       qu'écrire octet par octet sur 8086. */
    unsigned int far *dst;
    unsigned long i;
    unsigned int pixel;  /* deux pixels packed dans un mot */

    /* -------------------------------------------------------
       Initialisation (une seule fois par activation de scène)
       ------------------------------------------------------- */
    if (!initialized)
    {
        lastRender = now;

        /* Initialiser le LCG avec l'heure système comme
           graine pour avoir un bruit différent à chaque fois.
           Sans ça le bruit serait identique à chaque lancement
           (la graine srand() du main ne suffit pas ici car
           on veut une graine 32 bits pour le LCG). */
        lcg_state = (unsigned long)time(NULL);

        /* Utiliser la palette par défaut du BIOS.
           Les 256 entrées donnent toutes les couleurs
           possibles du mode 13h. */
        copyPalette(workingPalette, defaultPalette);
        setPalette(workingPalette);

        initialized = 1;
    }

    /* -------------------------------------------------------
       Mise à jour de l'image (toutes les 100 ms)
       ------------------------------------------------------- */

    /* Boucle de rattrapage : si plusieurs intervalles se
       sont écoulés depuis le dernier rendu (ex: après un
       ralentissement), on remet à jour plusieurs fois pour
       rester synchronisé avec le temps réel. */
    while (elapsedTimeMs(lastRender, now) >= render_interval_ms)
    {
        /* Remplir le backbuffer avec des pixels aléatoires.
           On caste backbuffer en unsigned int far * pour
           écrire 2 octets à la fois : 32000 mots = 64000 octets. */
        dst = (unsigned int far *)backbuffer;

        for (i = 0; i < 32000UL; i++)
        {
            /* Générateur congruentiel linéaire (LCG) :
               state = state * a + c (mod 2^32)
               Constantes de Numerical Recipes, reconnues pour
               leur bonne distribution statistique.
               On prend les 16 bits de poids fort (>> 16) car
               les bits bas d'un LCG ont une période plus courte
               et sont moins aléatoires. */
            lcg_state = lcg_state * 1664525UL + 1013904223UL;
            pixel     = (unsigned int)(lcg_state >> 16);

            /* pixel contient 2 octets : deux index de palette
               aléatoires, écrits en une seule opération. */
            dst[i] = pixel;
        }

        flip();

        /* Avancer lastRender d'exactement un intervalle en
           ticks (conversion ms → ticks) pour rester précis
           même si le rendu prend du temps. */
        lastRender += (render_interval_ms * TARGET_HZ) / 1000UL;
    }

    /* -------------------------------------------------------
       Transition vers la scène suivante
       ------------------------------------------------------- */

    /* Après 5 secondes, passer à la scène palette.
       On remet initialized à 0 pour que la prochaine
       activation de cette scène reparte de zéro. */
    if (elapsedTimeMs(sceneStart, now) > scene_ms)
    {
        initialized = 0;
        setScene(SCENE_2);
    }
}
