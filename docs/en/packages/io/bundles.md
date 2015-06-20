# Bundles #

A bundle is a mechanism for bundling, organizing and reading multiple catalog files from 
multiple domains. A catalog can be anything from configuration files to localization messages.
The built-in `Titon\Io\Bundle\ResourceBundle` can handle the most basic of use cases.

```hack
$bundle = new Titon\Io\Bundle\ResourceBundle();
```

## Mapping Domains & Catalogs ##

In the context of a bundle, a catalog is a resource file that contains information to be read, while a domain 
is a grouping and a location where the catalog can be found. 

Let's create a basic filesystem outline that will be used throughout the rest of this documentation.

```
cms/
├── foo/
|   └── messages.php
├── bar/
|   └── rules.php
└── baz/
|   └── rules.php
blog/
├── foo/
|   └── messages.php
└── baz/
|   └── validations.php
forum/
└── foo/
    ├── messages.php
    └── validations.php
```

In our outline, the domains are `foo`, `bar`, and `baz`, while our catalogs are `messages.php`, `rules.php`, 
and `validations.php`. The `cms`, `blog`, and `forum` folders are simply locations on the filesystem where 
our domains can be found.

To make use of domains and catalogs, they need to be mapped in the bundle. We can do this using `addPath()`, 
which requires a unique domain name and the path to the domain folder.

```hack
$bundle->addPath('foo', 'cms/foo/');
```

We can also add multiple paths using `addPaths()`.

```hack
$bundle->addPaths('foo', Set {'blog/foo/', 'forum/foo/'});
```

Catalog files should only contain data that can be converted into a map. For example, 
a PHP file should return an array of key-value pairs.

## Using Readers ##

A [reader](readers.md) is a specialized `Titon\Io\File` class that handles the reading of a specific file format, 
which in our case is a catalog file. Using our example above, we will need a `Titon\Io\Reader\PhpReader` 
as our catalog files are PHP.

Let's begin by adding a reader using `addReader()`.

```hack
$bundle->addReader(new Titon\Io\Reader\PhpReader());
```

Bundles support [multiple types of readers](readers.md), all of which will work in unison when 
reading catalog files with the same name.

## Loading Resources ##

Once our domains are mapped and our readers set, we can start loading resources with `loadResource()`. 
This method requires a domain name (mapped previously) and a catalog name (without extension).

```hack
$resource = $bundle->loadResource('foo', 'messages');
```

What this method does internally is loop over every domain path that has been mapped, 
and loop over every reader that has been added, and attempt to read a catalog at the built location. 
The location in question is the domain path + catalog name + reader extension. If a catalog is found, 
its contents will be read and merged with any other catalog that has been found during the lookup process. 
This allows multiple sets of resources to override and integrate with each other.

Using our outline above and the method call previously, the following catalogs would be loaded: 
`cms/foo/messages.php`, `blog/foo/messages.php`, and `forum/foo/messages.php`. If we use the catalog `validations` 
instead, then the following would be loaded: `forum/foo/validations.php`.
