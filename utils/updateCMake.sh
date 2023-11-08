#!/bin/bash
source utils/colorPrint.sh

ROOT=$(pwd)
SRC=${ROOT}/src
CMAKE=${ROOT}/CMakeLists.txt

for CPP in ${SRC}/*.cpp; do
    CPP=$(basename ${CPP})
    if [[ $(grep -E ${CPP} ${CMAKE}) ]]; then
        cecho WAR "${CPP} file already added as executable..."
        continue
    fi
    sed -i "/^add_executable/ s/$/\n\tsrc\/${CPP}/" ${CMAKE}
    cecho SUC "${CPP} added as executable in CMakeLists.txt!"
done