#!/bin/sh

set -ex

BUILD_TYPE='Release'
if [ "$#" -ge 1 ]; then
    if [ "$1" = "--debug" ]; then
        BUILD_TYPE='Debug'
    fi
fi

OLD_PWD="$PWD"
REPO_ROOT=$(git rev-parse --show-toplevel)
cd $REPO_ROOT

mkdir -p build-wasm
cd build-wasm

emcmake cmake .. -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
emmake cmake --build . --target damage.wasm
if [ "${BUILD_TYPE}" = 'Debug' ]; then
    emcc -lembind -o libdamage.js \
        -Wl,--whole-archive src/libdamage.wasm.a -Wl,--no-whole-archive \
        -s EXPORT_NAME='createWasmModule' -s MODULARIZE=1 -s EXPORT_ES6=1 \
        -s NO_DISABLE_EXCEPTION_CATCHING
else
    emcc -O3 -DNDEBUG -lembind -o libdamage.js \
        -Wl,--whole-archive src/libdamage.wasm.a -Wl,--no-whole-archive \
        -s EXPORT_NAME='createWasmModule' -s MODULARIZE=1 -s EXPORT_ES6=1
fi

cp libdamage.js libdamage.wasm "${REPO_ROOT}/scripts/components/wasm"

cd "$OLD_PWD"