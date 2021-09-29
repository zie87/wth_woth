FROM alpine:3.12

RUN apk add --no-cache \
             autoconf automake bison bzip2 cmake doxygen diffutils flex \
             gcc g++ git gzip libarchive-dev curl-dev libelf gpgme-dev file\
             libssl1.1 libtool libusb-compat-dev m4 make ncurses-dev patch pkgconf \
             python3 readline-dev subversion tar tcl texinfo unzip wget xz openssh

ENV PSPDEV /opt/pspsdk
ENV PSPSDK $PSPDEV/pspsdk
ENV PATH   $PATH:$PSPDEV/bin:$PSPSDK/bin

#RUN git clone --branch gcc-10.3.0 https://github.com/zie87/psptoolchain.git  /tmp/psptoolchain
RUN git clone --branch gcc-11.2.0 https://github.com/zie87/psptoolchain.git  /tmp/psptoolchain

# work around for musl paths
RUN echo "#include <unistd.h>" > /usr/include/sys/unistd.h

RUN cd /tmp/psptoolchain \
    && ./toolchain.sh \
    && cd .. && rm -Rf /tmp/psptoolchain

RUN git clone https://github.com/pspdev/psplibraries.git /tmp/psplibraries

RUN cd /tmp/psplibraries && ls -l scripts \ 
    && ./libraries.sh \
    && cd .. && rm -Rf psplibraries
