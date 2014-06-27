#!/usr/bin/env bash
# Install the xcache module from source

# Download the source
wget http://xcache.lighttpd.net/pub/Releases/3.0.1/xcache-3.0.1.tar.gz
tar -xzf xcache-3.0.1.tar.gz
cd xcache-3.0.1

# Build it
phpize
./configure --enable-xcache
make

# Install it
sudo make install