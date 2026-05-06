// ═══════════════════════════════════════════════════════════
// ÉTAT GLOBAL
// ═══════════════════════════════════════════════════════════

// 128 caractères ASCII complets : 0x00 à 0x7F
const CHARS = Array.from({length: 128}, (_, i) => String.fromCharCode(i));

// Police 128 glyphes (0x00-0x7F)
const DEFAULT_FONT8 = [
  [0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00], // 0x00
  [0x7E,0x81,0xA5,0x81,0xBD,0x99,0x81,0x7E], // 0x01
  [0x7E,0xFF,0xDB,0xFF,0xC3,0xE7,0xFF,0x7E], // 0x02
  [0x6C,0xFE,0xFE,0xFE,0x7C,0x38,0x10,0x00], // 0x03
  [0x10,0x38,0x7C,0xFE,0x7C,0x38,0x10,0x00], // 0x04
  [0x38,0x7C,0x38,0xFE,0xFE,0xD6,0x10,0x38], // 0x05
  [0x10,0x10,0x38,0x7C,0xFE,0x7C,0x10,0x38], // 0x06
  [0x00,0x00,0x18,0x3C,0x3C,0x18,0x00,0x00], // 0x07
  [0xFF,0xFF,0xE7,0xC3,0xC3,0xE7,0xFF,0xFF], // 0x08
  [0x00,0x3C,0x66,0x42,0x42,0x66,0x3C,0x00], // 0x09
  [0xFF,0xC3,0x99,0xBD,0xBD,0x99,0xC3,0xFF], // 0x0A
  [0x0F,0x07,0x0F,0x7D,0xCC,0xCC,0xCC,0x78], // 0x0B
  [0x3C,0x66,0x66,0x66,0x3C,0x18,0x7E,0x18], // 0x0C
  [0x3F,0x33,0x3F,0x30,0x30,0x70,0xF0,0xE0], // 0x0D
  [0x7F,0x63,0x7F,0x63,0x63,0x67,0xE6,0xC0], // 0x0E
  [0x99,0x5A,0x3C,0xE7,0xE7,0x3C,0x5A,0x99], // 0x0F
  [0x80,0xE0,0xF8,0xFE,0xF8,0xE0,0x80,0x00], // 0x10
  [0x02,0x0E,0x3E,0xFE,0x3E,0x0E,0x02,0x00], // 0x11
  [0x18,0x3C,0x7E,0x18,0x18,0x7E,0x3C,0x18], // 0x12
  [0x66,0x66,0x66,0x66,0x66,0x00,0x66,0x00], // 0x13
  [0x7F,0xDB,0xDB,0x7B,0x1B,0x1B,0x1B,0x00], // 0x14
  [0x3E,0x63,0x38,0x6C,0x6C,0x38,0xCC,0x78], // 0x15
  [0x00,0x00,0x00,0x00,0x7E,0x7E,0x7E,0x00], // 0x16
  [0x18,0x3C,0x7E,0x18,0x7E,0x3C,0x18,0xFF], // 0x17
  [0x18,0x3C,0x7E,0x18,0x18,0x18,0x18,0x00], // 0x18
  [0x18,0x18,0x18,0x18,0x7E,0x3C,0x18,0x00], // 0x19
  [0x00,0x18,0x0C,0xFE,0x0C,0x18,0x00,0x00], // 0x1A
  [0x00,0x30,0x60,0xFE,0x60,0x30,0x00,0x00], // 0x1B
  [0x00,0x00,0xC0,0xC0,0xC0,0xFE,0x00,0x00], // 0x1C
  [0x00,0x24,0x66,0xFF,0x66,0x24,0x00,0x00], // 0x1D
  [0x00,0x18,0x3C,0x7E,0xFF,0xFF,0x00,0x00], // 0x1E
  [0x00,0xFF,0xFF,0x7E,0x3C,0x18,0x00,0x00], // 0x1F
  [0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00], // 0x20
  [0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00], // 0x21
  [0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00], // 0x22
  [0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00], // 0x23
  [0x0C,0x3E,0x6C,0x3C,0x1E,0x7C,0x18,0x00], // 0x24
  [0x62,0x66,0x0C,0x18,0x30,0x66,0x46,0x00], // 0x25
  [0x38,0x6C,0x6C,0x38,0x6D,0x66,0x3B,0x00], // 0x26
  [0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00], // 0x27
  [0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00], // 0x28
  [0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00], // 0x29
  [0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00], // 0x2A
  [0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00], // 0x2B
  [0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30], // 0x2C
  [0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00], // 0x2D
  [0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00], // 0x2E
  [0x02,0x06,0x0C,0x18,0x30,0x60,0x40,0x00], // 0x2F
  [0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00], // 0x30
  [0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00], // 0x31
  [0x3C,0x66,0x06,0x0C,0x18,0x30,0x7E,0x00], // 0x32
  [0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00], // 0x33
  [0x06,0x0E,0x1E,0x66,0x7F,0x06,0x06,0x00], // 0x34
  [0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00], // 0x35
  [0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00], // 0x36
  [0x7E,0x06,0x0C,0x18,0x30,0x30,0x30,0x00], // 0x37
  [0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00], // 0x38
  [0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00], // 0x39
  [0x00,0x18,0x18,0x00,0x18,0x18,0x00,0x00], // 0x3A
  [0x00,0x18,0x18,0x00,0x18,0x18,0x30,0x00], // 0x3B
  [0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00], // 0x3C
  [0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00], // 0x3D
  [0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00], // 0x3E
  [0x3C,0x66,0x06,0x0C,0x18,0x00,0x18,0x00], // 0x3F
  [0x3C,0x66,0x6E,0x6A,0x6E,0x60,0x3C,0x00], // 0x40
  [0x18,0x3C,0x66,0x7E,0x66,0x66,0x66,0x00], // 0x41
  [0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00], // 0x42
  [0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00], // 0x43
  [0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00], // 0x44
  [0x7E,0x60,0x60,0x7C,0x60,0x60,0x7E,0x00], // 0x45
  [0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x00], // 0x46
  [0x3C,0x66,0x60,0x6E,0x66,0x66,0x3C,0x00], // 0x47
  [0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00], // 0x48
  [0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00], // 0x49
  [0x1E,0x06,0x06,0x06,0x06,0x66,0x3C,0x00], // 0x4A
  [0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00], // 0x4B
  [0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00], // 0x4C
  [0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00], // 0x4D
  [0x66,0x76,0x7E,0x7E,0x6E,0x66,0x66,0x00], // 0x4E
  [0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00], // 0x4F
  [0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00], // 0x50
  [0x3C,0x66,0x66,0x66,0x6A,0x6C,0x36,0x00], // 0x51
  [0x7C,0x66,0x66,0x7C,0x6C,0x66,0x66,0x00], // 0x52
  [0x3C,0x66,0x60,0x3C,0x06,0x66,0x3C,0x00], // 0x53
  [0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00], // 0x54
  [0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00], // 0x55
  [0x66,0x66,0x66,0x66,0x3C,0x3C,0x18,0x00], // 0x56
  [0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00], // 0x57
  [0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00], // 0x58
  [0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00], // 0x59
  [0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00], // 0x5A
  [0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00], // 0x5B
  [0x40,0x60,0x30,0x18,0x0C,0x06,0x02,0x00], // 0x5C
  [0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00], // 0x5D
  [0x18,0x3C,0x66,0x00,0x00,0x00,0x00,0x00], // 0x5E
  [0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF], // 0x5F
  [0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00], // 0x60
  [0x00,0x00,0x3C,0x06,0x3E,0x66,0x3E,0x00], // 0x61
  [0x60,0x60,0x7C,0x66,0x66,0x66,0x7C,0x00], // 0x62
  [0x00,0x00,0x3C,0x60,0x60,0x60,0x3C,0x00], // 0x63
  [0x06,0x06,0x3E,0x66,0x66,0x66,0x3E,0x00], // 0x64
  [0x00,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00], // 0x65
  [0x1C,0x30,0x30,0x7C,0x30,0x30,0x30,0x00], // 0x66
  [0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x3C], // 0x67
  [0x60,0x60,0x7C,0x66,0x66,0x66,0x66,0x00], // 0x68
  [0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00], // 0x69
  [0x06,0x00,0x0E,0x06,0x06,0x06,0x66,0x3C], // 0x6A
  [0x60,0x60,0x66,0x6C,0x78,0x6C,0x66,0x00], // 0x6B
  [0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00], // 0x6C
  [0x00,0x00,0x66,0x7F,0x7F,0x6B,0x63,0x00], // 0x6D
  [0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x00], // 0x6E
  [0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00], // 0x6F
  [0x00,0x00,0x7C,0x66,0x66,0x7C,0x60,0x60], // 0x70
  [0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x06], // 0x71
  [0x00,0x00,0x6C,0x76,0x60,0x60,0x60,0x00], // 0x72
  [0x00,0x00,0x3C,0x60,0x3C,0x06,0x7C,0x00], // 0x73
  [0x30,0x30,0x7C,0x30,0x30,0x30,0x1C,0x00], // 0x74
  [0x00,0x00,0x66,0x66,0x66,0x66,0x3E,0x00], // 0x75
  [0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x00], // 0x76
  [0x00,0x00,0x63,0x6B,0x7F,0x3E,0x36,0x00], // 0x77
  [0x00,0x00,0x66,0x3C,0x18,0x3C,0x66,0x00], // 0x78
  [0x00,0x00,0x66,0x66,0x3E,0x06,0x3C,0x00], // 0x79
  [0x00,0x00,0x7E,0x0C,0x18,0x30,0x7E,0x00], // 0x7A
  [0x0E,0x18,0x18,0x70,0x18,0x18,0x0E,0x00], // 0x7B
  [0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00], // 0x7C
  [0x70,0x18,0x18,0x0E,0x18,0x18,0x70,0x00], // 0x7D
  [0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00], // 0x7E
  [0x00,0x10,0x38,0x6C,0xC6,0xC6,0xFE,0x00], // 0x7F
];

// Mode courant : 8 ou 16
let MODE = 8;

// Deux banques de polices séparées
const font8  = {};
const font16 = {};
for (const c of CHARS) {
  const code = c.charCodeAt(0);
  // Indexer par code numérique pour couvrir les non-imprimables (0x00-0x1F, 0x7F)
  // et par caractère pour les imprimables — DEFAULT_FONT8 utilise les deux notations
  font8[c]  = DEFAULT_FONT8[code] ? [...DEFAULT_FONT8[code]] : new Array(8).fill(0);
  font16[c] = new Array(32).fill(0);
}

// Référence courante (pointe vers font8 ou font16)
let font = font8;

let current    = 'A';
let isDrawing  = false;
let drawVal    = null;

// ═══════════════════════════════════════════════════════════
// GESTION DU MODE
// ═══════════════════════════════════════════════════════════

function setMode(m) {
  MODE = m;
  font = (m === 8) ? font8 : font16;

  document.getElementById('btn-mode8').classList.toggle('active', m === 8);
  document.getElementById('btn-mode16').classList.toggle('active', m === 16);
  document.getElementById('status-right').textContent =
    `Font Editor — DOS ${m}×${m} // mode 13h`;
  document.title = `Font Editor — DOS ${m}×${m}`;

  // Classe sur body pour CSS cellule 16x16
  document.body.classList.toggle('mode16', m === 16);

  rebuildAll();
  updateAll();
}

// ═══════════════════════════════════════════════════════════
// CONSTRUCTION DE L'UI (reconstruction selon mode)
// ═══════════════════════════════════════════════════════════

function rebuildAll() {
  buildColLabels();
  buildGrid();
  buildCharGrid();
}

function buildColLabels() {
  const wrap = document.getElementById('col-labels');
  wrap.innerHTML = '';
  const spacer = document.createElement('div');
  spacer.className = 'col-lbl-spacer';
  wrap.appendChild(spacer);

  for (let i = 0; i < MODE; i++) {
    const s = document.createElement('span');
    s.textContent = i < 10 ? i : String.fromCharCode(55 + i); // A,B,C... après 9
    // largeur dynamique selon mode
    const cellPx = MODE === 8 ? 44 : 24;
    const gapPx  = 3;
    s.style.width = cellPx + 'px';
    s.style.marginRight = gapPx + 'px';
    s.style.display = 'inline-block';
    wrap.appendChild(s);
  }
}

function buildGrid() {
  const gridEl = document.getElementById('grid');
  gridEl.innerHTML = '';

  for (let row = 0; row < MODE; row++) {
    const rowDiv = document.createElement('div');
    rowDiv.className = 'grid-row';

    const lbl = document.createElement('div');
    lbl.className = 'row-label';
    lbl.textContent = row < 10 ? row : String.fromCharCode(55 + row);
    rowDiv.appendChild(lbl);

    for (let col = 0; col < MODE; col++) {
      const cell = document.createElement('div');
      cell.className = 'cell';
      cell.dataset.row = row;
      cell.dataset.col = col;

      cell.addEventListener('mousedown', e => {
        e.preventDefault();
        const bit = getPixel(row, col);
        drawVal = !bit;
        setPixel(row, col, drawVal);
        isDrawing = true;
        updateAll();
      });

      cell.addEventListener('mouseenter', () => {
        if (isDrawing) {
          setPixel(row, col, drawVal);
          updateAll();
        }
      });

      rowDiv.appendChild(cell);
    }
    gridEl.appendChild(rowDiv);
  }
}

function charLabel(c) {
  const code = c.charCodeAt(0);
  if (code < 0x20) return code.toString(16).toUpperCase().padStart(2,'0');
  if (code === 0x20) return '·';
  if (code === 0x7F) return '7F';
  return c;
}

function charTitle(c) {
  const code = c.charCodeAt(0);
  const hex = code.toString(16).toUpperCase().padStart(2,'0');
  if (code < 0x20) return `0x${hex} (ctrl)`;
  if (code === 0x7F) return `0x7F (DEL)`;
  return `'${c}' (0x${hex})`;
}

function buildCharGrid() {
  const gridEl = document.getElementById('char-grid');
  gridEl.innerHTML = '';
  for (const c of CHARS) {
    const cell = document.createElement('div');
    cell.className = 'char-cell';
    cell.textContent = charLabel(c);
    cell.title = charTitle(c);
    // Réduire la taille du texte pour les codes hex 2 caractères
    if (c.charCodeAt(0) < 0x20 || c.charCodeAt(0) === 0x7F)
      cell.style.fontSize = '8px';
    cell.dataset.charCode = c.charCodeAt(0);
    cell.addEventListener('click', () => selectChar(c));
    gridEl.appendChild(cell);
  }
}

document.addEventListener('mouseup', () => { isDrawing = false; });

// ═══════════════════════════════════════════════════════════
// OPÉRATIONS SUR LES GLYPHES
// ═══════════════════════════════════════════════════════════

function getPixel(row, col) {
  if (MODE === 8) {
    return !!(font[current][row] & (1 << (7 - col)));
  } else {
    // 16x16 : 2 octets par ligne, [row*2] et [row*2+1]
    const byteIdx = row * 2 + (col < 8 ? 0 : 1);
    const bitPos  = 7 - (col % 8);
    return !!(font[current][byteIdx] & (1 << bitPos));
  }
}

function setPixel(row, col, val) {
  if (MODE === 8) {
    if (val) font[current][row] |= (1 << (7 - col));
    else      font[current][row] &= ~(1 << (7 - col));
  } else {
    const byteIdx = row * 2 + (col < 8 ? 0 : 1);
    const bitPos  = 7 - (col % 8);
    if (val) font[current][byteIdx] |= (1 << bitPos);
    else      font[current][byteIdx] &= ~(1 << bitPos);
  }
}

function hasData(c) {
  return font[c].some(b => b !== 0);
}

function selectChar(c) {
  current = c;
  updateAll();
}

function shiftUp() {
  if (MODE === 8) {
    font[current] = [...font[current].slice(1), font[current][0]];
  } else {
    font[current] = [...font[current].slice(2), ...font[current].slice(0, 2)];
  }
  updateAll();
}

function shiftDown() {
  if (MODE === 8) {
    font[current] = [font[current][7], ...font[current].slice(0, 7)];
  } else {
    const last2 = font[current].slice(-2);
    font[current] = [...last2, ...font[current].slice(0, -2)];
  }
  updateAll();
}

function shiftLeft() {
  if (MODE === 8) {
    font[current] = font[current].map(b => ((b << 1) | (b >> 7)) & 0xFF);
  } else {
    const g = font[current];
    for (let r = 0; r < 16; r++) {
      const hi = g[r*2], lo = g[r*2+1];
      const combined = (hi << 8) | lo;
      const shifted  = ((combined << 1) | (combined >> 15)) & 0xFFFF;
      g[r*2]   = (shifted >> 8) & 0xFF;
      g[r*2+1] = shifted & 0xFF;
    }
  }
  updateAll();
}

function shiftRight() {
  if (MODE === 8) {
    font[current] = font[current].map(b => ((b >> 1) | (b << 7)) & 0xFF);
  } else {
    const g = font[current];
    for (let r = 0; r < 16; r++) {
      const hi = g[r*2], lo = g[r*2+1];
      const combined = (hi << 8) | lo;
      const shifted  = ((combined >> 1) | ((combined & 1) << 15)) & 0xFFFF;
      g[r*2]   = (shifted >> 8) & 0xFF;
      g[r*2+1] = shifted & 0xFF;
    }
  }
  updateAll();
}

function flipH() {
  if (MODE === 8) {
    font[current] = font[current].map(b => {
      let r = 0;
      for (let i = 0; i < 8; i++) if (b & (1 << i)) r |= (1 << (7 - i));
      return r;
    });
  } else {
    const g = font[current];
    for (let r = 0; r < 16; r++) {
      const hi = g[r*2], lo = g[r*2+1];
      let newHi = 0, newLo = 0;
      for (let i = 0; i < 8; i++) {
        if (lo & (1 << i)) newHi |= (1 << (7 - i));
        if (hi & (1 << i)) newLo |= (1 << (7 - i));
      }
      g[r*2] = newHi; g[r*2+1] = newLo;
    }
  }
  updateAll();
}

function flipV() {
  if (MODE === 8) {
    font[current] = [...font[current]].reverse();
  } else {
    const g = font[current];
    const rows = [];
    for (let r = 0; r < 16; r++) rows.push([g[r*2], g[r*2+1]]);
    rows.reverse();
    for (let r = 0; r < 16; r++) { g[r*2] = rows[r][0]; g[r*2+1] = rows[r][1]; }
  }
  updateAll();
}

function invert() {
  font[current] = font[current].map(b => (~b) & 0xFF);
  updateAll();
}

function clearGlyph() {
  font[current] = new Array(MODE === 8 ? 8 : 32).fill(0);
  updateAll();
}

// ═══════════════════════════════════════════════════════════
// MISE À JOUR DE L'UI
// ═══════════════════════════════════════════════════════════

function updateAll() {
  updateGridDisplay();
  updateCharInfo();
  updateCharGrid();
  updatePreviews();
  updateBytesDisplay();
  updateStringPreview();
  updateCodeOutput();
  updateStats();
}

function updateGridDisplay() {
  document.querySelectorAll('.cell').forEach(cell => {
    const row = +cell.dataset.row;
    const col = +cell.dataset.col;
    cell.classList.toggle('on', getPixel(row, col));
  });
}

function updateCharInfo() {
  const code = current.charCodeAt(0);
  { const code2 = current.charCodeAt(0);
    document.getElementById('char-badge').textContent =
      code2 < 0x20 || code2 === 0x7F ? charLabel(current) : (current === ' ' ? '·' : current); }
  document.getElementById('char-name').textContent  = getCharName(current);
  document.getElementById('char-code').textContent  =
    `ASCII 0x${code.toString(16).toUpperCase().padStart(2,'0')} — index ${code} (dec)`;
}

function getCharName(c) {
  const code = c.charCodeAt(0);
  const CTRL_NAMES = ['NUL','SOH','STX','ETX','EOT','ENQ','ACK','BEL',
                      'BS','HT','LF','VT','FF','CR','SO','SI',
                      'DLE','DC1','DC2','DC3','DC4','NAK','SYN','ETB',
                      'CAN','EM','SUB','ESC','FS','GS','RS','US'];
  if (code < 0x20) return `Ctrl 0x${code.toString(16).toUpperCase().padStart(2,'0')} (${CTRL_NAMES[code]})`;
  if (c === ' ')   return 'Espace (0x20)';
  if (code === 0x7F) return 'DEL (0x7F)';
  if (code >= 65 && code <= 90)  return `Majuscule '${c}'`;
  if (code >= 97 && code <= 122) return `Minuscule '${c}'`;
  if (code >= 48 && code <= 57)  return `Chiffre '${c}'`;
  return `Symbole '${c}'`;
}

function updateCharGrid() {
  document.querySelectorAll('.char-cell').forEach(cell => {
    const c = String.fromCharCode(parseInt(cell.dataset.charCode));
    cell.className = 'char-cell';
    if (c === current) cell.classList.add('active');
    if (hasData(c))    cell.classList.add('has-data');
  });
}

// Dessine un glyphe sur un canvas en mode 8 ou 16
function drawGlyphToCanvas(canvas, glyph, m, scale) {
  canvas.width  = m * scale;
  canvas.height = m * scale;
  const ctx = canvas.getContext('2d');
  ctx.fillStyle = '#000';
  ctx.fillRect(0, 0, canvas.width, canvas.height);
  ctx.fillStyle = '#fff';
  for (let r = 0; r < m; r++) {
    for (let c = 0; c < m; c++) {
      let on;
      if (m === 8) {
        on = !!(glyph[r] & (1 << (7 - c)));
      } else {
        const byteIdx = r * 2 + (c < 8 ? 0 : 1);
        const bitPos  = 7 - (c % 8);
        on = !!(glyph[byteIdx] & (1 << bitPos));
      }
      if (on) ctx.fillRect(c * scale, r * scale, scale, scale);
    }
  }
}

function updatePreviews() {
  const g = font[current];
  drawGlyphToCanvas(document.getElementById('prev1'), g, MODE, 1);
  drawGlyphToCanvas(document.getElementById('prev2'), g, MODE, 2);
  drawGlyphToCanvas(document.getElementById('prev3'), g, MODE, 3);
  drawGlyphToCanvas(document.getElementById('prev4'), g, MODE, 4);
  drawGlyphToCanvas(document.getElementById('prev6'), g, MODE, 6);

  ['prev1','prev2','prev3','prev4','prev6'].forEach((id, i) => {
    const scales = [1,2,3,4,6];
    const px = MODE * scales[i];
    document.getElementById(id).style.width  = px + 'px';
    document.getElementById(id).style.height = px + 'px';
  });
}

function updateBytesDisplay() {
  const g = font[current];
  const wrap = document.getElementById('bytes-display');
  wrap.innerHTML = '';

  if (MODE === 8) {
    g.forEach((b, i) => {
      const row = document.createElement('div');
      row.className = 'byte-row';
      const idx  = el('div','byte-idx', i);
      const hex  = el('div','byte-hex', '0x' + b.toString(16).toUpperCase().padStart(2,'0'));
      const bits = makeBits(b, 8);
      row.appendChild(idx); row.appendChild(hex); row.appendChild(bits);
      wrap.appendChild(row);
    });
  } else {
    // 16 lignes × 2 octets
    for (let r = 0; r < 16; r++) {
      const hi = g[r*2], lo = g[r*2+1];
      const row = document.createElement('div');
      row.className = 'byte-row';
      const idx = el('div','byte-idx', r);
      const hex = el('div','byte-hex byte-hex2',
        '0x'+hi.toString(16).toUpperCase().padStart(2,'0') + ' ' +
        '0x'+lo.toString(16).toUpperCase().padStart(2,'0'));
      const bits1 = makeBits(hi, 8);
      const bits2 = makeBits(lo, 8);
      // séparateur visuel entre les deux octets
      const sep = document.createElement('div');
      sep.style.cssText = 'width:4px;flex-shrink:0';
      row.appendChild(idx); row.appendChild(hex);
      row.appendChild(bits1); row.appendChild(sep); row.appendChild(bits2);
      wrap.appendChild(row);
    }
  }
}

function el(tag, cls, txt) {
  const d = document.createElement('div');
  d.className = cls;
  if (txt !== undefined) d.textContent = txt;
  return d;
}

function makeBits(b, n) {
  const wrap = document.createElement('div');
  wrap.className = 'byte-bits';
  for (let bit = n-1; bit >= 0; bit--) {
    const d = document.createElement('div');
    d.className = 'bit' + (b & (1<<bit) ? ' on' : '');
    wrap.appendChild(d);
  }
  return wrap;
}

function updateStringPreview() {
  const str = document.getElementById('preview-input').value || 'DEMO';
  renderStringToCanvas(document.getElementById('string-preview'),  str, 1);
  renderStringToCanvas(document.getElementById('string-preview2'), str, 2);
}

function renderStringToCanvas(canvas, str, scale) {
  const w = Math.max(str.length * MODE * scale, MODE);
  canvas.width  = w;
  canvas.height = MODE * scale;
  const ctx = canvas.getContext('2d');
  ctx.fillStyle = '#000';
  ctx.fillRect(0, 0, w, MODE * scale);
  ctx.fillStyle = '#fff';
  for (let ci = 0; ci < str.length; ci++) {
    const c = str[ci];
    const g = font[c] || new Array(MODE === 8 ? 8 : 32).fill(0);
    for (let r = 0; r < MODE; r++) {
      for (let col = 0; col < MODE; col++) {
        let on;
        if (MODE === 8) {
          on = !!(g[r] & (1 << (7 - col)));
        } else {
          const byteIdx = r * 2 + (col < 8 ? 0 : 1);
          on = !!(g[byteIdx] & (1 << (7 - (col % 8))));
        }
        if (on) ctx.fillRect(ci * MODE * scale + col * scale, r * scale, scale, scale);
      }
    }
  }
}

function updateCodeOutput() {
  const out = document.getElementById('code-output');
  // On affiche seulement les définis pour la lisibilité du panneau,
  // mais exportC/exportDefined exportent tout
  const defined = CHARS.filter(c => hasData(c));
  out.innerHTML = defined.length
    ? generateCodeHTML(defined)
    : '<span style="color:var(--dim)">// Aucun glyphe défini</span>';
}

function charLiteral(c) {
  const code = c.charCodeAt(0);
  if (code < 0x20 || code === 0x7F) return String(code);
  if (c === "'")  return "'\\''";
  if (c === "\\") return "'\\\\'";
  return "'" + c + "'";
}

function hex8(b)  { return '0x' + b.toString(16).toUpperCase().padStart(2,'0'); }
function hex16(w) { return '0x' + w.toString(16).toUpperCase().padStart(4,'0'); }

function generateCode(chars, header=true) {
  const lines = [];

  if (MODE === 8) {
    // ── Notation fontdata.c 8x8 ──────────────────────────
    // void _initFont8D(void)
    // {
    //     defineChar8(myFont8,'A', 0x18,0x3C,...,0x00);
    // }
    if (header) {
      lines.push('void _initFont8D(void)');
      lines.push('{');
    }
    let lastGroup = null;
    for (const c of chars) {
      const code = c.charCodeAt(0);
      // Commentaires de groupe comme dans fontdata.c
      const group = code < 0x20 ? 'Codes ASCII 0x00–0x1F'
                  : code < 0x30 ? 'Espace, Ponctuation et Symboles'
                  : code < 0x3A ? 'Chiffres'
                  : code < 0x41 ? 'Ponctuation et Symboles'
                  : code < 0x5B ? 'Majuscules'
                  : code < 0x61 ? 'Symboles'
                  : code < 0x7B ? 'Minuscules'
                  : 'Codes ASCII 0x7B–0x7F';
      if (group !== lastGroup) {
        lines.push(`    /* ${group} */`);
        lastGroup = group;
      }
      const g  = font[c];
      const cl = charLiteral(c);
      const bytes = g.map(hex8).join(',');
      lines.push(`    defineChar8(myFont8,${cl}, ${bytes});`);
    }
    if (header) lines.push('}');

  } else {
    // ── Notation fontdata.c 16x16 ────────────────────────
    // void _initFont16D(void)
    // {
    //     static unsigned int r[16];
    //     /* --- A --- */
    //     r[ 0]=0xF800;r[ 1]=0xF800; ...
    //     defineChar16(myFont16,'A',r);
    // }
    if (header) {
      lines.push('void _initFont16D(void)');
      lines.push('{');
      lines.push('    static unsigned int r[16];');
    }
    for (const c of chars) {
      const g  = font[c];
      const cl = charLiteral(c);
      const code16 = c.charCodeAt(0);
      const label = code16 < 0x20 ? `0x${code16.toString(16).toUpperCase().padStart(2,'0')}`
                  : c === ' ' ? 'Espace'
                  : code16 === 0x7F ? '0x7F'
                  : c;
      lines.push('');
      lines.push(`    /* --- ${label} --- */`);
      // 4 valeurs uint16 par ligne (r[i] = octet_hi<<8 | octet_lo)
      for (let row = 0; row < 16; row++) {
        const hi = g[row * 2];
        const lo = g[row * 2 + 1];
        const w  = ((hi << 8) | lo) & 0xFFFF;
        const idx = row.toString().padStart(2, ' ');
        // 4 assignations par ligne, alignées comme dans fontdata.c
        const entry = `r[${idx}]=${hex16(w)};`;
        if (row % 4 === 0) {
          // Début d'un groupe de 4 : on accumule
          lines.push('    ' + entry);
        } else {
          // Ajouter sur la même ligne que la précédente
          lines[lines.length - 1] += entry;
        }
      }
      lines.push(`    defineChar16(myFont16,${cl},r);`);
    }
    if (header) lines.push('}');
  }

  return lines.join('\n');
}

function generateCodeHTML(chars) {
  const code = generateCode(chars);
  // Échapper le HTML d'abord
  const escaped = code
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;');
  return escaped
    .replace(/\/\*[^*]*\*+(?:[^/*][^*]*\*+)*\//g, m => `<span class="code-cm">${m}</span>`)
    .replace(/\b(void|return|static|unsigned|int)\b/g, m => `<span class="code-kw">${m}</span>`)
    .replace(/\b(_initFont8D|_initFont16D|defineChar8|defineChar16)\b/g, m => `<span class="code-fn">${m}</span>`)
    .replace(/\b(myFont8|myFont16)\b/g, m => `<span class="code-str">${m}</span>`)
    .replace(/0x[0-9A-Fa-f]+/g, m => `<span class="code-num">${m}</span>`)
    .replace(/'(?:\\.|[^'\\])'/g, m => `<span class="code-str">${m}</span>`);
}

function updateStats() {
  const defined = CHARS.filter(c => hasData(c)).length;
  const total   = CHARS.length;
  const bits    = Object.values(font).flat().reduce((s,b) => s + b.toString(2).split('1').length-1, 0);
  const bytesPerGlyph = MODE === 8 ? 8 : 32;
  document.getElementById('stats').innerHTML =
    `Mode actif        : <strong style="color:var(--sel-light)">${MODE}×${MODE}</strong><br>
     Glyphes définis   : <strong style="color:var(--green)">${defined}</strong> / ${total}<br>
     Glyphes vides     : <strong>${total - defined}</strong><br>
     Pixels allumés    : <strong style="color:var(--yellow)">${bits}</strong> au total<br>
     Taille C estimée  : ~${defined * bytesPerGlyph * 3} octets`;
}

// ═══════════════════════════════════════════════════════════
// EXPORT / IMPORT JSON
// ═══════════════════════════════════════════════════════════

function flashBtn(id) {
  const btn = document.getElementById(id);
  if (!btn) return;
  btn.classList.add('success');
  setTimeout(() => btn.classList.remove('success'), 2000);
}

function setStatus(msg, color='var(--green)') {
  const el = document.getElementById('status-msg');
  el.textContent = msg;
  el.style.color = color;
  setTimeout(() => { el.textContent = 'Prêt.'; el.style.color = 'var(--dim)'; }, 3000);
}

function exportC() {
  const code = generateCode(CHARS);
  navigator.clipboard.writeText(code).then(() => {
    flashBtn('btn-export-all');
    setStatus('✓ 128 caractères copiés dans le presse-papiers !');
  });
}

function exportGlyph() {
  const code = generateCode([current], false).trim();
  navigator.clipboard.writeText(code).then(() => {
    flashBtn('btn-export-glyph');
    setStatus(`✓ Glyphe '${current}' copié !`);
  });
}

function exportDefined() {
  const defined = CHARS.filter(c => hasData(c));
  const code = generateCode(defined);
  navigator.clipboard.writeText(code).then(() => {
    flashBtn('btn-export-defined');
    setStatus(`✓ ${defined.length} glyphes définis copiés !`);
  });
}

function saveJSON() {
  // Sauvegarder les deux modes dans le JSON
  const data = { mode: MODE, font8: {}, font16: {} };
  for (const c of CHARS) { data.font8[c] = font8[c]; data.font16[c] = font16[c]; }
  const json = JSON.stringify(data, null, 2);
  const blob = new Blob([json], {type: 'application/json'});
  const a = document.createElement('a');
  a.href = URL.createObjectURL(blob);
  a.download = `mafont_${MODE}x${MODE}.fnt`;
  a.click();
  setStatus(`✓ Sauvegardé : mafont_${MODE}x${MODE}.fnt`);
}

function loadJSON(event) {
  const file = event.target.files[0];
  if (!file) return;
  const reader = new FileReader();
  reader.onload = e => {
    try {
      const data = JSON.parse(e.target.result);
      // Nouveau format avec les deux modes
      if (data.font8 && data.font16) {
        for (const c of CHARS) {
          if (data.font8[c])  font8[c]  = data.font8[c].slice(0,8);
          if (data.font16[c]) font16[c] = data.font16[c].slice(0,32);
        }
        if (data.mode) setMode(data.mode);
      } else {
        // Ancien format : 8 octets seulement → charger en 8x8
        for (const c of CHARS)
          if (data[c]) font8[c] = data[c].slice(0,8);
        setMode(8);
      }
      updateAll();
      setStatus(`✓ Chargé : ${file.name}`);
    } catch(err) {
      setStatus('✗ Erreur de lecture JSON', 'var(--red)');
    }
  };
  reader.readAsText(file);
}

function importFromTextarea() {
  try {
    const data = JSON.parse(document.getElementById('json-io').value);
    if (data.font8 && data.font16) {
      for (const c of CHARS) {
        if (data.font8[c])  font8[c]  = data.font8[c].slice(0,8);
        if (data.font16[c]) font16[c] = data.font16[c].slice(0,32);
      }
      if (data.mode) setMode(data.mode);
    } else {
      for (const c of CHARS)
        if (data[c]) font[c] = data[c].slice(0, MODE === 8 ? 8 : 32);
    }
    updateAll();
    setStatus('✓ Police importée depuis la zone texte');
  } catch(e) {
    setStatus('✗ JSON invalide', 'var(--red)');
  }
}

function applyHex() {
  const raw   = document.getElementById('hex-input').value.trim();
  const parts = raw.split(/[\s,]+/).filter(Boolean);
  const expected = MODE === 8 ? 8 : 32;
  if (parts.length !== expected) {
    setStatus(`✗ Entrez exactement ${expected} valeurs hex`, 'var(--red)');
    return;
  }
  const bytes = parts.map(p => parseInt(p, 16));
  if (bytes.some(isNaN)) {
    setStatus('✗ Valeurs hexadécimales invalides', 'var(--red)');
    return;
  }
  font[current] = bytes;
  updateAll();
  setStatus(`✓ Glyphe '${current}' mis à jour`);
}

// ═══════════════════════════════════════════════════════════
// EXPORT PNG
// ═══════════════════════════════════════════════════════════

function exportFontPng() {
  const scale = parseInt(document.getElementById('png-export-scale').value) || 2;
  const cols  = 16;
  const rows  = Math.ceil(CHARS.length / cols); // 8 lignes pour 128 chars
  const glyph = MODE;
  const canvas = document.createElement('canvas');
  canvas.width  = cols * glyph * scale;
  canvas.height = rows * glyph * scale;
  const ctx = canvas.getContext('2d');

  // Fond noir
  ctx.fillStyle = '#000000';
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  // Dessiner chaque glyphe
  ctx.fillStyle = '#ffffff';
  CHARS.forEach((c, i) => {
    const col = i % cols;
    const row = Math.floor(i / cols);
    const ox  = col * glyph * scale;
    const oy  = row * glyph * scale;
    const g   = font[c] || new Array(glyph === 8 ? 8 : 32).fill(0);
    for (let r = 0; r < glyph; r++) {
      for (let bc = 0; bc < glyph; bc++) {
        let on;
        if (glyph === 8) {
          on = !!(g[r] & (1 << (7 - bc)));
        } else {
          const byteIdx = r * 2 + (bc < 8 ? 0 : 1);
          const bitPos  = 7 - (bc % 8);
          on = !!(g[byteIdx] & (1 << bitPos));
        }
        if (on) ctx.fillRect(ox + bc * scale, oy + r * scale, scale, scale);
      }
    }
  });

  // Télécharger
  const a = document.createElement('a');
  a.href     = canvas.toDataURL('image/png');
  a.download = `mafont_${glyph}x${glyph}_x${scale}.png`;
  a.click();
  setStatus(`✓ PNG exporté : mafont_${glyph}x${glyph}_x${scale}.png (${canvas.width}×${canvas.height}px)`);
}


// ═══════════════════════════════════════════════════════════
// RACCOURCIS CLAVIER
// ═══════════════════════════════════════════════════════════

document.addEventListener('keydown', e => {
  if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA') return;

  const idx = CHARS.indexOf(current);
  if (e.key === 'ArrowRight' && idx < CHARS.length-1) { selectChar(CHARS[idx+1]); return; }
  if (e.key === 'ArrowLeft'  && idx > 0)              { selectChar(CHARS[idx-1]); return; }
  if (e.key === 'ArrowDown'  && idx+16 < CHARS.length){ selectChar(CHARS[idx+16]); return; }
  if (e.key === 'ArrowUp'    && idx-16 >= 0)          { selectChar(CHARS[idx-16]); return; }

  if (e.key === 'w' || e.key === 'W') { shiftUp();    return; }
  if (e.key === 's' || e.key === 'S') { shiftDown();  return; }
  if (e.key === 'a' || e.key === 'A') { shiftLeft();  return; }
  if (e.key === 'd' || e.key === 'D') { shiftRight(); return; }
  if (e.key === 'h' || e.key === 'H') { flipH();      return; }
  if (e.key === 'v' || e.key === 'V') { flipV();      return; }
  if (e.key === 'i' || e.key === 'I') { invert();     return; }
  if (e.key === 'Delete' || e.key === 'Backspace') { clearGlyph(); return; }
  if ((e.ctrlKey || e.metaKey) && e.key === 'c') {
    e.preventDefault(); exportGlyph();
  }
});

// ═══════════════════════════════════════════════════════════
// INIT
// ═══════════════════════════════════════════════════════════

rebuildAll();
updateAll();
