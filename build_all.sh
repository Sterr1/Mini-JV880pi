#!/bin/bash
set -ex

[ -f gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz ] || \
wget -q https://developer.arm.com/-/media/Files/downloads/gnu-a/10.3-2021.07/binrel/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz

tar xf gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz

export PATH="$(readlink -f ./gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf)/bin:$PATH"


git submodule update --init --recursive
sh submod.sh

mkdir -p build
rm -rf build/*

RPI=3 bash build.sh
cp src/*.img build/

RPI=4 bash build.sh
cp src/*.img build/

RPI=5 bash build.sh
cp src/*.img build/

cp src/minijv880.ini build/
cp src/changes.txt build/
cp src/minijv880-configurator.html build/

rm -rf gcc-arm-10.3-2021*

