#!/usr/bin/env python3
# img2vga.py — convertit une image en .RAW + .PAL pour le mode 13h VGA
# Usage : python3 img2vga.py image.jpg
# Usage : python3 img2vga.py image.png
#
# Néccesite d'avoir installé au préalable python3-pillow
#
# Cas gérés :
# - RGB classique (JPG, PNG sans transparence)
# - RGBA (PNG avec transparence) : aplati sur fond noir
# - Indexée (PNG déjà en 256 couleurs) : palette préservée, pas de re-quantification
# - Niveaux de gris (mode L) : converti en RGB avant quantification

import sys
from PIL import Image

if len(sys.argv) != 2:
    print("Usage: python3 img2vga.py <image>")
    sys.exit(1)

src  = sys.argv[1]
base = src.rsplit('.', 1)[0]

img = Image.open(src)

# --- Cas 1 : image déjà indexée en 256 couleurs ou moins ---
# On préserve la palette d'origine sans re-quantifier.
if img.mode == 'P':
    img = img.resize((320, 200), Image.LANCZOS)

# --- Cas 2 : RGBA (transparence) ---
# On aplatit le canal alpha sur du noir avant de quantifier.
elif img.mode == 'RGBA':
    bg = Image.new('RGB', img.size, (0, 0, 0))
    bg.paste(img, mask=img.split()[3])
    img = bg.resize((320, 200), Image.LANCZOS)
    img = img.quantize(colors=256, dither=Image.Dither.FLOYDSTEINBERG)

# --- Cas 3 : RGB ou L (niveaux de gris) ---
else:
    img = img.convert('RGB')
    img = img.resize((320, 200), Image.LANCZOS)
    img = img.quantize(colors=256, dither=Image.Dither.FLOYDSTEINBERG)

# --- RAW : 64000 octets d'index de palette ---
raw = img.tobytes()
assert len(raw) == 64000, f"Taille RAW inattendue : {len(raw)} octets"
with open(base + ".raw", "wb") as f:
    f.write(raw)

# --- PAL : 256 × RGB sur 6 bits (0-63) ---
# getpalette() peut renvoyer une liste tronquée si l'image a peu de couleurs.
# On padde à 256 entrées (768 valeurs) avant la conversion 8→6 bits.
pal8 = img.getpalette()
pal8 += [0] * (768 - len(pal8))
pal6 = bytes(v >> 2 for v in pal8)
with open(base + ".pal", "wb") as f:
    f.write(pal6)

print(f"OK  {base}.raw  ({len(raw)} octets)")
print(f"OK  {base}.pal  (256 couleurs sur 6 bits)")
