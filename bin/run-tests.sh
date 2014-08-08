#!/usr/bin/env bash

# Use primary script instead of Composer built one
hhvm /vagrant/vendor/phpunit/phpunit/phpunit -c /vagrant/phpunit.xml --debug -v --stop-on-failure

#--coverage-clover /vagrant/build/logs/clover.xml