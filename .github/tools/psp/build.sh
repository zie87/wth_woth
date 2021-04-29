#!/bin/bash

echo PSPDEV = $PSPDEV
echo psp-config = `psp-config --psp-prefix`

cd JGE
make -f Makefile.hge -j $(nproc)
cp libhgetools.a lib/psp/libhgetools.a
cd ..

cd  JGE
make -f Makefile -j $(nproc)
cd ..

cd projects/mtg
mkdir -p objs deps
make -f Makefile -j $(nproc)
cd -


