# Titon Framework #
[![Build Status](https://travis-ci.org/titon/framework.png)](https://travis-ci.org/titon/framework)
[![Coverage Status](https://coveralls.io/repos/titon/framework/badge.png)](https://coveralls.io/r/titon/framework)
[![Latest Stable Version](https://poser.pugx.org/titon/framework/version.svg)](https://packagist.org/packages/titon/framework)
[![Total Downloads](https://poser.pugx.org/titon/framework/downloads.svg)](https://packagist.org/packages/titon/framework)
[![License](https://poser.pugx.org/titon/framework/license.svg)](https://packagist.org/packages/titon/framework)

## What is Titon? ##

Titon is a full-stack web framework built on Hack that runs in HHVM. It is written with simplicity and efficiency in mind,
with enough room for extensibility and abstraction. The framework takes advantage of Hack's strict mode, type system,
generics, collections, and more. It also integrates the wonderful PSRs from PHP-FIG for increased interoperability
with external PHP code bases.

## Requirements ##

Titon is built on [Hack](http://hacklang.com/) and requires [HHVM 3.3.0+](http://hhvm.com/).

The framework is built around Composer and requires dependencies to be installed for testing.

## Usage ##

Since the project is heavy in development, there currently exists no documentation or installation process.
Feel free to mess around with the code or stay tuned for any updates.

## Contributing ##

Titon is an open source project that accepts any input from you, the community. If you have suggestions for improvement,
create a ticket. If you have code changes to improve effectiveness, submit a pull request. Or any ideas in general,
lets discuss them!

## Testing ##

Titon comes packaged with a Vagrant box and a handful of bash scripts to easy the testing process.
Boot up the Vagrant instance with `vagrant up` and SSH into the box with `vagrant ssh`.
Once in Vagrant, you can run the following command to run all tests.

```bash
bash /vagrant/bin/run-tests.sh
```

Or run tests for a single file or folder.

```bash
bash /vagrant/bin/run-single-test.sh Titon/Common
```

### Hack Strict Linting ###

For the most part, Titon strives to be strict mode only. This can be very challenging, but with Hack's built-in linter,
it couldn't be easier. Simply run the following command to check for strict issues.

```bash
bash /vagrant/lint-hack.sh
```

If any file changes are not reflected in lint results (it's a file system syncing issue),
you will need to either restart Vagrant, or run `hh_client restart`.