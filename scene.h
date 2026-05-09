#ifndef SCENE_H
#define SCENE_H

/* =========================================================
   SCENE.H — Gestionnaire de scènes
   =========================================================
   Une scène est une fonction autonome appelée en boucle
   par main(). Elle gère son propre état interne via des
   variables statiques et peut décider de passer à la
   scène suivante via setScene().

   Pour ajouter une scène :
   1. Ajouter une valeur dans l'enum Scene.
   2. Écrire la fonction dans scenes/<nom>.c.
   3. L'ajouter dans le tableau scenes[] de scene.c.
   ========================================================= */

typedef enum {
    SCENE_1 = 0,   /* pixels aléatoires (LCG)   */
    SCENE_2 = 1,   /* démonstration palette VGA */
    SCENE_3 = 2,   /* démonstration des polices */
    SCENE_4 = 3    /* affichage image RAW+PAL   */
} Scene;

extern Scene currentScene;
extern unsigned long sceneStart;

void setScene(Scene s);
void runCurrentScene(void);

#endif /* SCENE_H */
