
WDIR=`pwd`/src/shared/external_rsc/

BUILDDIR=$WDIR/build
OUTDIR=$WDIR/../external_include/capstone
SRCDIR=$WDIR/capstone-next

rm -rdf $BUILDDIR
rm -rdf $OUTDIR

mkdir $BUILDDIR
mkdir $OUTDIR

cd $BUILDDIR

cmake -DCMAKE_INSTALL_PREFIX=$OUTDIR $SRCDIR
if [[ ! $? -eq 0 ]]; then
    exit 1
fi

cmake --build . --config Release  --target install
if [[ ! $? -eq 0 ]]; then
    exit 1
fi