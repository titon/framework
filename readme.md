# Titon Framework #
[![Build Status](https://travis-ci.org/titon/framework.png)](https://travis-ci.org/titon/framework)
[![Latest Stable Version](https://poser.pugx.org/titon/framework/version.svg)](https://packagist.org/packages/titon/framework)
[![Total Downloads](https://poser.pugx.org/titon/framework/downloads.svg)](https://packagist.org/packages/titon/framework)
[![License](https://poser.pugx.org/titon/framework/license.svg)](https://packagist.org/packages/titon/framework)

## What is Titon? ##

Titon is a full-stack web framework built on Hack that runs in HHVM. It is written with simplicity and efficiency in mind,
with enough room for extensibility and abstraction. The framework takes advantage of Hack's strict mode, type system,
generics, collections, and more. It also integrates the wonderful PSRs from PHP-FIG for increased interoperability
with external PHP code bases and libraries.

Looking for more information? Find us in [#titon](http://webchat.freenode.net/?channels=titon) on FreeNode.

#### Unported Packages ####

* [db](https://github.com/titon/db)
* [db-mysql](https://github.com/titon/db-mysql)
* [db-postgresql](https://github.com/titon/db-postgresql)
* [db-sqlite](https://github.com/titon/db-sqlite)
* [db-mongodb](https://github.com/titon/db-mongodb)
* [model](https://github.com/titon/model)
* [mvc](https://github.com/titon/mvc)
* [g11n](https://github.com/titon/g11n)

#### Upcoming Packages ####

* Auth
* ACL
* Dependency/Service Containers
* Session
* Kernel
* ADR
* Mail
* Queue

#### Type Checked Packages ####

The following packages have been type checked against Hack's strict type checker.
They will still fail the checker with unbound errors, or mixed type warnings,
but there's nothing we can do until HHVM and Hack are patched.

* Cache
* Common
* Controller
* Debug
* Environment
* Event
* HTTP
* Io
* Route
* Type
* Utility
* View

## Roadmap ##

Upcoming features and release plan.

* December
    * Alpha release
* January
    * Context package - Dependency containers and service locators
* February
    * Kernel package - Application foundation
    * MVC package - Model-view-controller application
* March
    * G11N package - Internationalization and localization
* April - May
    * DB package - Database abstraction layer
* June
    * Model package - Object relational mapper and active record for DB entities
    * Beta release
* July
    * Session package - Custom session handler
* August
    * Queue package - Queue, job, and worker services
* September
    * Mail package - Emailing layer
* October
    * Auth package - Authentication and authorization
* November
    * ACL package - Access control lists
* December
    * ADR package - Action-domain-responder application
    * 0.1.0 release

## Requirements ##

Titon is built on the powerful [Hack language](http://hacklang.org/) and requires [HHVM 3.4.0+](http://hhvm.com/) 
as well as [Composer](http://getcomposer.org) for its autoloader.

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

#### Hack Strict Mode ####

For the most part, Titon strives to be strict mode only. This can be very challenging, but with Hack's built-in type checker,
it couldn't be easier. Simply run the following command to check for strict issues.

```bash
/vagrant/bin/type-check
```

Or type check a specific folder.

```bash
/vagrant/bin/type-check --path=Titon/Utility
```

When filtering down by path, multiple errors will still arise like "Was expecting a class", "Unbound name",
"Unbound global constant", "This is not a container, this is an object of type X", etc. The main cause of this issue 
is that the type checker is ran in a sub-folder, and not the root, so all those classes are not in scope. 
This should not be an issue if the type checker is ran in the source root.
