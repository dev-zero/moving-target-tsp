#!/bin/sh
sed -i -e '/^\/\/$/a #pragma GCC diagnostic ignored "-Wshadow"' "${1}.cxx" 
