#!/usr/bin/env bash
# Install the redis module from source
# https://github.com/nicolasff/phpredis

# Download the source
git clone git://github.com/nicolasff/phpredis.git
cd phpredis

# Build it
phpize
./configure
make

# Install it
sudo make install