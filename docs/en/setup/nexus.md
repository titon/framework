# Nexus #

[Titon Nexus](https://github.com/titon/nexus) is an official Vagrant box that provides an easy to use development environment 
that comes pre-packaged for HHVM, Hack, and PHP development. It bundles a built-in web server, 
popular databases, packaging tools, and more.

The Nexus acts as a central hub for *all* your projects and aims to replace the individual `Vagrantfile` per project scenario. 
It's bundled with a built-in command line tool that aids in the management of projects, databases, Vagrant, and more.

## Requirements ##

* Vagrant 1.7
* VirtualBox 4.3
* Ruby 1.9

## Specifications ##

* Ubuntu 14.04
* HHVM Nightly 3.5.0 (PHP 5.6)
* Composer 1.0
* Nginx 1.6.2
* MariaDB 10.0.15 (MySQL)
* PostgreSQL 9.3.5
* Node.js 0.10.25
* NPM 1.3.10
* Redis 2.8.4
* Memcache 1.4.14

## Upcoming Features ##

* SSH key management
* Bash alias linking
* More database engines
* VMWare support

## Installation ##

Clone the repository to your local machine, preferably in a location where your projects reside, like `~/Sites/`.

```bash
git clone git@github.com:titon/nexus.git
```

Move into the nexus directory and install the gems required by the command line tool.

```bash
cd nexus/
bundle
```

If you don't have Bundler installed, you can install the gems directly.

```bash
gem install 'escort'
gem install 'table_print'
gem install 'win32console' # If you are on Windows
```

Initialize the Nexus environment.

```bash
nexus init
```

If you are on Windows, you may need to pipe the command through ruby to get it working.

```bash
ruby nexus init
```

Before you boot up and provision Vagrant for the first time, the Nexus environment will need to be configured. 
Documentation on how to use the Nexus command line tool and how to configure all the settings can be found below. 
Help menus can also be accessed from the CLI by passing `--help`.

Once Nexus is configured, boot up Vagrant and wait for it to provision.

```bash
nexus up
```

Once complete, direct your browser to `192.168.13.37`. If everything worked correctly, you should see the following splash screen.
This splash screen will display the current version for each installed technology.

![Nexus](https://s3.amazonaws.com/titon/nexus/splash.png)

## Usage ##

All configuration is stored in the `.nexus/` folder within the repository checkout.

### Projects ###

A project is a website, or library, or simply a folder that will be synced into Vagrant. 
Once a project is synced, it will be available through nginx.

To add a project, use `nexus project add`. The project's source directory should be passed as the 1st argument.

```bash
nexus project add ~/Sites/FooBar/
```

This command will sync your source directory to the Vagrant `/home/vagrant/FooBar` target directory. 
To change the target directory name, pass a 2nd argument to the command.

```bash
nexus project add ~/Sites/FooBar/ foo-bar
```

You can customize the nginx hostname by passing a `--hostname` option to the command. 
If no option is passed, it will fallback to the target directory name + `.app`.

```bash
nexus project add ~/Sites/FooBar/ --hostname=foobar.app
```

If the root of the source directory is not the nginx public webroot, you can pass a relative path to the `--webroot` option.

```bash
nexus project add ~/Sites/FooBar/ --hostname=foobar.app --webroot=public/
```

To list all defined projects, use `nexus project list`.

```bash
nexus project list
```

To delete a project, use `nexus project delete` and pass the hostname as its argument. 
If you do not know the hostname, use the list command above to find it.

```bash
nexus project delete foobar.app
```

### Environment Variables ###

Environment variables are values that are available through `getenv()` within your PHP or Hack scripts. 
They are made available through HHVM's FastCGI layer.

To add or update a variable, use `nexus var add`. The variable key should be passed as the 1st argument, and the value the 2nd.
The `APP_ENV` variable is defined as `local` by default.

```bash
nexus var add APP_ENV local
```

To list all variables, use `nexus var list`.

```bash
nexus var list
```

To delete a variable, use `nexus var delete` and pass the variable key as its argument.

```bash
nexus var delete APP_ENV
```

### Databases ###

A database is simply that, a database. You can configure a list of databases to be automatically created for 
MySQL and PostgreSQL. 

To create a database, use `nexus db add` and pass the database name. Either the `--mysql` or `--pgsql` option 
should be passed, depending on which engine you want the database created in.

```bash
nexus db add foo --mysql
```

To list all databases, use `nexus db list`.

```bash
nexus db list
```

To delete a database, use `nexus db delete` and pass the database name as its argument. 
Please note that this does not actually delete the database from MySQL or PostgreSQL, it simply removes the configuration.

```bash
nexus db delete foo
```

### Importing & Exporting ###

There may be situations where you need to reuse a Nexus configuration across multiple users or computers. 
This can easily be achieved by importing and exporting the configuration files.

To export the current configuration, use `nexus export`. This will export all configuration files to `~/.nexus`.

```bash
nexus export
```

To import any configuration files found in `~/.nexus`, use `nexus import`.

```bash
nexus import
```

Both of these commands will overwrite existing files, so be careful.

### Provisioning ###

Once configuration has been modified, the Vagrant box will need to be provisioned. 
If it's the first time Vagrant is booted, use `nexus up`, else use `nexus reload`.

```bash
nexus up
```

The reload command is equivalent to `vagrant reload --provision`.

```bash
nexus reload
```

#### Before & After Provisions ####

To hook into the provisioning process, there are two scripts found in `.nexus/` that can be modified. 
They are `after-provision.sh` and `before-provision.sh`.

Both of these scripts will be ran with `bash` when Vagrant is provisioned. They will also be exported and imported.
