#ifndef FONTDATA_H
#define FONTDATA_H

/* =========================================================
   FONTDATA.H — Déclarations internes des chargeurs de glyphes
   =========================================================
   Ces deux fonctions sont appelées exclusivement par
   font.c (dans initMyFont8x8/16x16) après _initFontBank().
   Elles ne font partie de l'API publique : ne pas les
   appeler directement depuis les scènes ou main.c.

   Pour ajouter une nouvelle police ou de nouveaux glyphes :
   modifier uniquement fontdata.c, sans toucher à font.c.
   ========================================================= */

/* Charge les glyphes 8x8 dans myFont8x8.
   Appelle defineChar8x8() pour chaque caractère défini. */
void _initFont8x8D(void);

/* Charge les glyphes 16x16 dans myFont16x16.
   Appelle defineChar16x16() pour chaque caractère défini. */
void _initFont16x16D(void);

#endif /* FONTDATA_H */
