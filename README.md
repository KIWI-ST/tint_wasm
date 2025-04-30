# tint_wasm

> build this project

```shell
// native
cmake -B build/native -S. -G "Visual Studio 17 2022"

// emscripten
emcmake cmake -B build/wasm -S. -G Ninja
```