#!/bin/bash

set -e



SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


##if [ "$(pwd)" == "$SCRIPT_DIR" ]; then
##    echo "Building inside source code is forbidden. Change current working directory"
##    exit 1
##fi


scan-build cmake $SCRIPT_DIR

scan-build -V make -j4

