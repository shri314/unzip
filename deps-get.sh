#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(CDPATH= cd "$(dirname "$0")" && pwd)

PKG=zlib-1.3.1

(
    cd "$SCRIPT_DIR"
    rm -rf ./tmp_stage thirdparty
    mkdir -p ./tmp_stage

    (
        cd ./tmp_stage
        curl -O https://www.zlib.net/$PKG.tar.gz
        tar -xzf $PKG.tar.gz

        (
            cd "$PKG"
            ./configure --prefix=$SCRIPT_DIR/thirdparty
            make
            make install
        )
    )
)
