#!/usr/bin/env bash

# TODO : Fix paths
RUN_DIR=$(pwd);
SOURCE_DIR=$RUN_DIR;
BUILD_DIR=$RUN_DIR/build;

sh cmake-clean.sh

cmake -E make_directory "$BUILD_DIR"
cmake -E chdir "$BUILD_DIR" cmake "$SOURCE_DIR"
cmake --build "$BUILD_DIR" --clean-first -- -j 4

mv -f "$BUILD_DIR"/bin "$SOURCE_DIR"/bin
cp -R -f "$SOURCE_DIR"/assets/* "$SOURCE_DIR"/bin

#chmod +x "$SOURCE_DIR"/bin/HologNativeCore
touch "$SOURCE_DIR"/bin/HologNativeCore