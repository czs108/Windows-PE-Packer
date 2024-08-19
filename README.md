# *Windows* *PE* Packer

![C](docs/badges/C-17.svg)
![MASM](docs/badges/MASM-8.svg)
[![CMake](docs/badges/Made-with-CMake.svg)](https://cmake.org)
[![Windows](docs/badges/Microsoft-Windows.svg)](https://www.microsoft.com/en-ie/windows)
![License](docs/badges/License-MIT.svg)

## Languages

- [English](https://github.com/czs108/Windows-PE-Packer/blob/master/README.md)
- [简体中文](https://github.com/czs108/Windows-PE-Packer/blob/master/README-CN.md)

## Introduction

![test-helloworld](docs/screenshots/test-helloworld.png)

***PE-Packer*** is a simple packer for **Windows *PE*** files. The new *PE* file after packing can obstruct the process of reverse engineering.

It will do the following things when packing a *PE* file:

- Transforming the original import table.
- Encrypting sections.
- Clearing section names.
- Installing the *shell-entry*.

When running a packed *PE* file, the *shell-entry* will decrypt and load the original program as follows:

- Decrypting sections.
- Initializing the original import table.
- Relocation.

Before packing, using some disassembly tools can disassemble the executable file to analyze the code, such as [*IDA Pro*](https://www.hex-rays.com/products/ida).

- Disassembling the code.

  ![code](docs/screenshots/code.png)

- Searching constant strings.

  ![string](docs/screenshots/string.png)

- Analyzing the import table.

  ![import-table](docs/screenshots/import-table.png)

After packing, the reverse analysis will be obstructed.

- Disassembling the code.

  ![packed-code](docs/screenshots/packed-code.png)

- Searching constant strings.

  ![packed-string](docs/screenshots/packed-string.png)

- Analyzing the import table.

  ![packed-import-table](docs/screenshots/packed-import-table.png)

### Warning

> This project is just a demo for beginners to study *Windows PE Format* and *Assembly Language*. It still has some compatibility problems and bugs that cannot be used in practice.

## Getting Started

### Prerequisites

The project must configure on/for **Windows 32-bit** and can only process **32-bit** `.exe` programs now.

- Install [*MASM32*](http://www.masm32.com).
- Install [*MinGW-w64*](https://www.mingw-w64.org), select `i686` architecture.
- Install [*CMake*](https://cmake.org).
- Set the `PATH` environment variables of these three tools.

### Building

```bash
mkdir -p build
cd build
cmake .. -D CMAKE_C_COMPILER=gcc -G "MinGW Makefiles"
cmake --build .
```

Or run the `build.ps1` file directly:

```console
PS> .\build.ps1
```

## Usage

To pack a program, you must specify its *input name* and the *output name*.

```console
PE-Packer <input-file> <output-file>
```

For example:

```console
PE-Packer hello.exe hello-pack.exe
```

## Documents

You can use [*Doxygen*](http://www.doxygen.nl) to generate the document.

## References

- [*《加密与解密（第3版）》段钢*](https://book.douban.com/subject/3091212)
- [*PE Format - Windows Dev Center*](https://docs.microsoft.com/en-us/windows/win32/debug/pe-format)

## License

Distributed under the *MIT License*. See `LICENSE` for more information.