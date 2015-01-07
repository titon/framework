# Installing #

This guide will help get the Titon Framework up and running by installing any required dependencies.

## Requirements ##

The following system dependencies are required.

* [HHVM](http://hhvm.com/)
* [Composer](https://getcomposer.org/)
* Multibyte extension

### Installing Vagrant ###

We suggest using [Vagrant](https://www.vagrantup.com/) for local development. 
You can install Vagrant by downloading one of their official installers.

### Installing HHVM ###

To install HHVM, run the following commands on Ubuntu 14.04. 

```shell
wget -O - http://dl.hhvm.com/conf/hhvm.gpg.key | sudo apt-key add -
echo deb http://dl.hhvm.com/ubuntu trusty main | sudo tee /etc/apt/sources.list.d/hhvm.list
sudo apt-get update
sudo apt-get install hhvm
```

If you need to install HHVM on another distro, or are encountering installation errors, 
[please refer to the official docs](https://github.com/facebook/hhvm/wiki/Prebuilt%20Packages%20for%20HHVM).

### Installing Composer ###

Titon utilizes Composer for managing its dependencies. Depending on your operating system, installation may differ.

<div class="notice is-warning">
    Using Composer will require PHP (or HHVM) to be installed locally on the machine.
</div>

#### Windows ####

Download and run the [official Composer installer](https://getcomposer.org/Composer-Setup.exe).

#### OSX ####

We suggest using [Homebrew](http://brew.sh/) for installation. If this does not work correctly, skip to the next section. 

```shell
brew install composer
```

#### Other ####

If using Linux, or the previous suggestions weren't helpful, download Composer manually via cURL. 

```shell
curl -sS https://getcomposer.org/installer | php
```

If you are still having issues, [please refer to the official docs](https://getcomposer.org/doc/00-intro.md).

## Installation ##

Once the requirements are in place, we can proceed with installing Titon.
The preferred way of using Titon is by installing it as a vendor through Composer.

```shell
composer require titon/framework:*
composer update
```

Titon also comes bundled as individual packages, if you prefer not use the entire framework. 
Check out [Packagist](https://packagist.org/packages/titon/) for a list of supported packages.

```shell
composer require titon/event:*
```