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

### Current State ###

Many packages have already been ported over from their old PHP codebase to Hack.
However, they have not been linted against Hack's strict mode yet.

The following packages are still in the porting process.

* IO
* Type

The following packages have yet to be ported over.

* [db](https://github.com/titon/db)
* [db-mysql](https://github.com/titon/db-mysql)
* [db-postgresql](https://github.com/titon/db-postgresql)
* [db-sqlite](https://github.com/titon/db-sqlite)
* [db-mongodb](https://github.com/titon/db-mongodb)
* [model](https://github.com/titon/model)
* [mvc](https://github.com/titon/mvc)
* [g11n](https://github.com/titon/g11n)

### Upcoming Packages ###

The following packages have not been written yet but are planned for future releases.

* Auth
* ACL
* Dependency/Service Containers
* Session
* Kernel
* ADR

## Requirements ##

Titon is built on [Hack](http://hacklang.com/) and requires [HHVM 3.3.0+](http://hhvm.com/).
It also requires Composer and requires dependencies to be installed for testing.

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