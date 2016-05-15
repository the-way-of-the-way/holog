#!/usr/bin/env bash

# TODO : Fix paths
RUN_DIR=$(pwd);
SOURCE_DIR=$RUN_DIR;
BUILD_DIR=$RUN_DIR/build;

rm -rf "$SOURCE_DIR"/Android
rm -rf "$SOURCE_DIR"/bin
rm -rf "$SOURCE_DIR"/libs
rm -rf "$BUILD_DIR"
