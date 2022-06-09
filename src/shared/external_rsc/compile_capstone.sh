
WDIR=`pwd`

BUILDDIR=$WDIR/build
OUTDIR=$WDIR/../external_include/capstone
SRCDIR=$WDIR/capstone-next

rm -rdf $BUILDDIR

mkdir $BUILDDIR
mkdir $OUTDIR
cd $BUILDDIR
cmake -DCMAKE_INSTALL_PREFIX=$OUTDIR $SRCDIR
cmake --build . --config Release  --target install
