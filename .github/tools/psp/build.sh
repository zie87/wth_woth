#!/bin/bash

echo PSPDEV = $PSPDEV
echo psp-config = `psp-config --psp-prefix`

cd JGE
make -f Makefile.hge -j $(nproc)
cp libhgetools.a lib/psp/libhgetools.a
cd ..

cd  JGE
if [ $1 == "debug" ]; then
    make -f Makefile debug -j $(nproc)
else
    make -f Makefile -j $(nproc)
fi
cd ..

cd projects/mtg
mkdir -p objs deps
if [ $1 == "debug" ]; then
    make -f Makefile debug -j $(nproc)
else
    make -f Makefile -j $(nproc)
fi
cd -


