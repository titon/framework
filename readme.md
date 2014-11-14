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
with external PHP code bases and libraries.

Looking for more information? Find us in [#titon](http://webchat.freenode.net/?channels=titon) on FreeNode.

### Current State ###

A handful of packages have already been ported over from their old PHP codebase to Hack.
However, they have not been linted against Hack's strict type checker yet.

The following packages are still in the porting process.

* Cache
* HTTP
* IO

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

The following packages are planned for future releases.

* Auth
* ACL
* Dependency/Service Containers
* Session
* Kernel
* ADR
* Mail
* Queue

### Linted Packages ###

The following packages have been linted against Hack's type checker.
They will still fail the checker with unbound errors, or mixed type warnings,
but there's nothing we can do until HHVM and Hack are patched.

* Common
* Controller
* Debug
* Environment
* Event
* Route
* Type
* Utility
* View

## Requirements ##

Titon is built on [Hack](http://hacklang.org/) and requires [HHVM 3.3.0+](http://hhvm.com/).
It also requires Composer for its autoloader and for dependencies to be installed for testing.

## Usage ##

Since the project is heavy in development, there currently exists no documentation or installation process.
Feel free to mess around with the code or stay tuned for any updates.

## Contributing ##

Titon is looking for any knowledgable PHP developers that are looking to get their feet wet in Hack.
Chat with us over in [#titon](http://webchat.freenode.net/?channels=titon) for more information.

## Testing ##

Titon comes packaged with a Vagrant box and a handful of Python scripts to ease the testing process.
Boot up the Vagrant instance with `vagrant up` and SSH into the box with `vagrant ssh`.
Once in Vagrant, you can run the following command to run all tests.

```bash
/vagrant/bin/run-tests
```

Or run tests for a single file or folder.

```bash
/vagrant/bin/run-tests --path=Titon/Common
```

Or filter tests based on class or method name.

```bash
/vagrant/bin/run-tests --filter=methodName
```

### Hack Strict Linting ###

For the most part, Titon strives to be strict mode only. This can be very challenging, but with Hack's built-in linter,
it couldn't be easier. Simply run the following command to check for strict issues.

```bash
/vagrant/bin/lint-hack
```

Or lint a specific folder.

```bash
/vagrant/bin/lint-hack --path=Titon/Utility
```

### PHPUnit Support ###

Titon requires a modified PHPUnit to run tests properly. Simply modify the local file `Framework\TestSuite.php` and change this.

```php
$shortname = basename($filename, '.php');
```

To the following. This will allow `.hh` file extensions to be located.

```php
$shortname = basename(basename($filename, '.hh'), '.php');
```
