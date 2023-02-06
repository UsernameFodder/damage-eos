# damage-eos
[![Deployment](https://github.com/UsernameFodder/damage-eos/actions/workflows/deploy.yml/badge.svg)](https://usernamefodder.github.io/damage-eos/)
[![GitHub](https://img.shields.io/github/license/usernamefodder/damage-eos)](LICENSE.txt)

A reimplementation of the damage calculation in _Pokémon Mystery Dungeon: Explorers of Sky_. The goal is to closely reproduce the way the original game's damage calculation code works.

This calculator is available as an [interactive web application](https://usernamefodder.github.io/damage-eos/). It can also be compiled as a command-line application and as a static library. If you just want to look at how the code works, the main damage calculation routines are in [`src/damage.cpp`](src/damage.cpp).

Note that some things have not yet been implemented. See the [Not Yet Implemented](#not-yet-implemented) section.

## Building
First, clone the repository recursively:
```sh
git clone --recursive https://github.com/usernamefodder/damage-eos.git
```
If you already cloned the project but forgot to do it recursively, you can instead run:
```sh
git submodule update --init
```

To build, you'll need to install [CMake](https://cmake.org/install/). Assuming `cmake` is available, navigate to the top-level project directory and run:
```sh
# If on Windows, use the equivalents to `mkdir` and `cd`
# in your environment, or build with Visual Studio
mkdir build && cd build
cmake ..
cmake --build . --target damagecalc
```
Once finished, this should place the `damagecalc` command-line utility and the `libdamage.a` static library in `build/src`.

### WebAssembly Build
You shouldn't need to build this for WebAssembly; it's done automatically and deployed to a [GitHub Pages site](https://usernamefodder.github.io/damage-eos/) for easy use. However, if you do want to build to Wasm for some reason, just run [`build-wasm.sh`](build-wasm.sh) on a Unix system.

## Command-Line Usage
(Have you considered using the [web application](https://usernamefodder.github.io/damage-eos/)?)

To use `damagecalc`, pass it the path to a JSON configuration file. For more information on output, you can also optionally pass the verbose flag up to three times. For example:
```sh
damagecalc -i <path/to/config/file> -vvv
```
For an example config file, see [`sample-config.json`](sample-config.json).

Where relevant, the config file works with names rather than internal IDs. For example, `"bulbasaur"` rather than its ID of 1. All names are case-insensitive, and some IDs (statuses and exclusive item effects) can even be specified by multiple names. You can see most of the allowable names for moves, species, items, etc. in [`idmap.cpp`](src/idmap.cpp). Note that with moves and status conditions, not every listed possibility is actually suppported by the damage calculator. There are also a few special names for certain fields:

- You can use the `"guts/marvel scale"` status to indicate any status that would activate Guts or Marvel Scale.
- You can use the following item names in place of a move to indicate a thrown item: `"stick"`, `"iron thorn"`, `"silver spike"`, `"gold fang"`, `"cacnea spike"`, `"corsola twig"`, `"gold thorn"`.

### Type and Ability Overrides
The following properties can optionally be specified within the attacker and defender objects: `"type1"`, `"type2"`, `"ability1"`, `"ability2"`. If present, these values will override the normal values determined based on the `"species"` field.

## Not Yet Implemented
- Some special-case moves with unique handling
  - One notable group is all moves using the "fixed damage" routines. This includes moves that are not actually _fixed_ damage, but are implemented using these routines. For example, Psywave.
- Probably other things I'm forgetting
