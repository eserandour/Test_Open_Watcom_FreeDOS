/* =========================================================
   ENVIRONNEMENT
   Open Watcom 1.9 sous FreeDOS 1.4
   
   PROJET DOS 16 bits
   Version : 22/04/2026 à 12:38
   ========================================================= */
   

/* =========================================================
   INCLUDES
   ========================================================= */

#include <stdio.h>   // Fonctions d'entrée/sortie standard 
#include <stdlib.h>  // Fonctions utilitaires générales 
#include <dos.h>     // Fonctions DOS et interruptions
#include <string.h>  // Fonctions mémoire et chaînes de caractères
#include <malloc.h>  // Fonctions d'allocation spécifiques à la mémoire segmentée (near/far)
#include <conio.h>   // Fonctions console DOS
#include <time.h>    // Gestion du temps et des dates
#include <math.h>    // Fonctions mathématiques
/*
#include <graph.h>   // Graphismes Watcom
#include <ctype.h>   // Fonctions pour tester ou transformer des caractères
*/

/* =========================================================
   CONFIG
   ========================================================= */

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 200
#define BACKBUFFER_SIZE 64000UL  // 320 * 200 (mode 13h, 1 octet/pixel) - UL : Unsigned Long

#define VGA_SEG 0xA000
#define VGA_OFF 0x0000

#define PIT_FREQ 1193180UL  // La fréquence de l'horloge du Programmable Interval Timer (PIT) est de 1 193 180 Hz. C’est la fréquence de base utilisée dans les systèmes DOS pour gérer les temporisations.
#define TARGET_HZ 70  // La fréquence cible de l'intervalle du timer, ici 70 Hz. Cela signifie que nous voulons que notre timer déclenche une interruption 70 fois par seconde.
#define DIVISOR (PIT_FREQ / TARGET_HZ)  // Le diviseur nécessaire pour configurer le PIT à 70 Hz.
// #define BIOS_HZ (PIT_FREQ / 65536) // 18.20648193 Hz

/* =========================================================
   VIDEO MEMORY
   ========================================================= */

// Pointeur vers la mémoire vidéo VGA (mode 13h) - Utilisé exclusivement dans la version de flip la moins rapide
unsigned char far *vga = (unsigned char far *)MK_FP(VGA_SEG, VGA_OFF);
// Buffer mémoire hors écran (double buffering)
unsigned char far *backbuffer = NULL;

/* =========================================================
   TIMER SYSTEM
   ========================================================= */

volatile unsigned long timer_ticks = 0;  // Cette variable est utilisée pour compter les ticks (intervalles de temps) de notre timer.
void interrupt (far *old_timer_isr)();  // Un pointeur vers la fonction ISR (Interrupt Service Routine) d'origine.
static unsigned long accum = 0;

/* ISR principale (cœur du système) */
void interrupt new_timer_isr()
{
    timer_ticks++;
    accum += DIVISOR;  // Accumule les ticks
    if (accum >= 65536UL)
    {
        accum -= 65536UL;
        _chain_intr(old_timer_isr);  // Appelle l'ISR du BIOS (chaînage d'interruptions)
    }
    else
    {
        outp(0x20, 0x20);  // Envoyer un signal d'acknowledgment (ACK) au PIC (Programmable Interrupt Controller)
    }
}
/*
void interrupt new_timer_isr()
{
    timer_ticks++;  // Incrémente le compteur de ticks
    outp(0x20, 0x20);  // Envoyer un signal d'acknowledgment (ACK) au PIC (Programmable Interrupt Controller)
}
*/
/* Installation du timer */
void installTimer(void)
{
    _disable();  // Désactive les interruptions globales

    old_timer_isr = _dos_getvect(0x08);  // Sauvegarde de l'ISR original
    _dos_setvect(0x08, new_timer_isr);   // Remplace l'ISR avec notre fonction

    /* Programmation PIT canal 0 */
    outp(0x43, 0x36);                    // Mode 3, wave carrée
    outp(0x40, DIVISOR & 0xFF);          // Byte bas
    outp(0x40, DIVISOR >> 8);            // Byte haut

    _enable();  // Réactive les interruptions globales
}

/* Restauration du timer */
void restoreTimer(void)
{
    _disable();  // Désactive les interruptions globales
    
    _dos_setvect(0x08, old_timer_isr);   // Restauration de l'ISR original

    /* remettre fréquence par défaut (~18.2 Hz) */
    outp(0x43, 0x36);
    outp(0x40, 0);
    outp(0x40, 0);

    _enable();  // Réactive les interruptions globales
}

/* =========================================================
   TIME HELPERS
   ========================================================= */

/* Retourne le nombre actuel de ticks */
unsigned long readTimer(void)
{
    return timer_ticks;
}

/* Temps écoulé en ticks */
unsigned long elapsedTime(unsigned long start, unsigned long stop)
{
    // Temps écoulé si 'stop' est plus grand ou égal à 'start'
    if (stop >= start) return stop - start;
    // Temps écoulé si 'stop' est inférieur à 'start', cela signifie qu'on a dépassé le comptage des ticks (par exemple, à 32 bits).
    return (0xFFFFFFFFUL - start) + stop + 1;  // UL_MAX pour 32 bits : 0xFFFFFFFFUL
}

/* Temps écoulé en ms */
unsigned long elapsedTimeMs(unsigned long start, unsigned long stop)
{
    return (elapsedTime(start, stop) * 1000UL) / TARGET_HZ;
}

/* Pause en ms : remplay delay qui n'est pas du tout précis */
void pause(unsigned long ms)
{
    unsigned long start = timer_ticks;
    unsigned long ticks_to_wait = (ms * TARGET_HZ) / 1000UL;
    
    while ((unsigned long)(timer_ticks - start) < ticks_to_wait) {
        /* Attend la fin de la boucle */
    }
}

/* =========================================================
   MEMORY (BACKBUFFER)
   ========================================================= */
   
/* Initialise le backbuffer (allocation FAR 64000 octets) */
/*
int initBackbuffer(void)
{
    backbuffer = (unsigned char far *)_fmalloc(BACKBUFFER_SIZE);
    if (!backbuffer) return 0;  // Échec allocation
     _fmemset(backbuffer, 0, BACKBUFFER_SIZE);  // Nettoyage de l'écran (en noir)
    return 1; // Succès
}
*/

int initBackbuffer(void)
{
    backbuffer = (unsigned char far *)_fmalloc(BACKBUFFER_SIZE);
    if (!backbuffer) {
        printf("Erreur : Impossible d'allouer le backbuffer (64000 octets requis).\n");
        return 0;  // Échec allocation
    }
    _fmemset(backbuffer, 0, BACKBUFFER_SIZE);  // Nettoyage de l'écran (en noir)
    return 1; // Succès
}


/* Libère le backbuffer */
void freeBackbuffer(void)
{
    if (backbuffer) {
        _ffree(backbuffer);
        backbuffer = NULL; // Évite pointeur invalide
    }
}

/* =========================================================
   VIDEO MODE
   ========================================================= */

/* Change le mode vidéo via interruption 10h du BIOS */
void setVideoMode(unsigned char mode)
{
    union REGS regs;
    regs.h.ah = 0x00;   // Fonction: set video mode
    regs.h.al = mode;   // Mode (ex: 13h)
    int86(0x10, &regs, &regs);
}

/* Fonction pour désactiver le curseur (appelle une interruption BIOS) */
void cursorOff() {
    _asm {
        mov ah, 01h     // Fonction 01h du BIOS : Définir la taille du curseur
        mov cx, 1400h   // 00010100 00000000 (curseur invisible)
        int 10h         // Interruption 10h du BIOS : Fonctions vidéo
    }
}

/* Fonction pour activer le curseur */
void cursorOn() {
    _asm {
        mov ah, 01h     // Fonction 01h du BIOS : Définir la taille du curseur
        mov cx, 0607h   // 00000110 00000111 : Taille et forme du curseur
        int 10h         // Interruption 10h du BIOS : Fonction vidéo
    }
}

/* =========================================================
   VIDEO FLIP
   ========================================================= */

/* Copie le backbuffer vers la VRAM (affichage) */
void flip(void)  // Optimisé
{
    unsigned char far *src = backbuffer;
    
    _asm {
        push ds

        mov ax, VGA_SEG
        mov es, ax
        xor di, di

        lds si, src

        mov cx, 32000     ; 64000 / 2 (word copy)
        rep movsw

        pop ds
    }
}
/*
void flip()  // Moins rapide à priori
{
    movedata(
        FP_SEG(backbuffer), FP_OFF(backbuffer),
        VGA_SEG, VGA_OFF,
        BACKBUFFER_SIZE
    );
}
*/
/*
void flip()  // Encore moins rapide à priori
{
    _fmemcpy(vga, backbuffer, BACKBUFFER_SIZE);
}
*/

/* =========================================================
   VIDEO PALETTE VGA (256 couleurs)
   ========================================================= */

typedef struct {
    unsigned char r, g, b;
} Color;

Color defaultPalette[256];           // Palette VGA par défaut
Color currentPalette[256];           // Palette en cours d'utilisation
Color paletteA[256], paletteB[256];  // Palettes temporaires pour interpolations
Color grayPalette[256];              // Palette dégradé noir => blanc
Color pinkPalette[256];              // Palette noir + dégradé rouge => blanc

/* Définit une couleur individuelle dans la palette VGA */
void setPaletteColor(unsigned char index, unsigned char r, unsigned char g, unsigned char b)
{
    outp(0x3C8, index);  // Sélectionne l'index de la couleur (0 à 255)
    outp(0x3C9, r);      // Définition de la composante rouge (0–63)
    outp(0x3C9, g);      // Définition de la composante verte (0–63)
    outp(0x3C9, b);      // Définition de la composante bleue (0–63)
}

/* Applique une palette complète au matériel VGA */
void setPalette(Color *pal)
{
    int i;
    
    for (i = 0; i < 256; i++)
        setPaletteColor(i, pal[i].r, pal[i].g, pal[i].b);
}

/* Lit la palette VGA actuelle et la copie dans un tableau en mémoire */
void getPalette(Color *pal)
{
    int i;

    for (i = 0; i < 256; i++)
    {
        outp(0x3C7, i);          // Sélectionne l'index à lire (0-255)
        pal[i].r = inp(0x3C9);   // Composante rouge (0–63)
        pal[i].g = inp(0x3C9);   // Composante verte (0–63)
        pal[i].b = inp(0x3C9);   // Composante bleue (0–63)
    }
}

/* Copie une palette source vers une palette destination */
void copyPalette(Color *src, Color *dest)
{
    int i;
    
    for (i = 0; i < 256; i++) dest[i] = src[i];
}

/* Interpolation linéaire entre deux palettes (palA => palB) */
void lerpPalette(Color *dest, Color *palA, Color *palB, float t)  // t varie de 0 à 1. 0 → palette A, 1 → palette B.
{
    int i;
    
    for (i = 0; i < 256; i++)
    {
        dest[i].r = (unsigned char)(palA[i].r + t * (palB[i].r - palA[i].r));
        dest[i].g = (unsigned char)(palA[i].g + t * (palB[i].g - palA[i].g));
        dest[i].b = (unsigned char)(palA[i].b + t * (palB[i].b - palA[i].b));
    }
}

/* Fade in/out d'une palette (0 = noir, 1 = palette complète) */
void fadePalette(Color *pal, float t)
{
    int i;
    
    Color tmp;
    for (i = 0; i < 256; i++)
    {
        tmp.r = (unsigned char)(pal[i].r * t);
        tmp.g = (unsigned char)(pal[i].g * t);
        tmp.b = (unsigned char)(pal[i].b * t);
        setPaletteColor(i, tmp.r, tmp.g, tmp.b);
    }
}

/* Cycle une portion de palette (fait tourner les couleurs vers la gauche) */
void cyclePaletteLeft(Color *pal, int start, int end)
{
    int i;

    Color tmp = pal[start];
    for (i = start; i < end; i++)
        pal[i] = pal[i+1];
    pal[end] = tmp;
    setPalette(pal);
}

/* Cycle une portion de palette (fait tourner les couleurs vers la droite) */
void cyclePaletteRight(Color *pal, int start, int end)
{
    int i;
    
    Color tmp = pal[end];
    for (i = end; i > start; i--)
        pal[i] = pal[i-1];
    pal[start] = tmp;
    setPalette(pal);
}

/* Génère une palette (noir exclusivement) */
void generateBlackPalette(Color *pal)
{
    _fmemset(pal, 0, 256 * sizeof(Color));
}

/* Génère une palette (dégradé noir => blanc) */
void generateGrayPalette(Color *pal)
{
    int i;

    for (i = 0; i < 256; i++)
    {
        unsigned char v = i >> 2;  // 0–255 => 0–63

        pal[i].r = v;
        pal[i].g = v;
        pal[i].b = v;
    }
}

/* Génère une palette (noir + dégradé rouge => blanc) */
void generatePinkPalette(Color *pal)
{
    int i;
    
    pal[0].r = 0;
    pal[0].g = 0;
    pal[0].b = 0;
    
    for (i = 1; i < 256; i++)
    {
        unsigned char r = 63;                 // Rouge toujours à max
        unsigned char g = (i * 63) / 255;     // Interpolation linéaire 0 => 63
        unsigned char b = (i * 63) / 255;     // Interpolation linéaire 0 => 63
        
        pal[i].r = r;
        pal[i].g = g;
        pal[i].b = b;
    }
}

/* =========================================================
   GRAPHICS CORE
   ========================================================= */

#define OFFSET(x,y) ((y<<8) + (y<<6) + x)  // Calcul offset : (y << 8) + (y << 6) + x = y * 320 + x

/* Efface l'écran en remplissant le backbuffer avec une couleur uniforme */
void clearScreen(unsigned char color)
{
    _fmemset(backbuffer, color, BACKBUFFER_SIZE);
}

/* Dessine un Pixel */
void putPixel(int x, int y, unsigned char color)
{
    // if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)  // Clipping => Pour plus de sécurité, mais c'est plus lent
        backbuffer[OFFSET(x,y)] = color;
}

/* Lit un Pixel */
unsigned char getPixel(int x, int y)
{
    return backbuffer[OFFSET(x,y)];
}

/* Dessine une Ligne (algorithme de tracé de segment de Bresenham) */
void drawLine(int x1, int y1, int x2, int y2, unsigned char color)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int e2;

    while (1) {
        putPixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;

        e2 = err << 1; // e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
    }
}

/* Dessine un Rectangle */
void drawRect(int x1, int y1, int x2, int y2, unsigned char color) {
    drawLine(x1, y1, x2, y1, color);
    drawLine(x2, y1, x2, y2, color);
    drawLine(x2, y2, x1, y2, color);
    drawLine(x1, y2, x1, y1, color);
}

/* Dessine un Rectangle plein */
void drawRectFill(int x1, int y1, int x2, int y2, unsigned char color)
{
    int y;
    int offset;

    for (y = y1; y <= y2; y++) {
        offset = (y << 8) + (y << 6) + x1;  // y * 320 + x1
        _fmemset(backbuffer + offset, color, x2 - x1 + 1);
    }
}
/* Dessine un Cercle (algorithme de tracé d'arc de cercle de Bresenham) */
void drawCircle(int xc, int yc, int r, unsigned char color)
{
    int x = 0, y = r;
    int d = 3 - 2 * r;

    while (x <= y) {
        putPixel(xc + x, yc + y, color);
        putPixel(xc - x, yc + y, color);
        putPixel(xc + x, yc - y, color);
        putPixel(xc - x, yc - y, color);
        putPixel(xc + y, yc + x, color);
        putPixel(xc - y, yc + x, color);
        putPixel(xc + y, yc - x, color);
        putPixel(xc - y, yc - x, color);

        if (d < 0) d += 4 * x + 6;
        else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

/* =========================================================
   TOOLS / DEBUG
   ========================================================= */

/* Affiche la palette courante sur une grille */
void drawPaletteGrid(void)
{
    int gridSize = 16;  // 16x16 = 256 couleurs
    int cellSize = 10;  // Taille des carrés
    int spacing  = 2;   // Espace entre carrés
    
    /* Taille totale en tenant compte des espacements */
    int gridW = gridSize * cellSize + (gridSize - 1) * spacing;
    int gridH = gridSize * cellSize + (gridSize - 1) * spacing;

    /* Centrage */
    int offsetX = (SCREEN_WIDTH  - gridW) / 2;
    int offsetY = (SCREEN_HEIGHT - gridH) / 2;

    int x, y;
    int i = 0;

    for (y = 0; y < gridSize; y++)
    {
        for (x = 0; x < gridSize; x++)
        {
            int px = offsetX + x * (cellSize + spacing);
            int py = offsetY + y * (cellSize + spacing);

            drawRectFill(px, py,
                         px + cellSize - 1,
                         py + cellSize - 1,
                         (unsigned char)i);  // Index direct
            i++;
        }
    }
}

/* Affiche la palette courante pour visualiser l'éventuel dégradé */
void drawPaletteGradient(void)
{
    int x, y;
    int startX = (SCREEN_WIDTH - 256) / 2;  // Centrage horizontal
    
    for (x = 0; x < 256; x++)
    {
        unsigned char color = (unsigned char)x;

        // Remplissage vertical de chaque colonne
        int offset = (0 << 8) + (0 << 6) + (startX + x); // offset du premier pixel de la colonne
        for (y = 0; y < SCREEN_HEIGHT; y++)
        {
            backbuffer[offset] = color;
            offset += SCREEN_WIDTH;  // Passage à la ligne suivante
        }
    }
}

/* =========================================================
   SCENE SYSTEM
   ========================================================= */

typedef enum {
    SCENE_RANDOM,
    SCENE_PALETTE,
    SCENE_END
} Scene;

Scene currentScene;
unsigned long sceneStart;

/* Changer de scène */
void setScene(Scene s)
{
    currentScene = s;
    sceneStart = readTimer();
}

/* =========================================================
   SCENES
   ========================================================= */

/* SCENE 1 : pixels random */
void sceneRandom(void)
{
    static unsigned long lastRender = 0;
    unsigned long now = readTimer();
    unsigned long render_interval_ms = 100UL;  // Durée entre 2 frames
    unsigned long scene_ms = 5000UL; // Durée de la scène

    setPalette(defaultPalette);
    clearScreen(0);  
    
    while (elapsedTimeMs(lastRender, now) >= render_interval_ms)
    {
        // Logique de la scène /////////////////////////////////////////
             
                
               
        // Dessin backbuffer ///////////////////////////////////////////
        
        unsigned long i;

        for (i = 0; i < 64000UL; i++) {
            backbuffer[i] = rand() & 255;
        }
        
        flip();  // Affiche le backbuffer à l'écran
        
        ////////////////////////////////////////////////////////////////

        lastRender += (render_interval_ms * TARGET_HZ) / 1000UL;
    }

    if (elapsedTimeMs(sceneStart, now) > scene_ms)
    {
        setScene(SCENE_PALETTE);
    }
}

/* SCENE 2 : palette */
void scenePalette(void)
{
    static unsigned long lastRender = 0UL;
    unsigned long now = readTimer();
    unsigned long render_interval_ms = 25UL;  // durée entre 2 frames

    // --- Durées des différentes phases (en ms) ---
    const unsigned long DURATION_FADE_IN         = 3000UL;  // Fade depuis le noir
    const unsigned long DURATION_STATIC_DEFAULT  = 1000UL;  // Affichage statique defaultPalette
    const unsigned long DURATION_RIGHT           = 5000UL;  // Cycle vers la droite
    const unsigned long DURATION_LEFT            = 5000UL;  // Cycle vers la gauche
    const unsigned long DURATION_FADE_TO_PINK    = 3000UL;  // Fade vers pinkPalette
    const unsigned long DURATION_STATIC_PINK     = 4000UL;  // Affichage statique pinkPalette

    unsigned long elapsed = elapsedTimeMs(sceneStart, now);

    clearScreen(0);

    // 1) Fade-in depuis le noir vers defaultPalette
    if (elapsed < DURATION_FADE_IN)
    {
        float t = (float)elapsed / (float)DURATION_FADE_IN; // 0 → 1
        fadePalette(defaultPalette, t);
        drawPaletteGrid();
        flip();
        return;
    }

    // 2) Affichage statique de defaultPalette
    else if (elapsed < DURATION_FADE_IN + DURATION_STATIC_DEFAULT)
    {
        setPalette(defaultPalette);
        drawPaletteGrid();
        flip();
        return;
    }

    // 3) Cycle vers la droite
    else if (elapsed < DURATION_FADE_IN + DURATION_STATIC_DEFAULT + DURATION_RIGHT)
    {
        if (elapsedTimeMs(lastRender, now) >= render_interval_ms)
        {
            cyclePaletteRight(defaultPalette, 0, 255);
            drawPaletteGrid();
            flip();
            lastRender = now;
        }
        return;
    }

    // 4) Cycle vers la gauche
    else if (elapsed < DURATION_FADE_IN + DURATION_STATIC_DEFAULT + DURATION_RIGHT + DURATION_LEFT)
    {
        if (elapsedTimeMs(lastRender, now) >= render_interval_ms)
        {
            cyclePaletteLeft(defaultPalette, 0, 255);
            drawPaletteGrid();
            flip();
            lastRender = now;
        }
        return;
    }

    // 5) Fade progressif vers pinkPalette
    else if (elapsed < DURATION_FADE_IN + DURATION_STATIC_DEFAULT + DURATION_RIGHT + DURATION_LEFT + DURATION_FADE_TO_PINK)
    {
        unsigned long fadeElapsed = elapsed - (DURATION_FADE_IN + DURATION_STATIC_DEFAULT + DURATION_RIGHT + DURATION_LEFT);
        float t = (float)fadeElapsed / (float)DURATION_FADE_TO_PINK; // 0 → 1
        lerpPalette(currentPalette, defaultPalette, pinkPalette, t); // Interpolation
        setPalette(currentPalette);
        drawPaletteGrid();
        flip();
        return;
    }

    // 6) Affichage statique de pinkPalette
    else if (elapsed < DURATION_FADE_IN + DURATION_STATIC_DEFAULT + DURATION_RIGHT + DURATION_LEFT + DURATION_FADE_TO_PINK + DURATION_STATIC_PINK)
    {
        setPalette(pinkPalette);
        drawPaletteGrid();
        flip();
        return;
    }

    // 7) Fin de la scène
    else
    {
        setScene(SCENE_END);
    }
}

/* SCENE 3 : fin */
void sceneEnd(void)
{
    static unsigned long lastRender = 0;
    unsigned long now = readTimer();
    unsigned long render_interval_ms = 100UL;  // Durée entre 2 frames
    unsigned long scene_ms = 3000UL; // Durée de la scène
      
    while (elapsedTimeMs(lastRender, now) >= render_interval_ms)
    {
        // Logique de la scène /////////////////////////////////////////

               
                
        
        // Dessin backbuffer ///////////////////////////////////////////
        
        clearScreen(127);
        flip();
            
        ////////////////////////////////////////////////////////////////

        //lastRender += (render_interval_ms * TARGET_HZ) / 1000UL;
        lastRender = now; // Réinitialise lastRender avec l'heure actuelle
    }

    // Quitte après scene_ms
    if (elapsedTimeMs(sceneStart, now) > scene_ms)
    {
        setScene(SCENE_RANDOM);
    }
}

/* =========================================================
   MAIN LOOP
   ========================================================= */
   
typedef void (*SceneFunc)(void);
SceneFunc scenes[] =  // Tableau de fonctions
{
	sceneRandom,
	scenePalette,
	sceneEnd
};

int main(void)
{
    srand(time(0));  // Initialise le générateur de nombres aléatoires avec l'heure actuelle
    if (!initBackbuffer()) return 1;  // Allocation mémoire
    setVideoMode(0x13);  // Passage en mode VGA 13h (320x200x256)
    
    getPalette(defaultPalette);  // Sauvegarder la palette par défaut
    generatePinkPalette(pinkPalette);  // Génère une palette (noir + dégradé rouge => blanc)
        
    installTimer();  // Initialisation du timer
    
    setScene(SCENE_RANDOM);
    while (!kbhit()) scenes[currentScene]();

    restoreTimer();  // Restauration du timer
    setVideoMode(0x03);  // Retour mode texte
    freeBackbuffer();  // Libération mémoire

    return 0;
}
