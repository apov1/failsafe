#!/bin/bash

generateFiles() {
    local WHERE=$1; shift
    local NUM_FILES=$1; shift
    local FILE_SIZE=$1; shift

    for (( i = 0 ; i < $NUM_FILES ; i++ )); do
        dd bs=$FILE_SIZE count=1 if=/dev/urandom of=$(mktemp -p $WHERE -t tmp.XXXXXX) > /dev/null 2>&1
    done
}

createTest() { 
    local WHERE=$1; shift
    local NUM_FILES=$1; shift

    /usr/bin/time -f "%e" bash -c "for (( i = 0 ; i < $NUM_FILES ; i++ )); do touch "$WHERE/$i"; done"
}

copyTest() {
    local WHERE=$1; shift
    local NUM_FILES=$1; shift
    local FILE_SIZE=$1; shift 

    local SRCDIR="$WHERE/SRC_DIR"
    local DSTDIR="$WHERE/DST_DIR"

    mkdir $SRCDIR
    mkdir $DSTDIR
    generateFiles "$WHERE/SRC_DIR" $NUM_FILES $FILE_SIZE

    /usr/bin/time -f "%e" cp $SRCDIR/* $DSTDIR 
}

moveTest() {
    local WHERE=$1; shift
    local NUM_FILES=$1; shift
    local FILE_SIZE=$1; shift 

    local SRCDIR="$WHERE/SRC_DIR"
    local DSTDIR="$WHERE/DST_DIR"

    mkdir $SRCDIR
    mkdir $DSTDIR
    generateFiles "$WHERE/SRC_DIR" $NUM_FILES $FILE_SIZE

    /usr/bin/time -f "%e" mv $SRCDIR/* $DSTDIR 
}

deleteTest() {
    local WHERE=$1; shift
    local NUM_FILES=$1; shift
    local FILE_SIZE=$1; shift 

    generateFiles "$WHERE" $NUM_FILES $FILE_SIZE

    /usr/bin/time -f "%e" rm $WHERE/* 
}

TEST_MODE=$1; shift
TEST_LOCATION=$1; shift
TEST_NUMBER_FILES=$1; shift
TEST_FILESIZE=$1; shift

TEST_DIR=$(mktemp -d -p $TEST_LOCATION -t "temp.XXXXXX")

if [ $TEST_MODE = "create" ]; then
    createTest $TEST_LOCATION/$TEST_DIR $TEST_NUMBER_FILES 
elif [ $TEST_MODE = "copy" ]; then
    copyTest $TEST_LOCATION/$TEST_DIR $TEST_NUMBER_FILES $TEST_FILESIZE
elif [ $TEST_MODE = "move" ]; then
    moveTest $TEST_LOCATION/$TEST_DIR $TEST_NUMBER_FILES $TEST_FILESIZE
elif [ $TEST_MODE = "delete" ]; then
    deleteTest $TEST_LOCATION/$TEST_DIR $TEST_NUMBER_FILES $TEST_FILESIZE
else 
    echo "BAD MODE"
fi

rm -rf $TEST_DIR
