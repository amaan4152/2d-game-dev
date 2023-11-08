#!/bin/bash

# --- colors --- #
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m'

function cecho {
    local code=$1
    local mssg=$2
    case $code in
        "SUC")
            echo -e "[${GREEN}SUCCESS${NC}]: ${mssg}"
            ;;
        "WAR")
            echo -e "[${YELLOW}WARNING${NC}]: ${mssg}"
            ;;
        "ERR")
            echo -e "[${RED}ERROR${NC}]: ${mssg}"
    esac
}