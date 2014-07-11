#!/usr/bin/env bash

# Use primary script instead of Composer built one
hhvm /vagrant/vendor/phpunit/phpunit/phpunit --bootstrap /vagrant/tests/bootstrap.php --colors /vagrant/tests/$1