# 2D Graphics Editor in C

A terminal-based 2D Graphics Editor written in C that renders shapes on a character grid canvas.

## Features

- Draw and manage 4 shape types:
  - **Line** – using Bresenham's Line Algorithm
  - **Rectangle** – outline drawing
  - **Circle** – using Midpoint Circle Algorithm
  - **Triangle** – three connected lines
- Add, Delete, and Modify shapes
- Two UI modes:
  - **CLI Mode** – standard terminal menu
  - **ncurses Mode** – split-screen interactive UI with colors

## Files

| File | Description |
|------|-------------|
| `graphics_editor.c` | Main source code |
| `Makefile` | Build configuration |

## How to Build and Run

### CLI Mode (default)
```bash
make
./graphics_editor
```

### ncurses Mode
```bash
make ncurses
./graphics_editor_ncurses
```

## How to Use

### CLI Mode
- Choose from the menu: Add / Delete / Modify / Display / List / Exit
- Enter shape coordinates when prompted

### ncurses Mode
| Key | Action |
|-----|--------|
| A | Add a shape |
| D | Delete a shape |
| M | Modify a shape |
| Q | Quit |

## Canvas

- Size: **60 x 20** character grid
- Shapes are drawn using `*` characters
- Background is filled with `_` characters

## Requirements

- GCC compiler
- ncurses library (for ncurses mode only)

## Author

Apoorva  
B.Tech Computer Science, REVA University
