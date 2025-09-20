# The matrix effect

A small program to recreate in your shell the Matrix effect.

## Installation

Compile by running `build.sh`:

```
./build.sh
```

If it doesn't work, make the script executable and retry:

```
chmod +x build.sh
./build.sh
```

## Usage

### Parameters

The program takes 2 parameters:

- **Width**: number of characters per row. If you exceed your terminal width, the effect may break — try lowering the width.
- **Height**: number of characters per column.

### Command

```
matrix 100 60
```

If you omit the parameters, the program will choose random values between 0 and the `MATRIX` macro (configurable in `matrix.c`):

```
matrix
```

_Quick note: this program was meant as a built-in for a minishell. The `rand()` function was not allowed there, so I implemented a workaround to generate pseudo-random values._
