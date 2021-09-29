FROM devkitpro/toolchain-base

MAINTAINER Dave Murphy <davem@devkitpro.org>

RUN dkp-pacman -Syyu --noconfirm wii-dev wiiu-dev && \
    dkp-pacman -S --needed --noconfirm ppc-portlibs wii-portlibs wiiu-portlibs && \
    dkp-pacman -Scc --noconfirm
ENV DEVKITPPC=${DEVKITPRO}/devkitPPC

RUN dkp-pacman -Syyu --noconfirm 3ds-dev && \
    dkp-pacman -S --needed --noconfirm 3ds-portlibs && \
    dkp-pacman -Scc --noconfirm
ENV DEVKITARM=${DEVKITPRO}/devkitARM


