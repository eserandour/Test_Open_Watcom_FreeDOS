/* =========================================================
   SCENE4.C — Affiche HELLO au centre de l'ecran
   ========================================================= */

#include <stdlib.h>   /* exit                              */
#include "video.h"    /* flip                              */
#include "graphics.h" /* clearScreen                       */
#include "image.h"    /* loadImagePal, IMG_OK              */
#include "font2.h"    /* drawTextCentered2                 */
#include "scene.h"    /* setScene, SCENE_4                 */

void shutdown(void);

void scene4(void)
{
    static int initialized = 0;

    if (!initialized)
    {
        int err;

        err = loadImagePal("images\\font.pal");
        if (err != IMG_OK) { shutdown(); exit(1); }

        clearScreen(0);                          /* fond jaune-vert  */
        drawTextCentered2("HELLO", 84, 127);     /* index 127 = transparent */
        drawText2("WORLD", 0,0,-1);     /* index -1 = opaque */
        flip();

        initialized = 1;
    }
}
