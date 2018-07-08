#!/bin/bash

#################################################################
# prerequisites
# * Emscripten
#################################################################

# ==> Select latest version for security reasons
OPENSSL_VERSION="1.1.0h"

MY_HOME="$HOME"

cd "${MY_HOME}/openssl_em" || cd "./openssl_em" || exit 1

if [ ! -f openssl-${OPENSSL_VERSION}.tar.gz ]; then
  wget https://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz
fi

#rm -rf "openssl-${OPENSSL_VERSION}"
#tar xvf "openssl-${OPENSSL_VERSION}.tar.gz"
cd openssl-${OPENSSL_VERSION}

export CC=emcc
export CXX=emcc
export LINK=${CXX}
export ARCH_FLAGS=""
export ARCH_LINK=""
export CPPFLAGS=" ${ARCH_FLAGS} "
export CXXFLAGS=" ${ARCH_FLAGS} "
export CFLAGS=" ${ARCH_FLAGS} "
export LDFLAGS=" ${ARCH_LINK} "

./Configure purify

PATH=$TOOLCHAIN_PATH:$PATH make

#old path :  /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
