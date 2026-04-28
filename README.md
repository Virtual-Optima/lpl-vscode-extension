# LPL Language Support

Syntax highlighting for the **LPL** (Linear Programming Language) mathematical modeling language in Visual Studio Code.

## Features

- Syntax highlighting for `.lpl` and `.inc` files
- Comment toggling (`//`, `--`, `/* */`)
- Bracket matching and auto-closing for `()`, `[]`, `{}`, `''`
- Code folding on `model`/`end` blocks

## Highlighted Elements

- **Keywords**: `model`, `end`, `variable`, `constraint`, `parameter`, `set`, `maximize`, `minimize`, `solve`, etc.
- **Type modifiers**: `binary`, `integer`, `string`, `alldiff`
- **Control flow**: `if`, `then`, `else`, `for`, `while`, `do`
- **Logical operators**: `and`, `or`, `not`, `nand`, `in`, `exist`, `forall`
- **Built-in functions**: `Write`, `Read`, `sum`, `Abs`, `Floor`, `Ceil`, `Rnd`, `SetSolver`, `Draw.*`, etc.
- **Operators**: `:=`, `<->`, `->`, `<=`, `>=`, `..`, `~`, `#`, `&`
- **Strings**: Single-quoted with format specifier highlighting
- **Comments**: Line (`//`, `--`) and block (`/* */`, `/** */`)
- **Numbers**: Integer and float literals

## About LPL

LPL is an advanced algebraic modeling language for formulating and solving linear, non-linear, and integer optimization models. Learn more at [matmod.ch](https://matmod.ch).
