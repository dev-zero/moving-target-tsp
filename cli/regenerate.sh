#!/bin/bash

CLIFILES="exhaustive-search moving-target-tsp"

for f in ${CLIFILES} ; do
    /usr/bin/cli --generate-cxx "${f}.cli"
    sed -i -e '/^\/\/$/a #pragma GCC diagnostic ignored "-Wshadow"' "${f}.cxx"
done

