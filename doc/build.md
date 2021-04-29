# Building

## PSP

```.sh
docker pull zie87/wth_woth_pspdev:latest
docker run --rm -u $UID --init -v $PWD:/opt/workspace -e LANG=$LANG -w /opt/workspace -it zie87/wth_woth_pspdev /bin/sh "./.github/tools/psp/build.sh" Debug
```

## Linux (Qt4)

```.sh
# Arch Linux
sudo pacman -S cmake gcc boost libpng12
paru -S giflib4 qt4 phonon-qt4-gstreamer

mkdir build_qt && cd build_qt
qmake-qt4 ../projects/mtg/wagic-qt.pro CONFIG+=debug
make -j 4(nproc)
```
