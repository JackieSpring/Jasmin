#! /bin/sh

WDIR=`pwd`/src/shared/external_rsc/

BUILDDIR=$WDIR/build
OUTDIR=$WDIR/../external_include/keystone
SRCDIR=$WDIR/keystone-master

rm -rdf $BUILDDIR
rm -rdf $OUTDIR

mkdir $BUILDDIR
mkdir $OUTDIR

cd $SRCDIR
. "$(dirname "$0")"/make-common.sh

cd $BUILDDIR
cmake -DCMAKE_INSTALL_PREFIX=$OUTDIR -DBUILD_LIBS_ONLY=$BUILD_LIBS_ONLY -DLLVM_BUILD_32_BITS="$LLVM_BUILD_32_BITS" -DCMAKE_OSX_ARCHITECTURES="$ARCH" -DCMAKE_BUILD_TYPE=$BUILDTYPE -DBUILD_SHARED_LIBS=OFF -DLLVM_TARGETS_TO_BUILD="all" -G "Unix Makefiles" $SRCDIR
if [[ ! $? -eq 0 ]]; then
    exit 1
fi

make -j8
if [[ ! $? -eq 0 ]]; then
    exit 1
fi

make install
if [[ ! $? -eq 0 ]]; then
    exit 1
fi

cmake --build . --config Release  --target install
if [[ ! $? -eq 0 ]]; then
    exit 1
fi
