# damage-eos

A reimplementation of the damage calculation in _Pokémon Mystery Dungeon: Explorers of Sky_. The goal is to closely reproduce the way the original game's damage calculation code works.

**This project is currently under construction.** It's currently only available as a command-line utility.

If you just want to look at how the code works, the main damage calculation routines are in [`src/damage.cpp`](src/damage.cpp).

## Building
First, clone the repository recursively:
```
git clone --recursive https://github.com/usernamefodder/damage-eos.git
```
If you already cloned the project but forgot to do it recursively, you can instead run:
```
git submodule update --init
```

To build, you'll need to install [CMake](https://cmake.org/install/). Assuming `cmake` is available, navigate to the top-level project directory and run:
```
cmake .
cmake --build . --target damagecalc
```
Once finished, this should place the `damagecalc` command-line utility in your source tree (by default in the `src/` directory).

## Usage
To use `damagecalc`, pass it the path to a JSON configuration file. For more information on output, you can also optionally pass the verbose flag up to three times. For example:
```
damagecalc -i <path/to/config/file> -vvv
```
For an example config file, see [`sample-config.json`](sample-config.json).

Where relevant, the config file works with names rather than internal IDs. For example, `"bulbasaur"` rather than its ID of 1. You can see most of the allowable names for moves, species, items, etc. in [`idmap.cpp`](src/idmap.cpp). Note that with moves and status conditions, not every listed possibility is actually suppported by the damage calculator. There are also a few special names for certain fields:

- You can use the `"guts/marvel scale"` status to indicate any status that would activate Guts or Marvel Scale.
- You can use the following item names in place of a move to indicate a thrown item: `"stick"`, `"iron thorn"`, `"silver spike"`, `"gold fang"`, `"cacnea spike"`, `"corsola twig"`, `"gold thorn"`.
