#!/usr/bin/env bash

# TODO : Fix paths
RUN_DIR=$(pwd);
SOURCE_DIR=$RUN_DIR;
BUILD_DIR=$RUN_DIR/build;

sh cmake-clean.sh

mkdir "$SOURCE_DIR"/Android
cp -R -f "$SOURCE_DIR"/assets "$SOURCE_DIR"/Android

cmake -E make_directory "$BUILD_DIR"
cmake -E chdir "$BUILD_DIR" cmake "$SOURCE_DIR" -DANDROID=1 -DURHO3D_LIB_TYPE=SHARED -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/android.toolchain.cmake -DANDROID_NDK=/home/user2/Tools/android-ndk -DURHO3D_HOME=/home/user2/Tools/urho3d-android
cmake --build "$BUILD_DIR" --clean-first -- -j 4

mv -f "$BUILD_DIR"/libs "$SOURCE_DIR"/libs