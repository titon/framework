# Conventions #

The following conventions must be followed when contributing to the framework.

## PSR-1 ##

Titon follows the official [PSR-1 Basic Coding Standard](http://www.php-fig.org/psr/psr-1/) with the following additions.

* Folder paths must end in a trailing slash.
* All docblock sentences must end in a period.

## PSR-2 ##

Titon follows the majority of the official [PSR-2 Style Guide](http://www.php-fig.org/psr/psr-1/) with the following exceptions.

* Opening bracket `{` must be on the same line instead of a new line for all instances.
* There must be a single new line between every method, property, and class declaration.
* The `elseif` is written as `else if`.
* Properties and methods must be in alphabetical order.
* Properties and methods must be in order of public, protected, private.

## PSR-4 ##

All packages follow the official [PSR-4 Autoloader Standard](http://www.php-fig.org/psr/psr-4/). 
Each package must have a `composer.json` within the root of the package folder.

## Namespace Organization ##

The term "class" refers to classes, interfaces, traits, and other similar structures.

When declaring a class, it must be organized into the following namespace structure `Titon(\<PackageName>)*\<ClassName>`. All implementations of an interface (including an optional abstract class) must be organized in a sub-folder that matches the interface name.

Say we have the following setup. A class named `Foo`, an interface named `Bar`, its implementations, and a few traits. We would have the following organization.

```
Titon\Package\Bar               // interface
Titon\Package\Bar\AbstractBar   // abstract class
Titon\Package\Bar\PrimaryBar    // class
Titon\Package\Bar\SecondaryBar  // class
Titon\Package\Bazable           // trait
Titon\Package\Foo               // class
Titon\Package\FooAware          // trait
```

The exception to this interface sub-folder organization rule is when a package is based solely around a single interface, usually one that shares the same name. An example of this is the controller and view packages. This exception exists primarily to stop duplicate folder names, like `Titon\Controller\Controller\AbstractController`.

## Naming Conventions ##

### Interfaces ###

* Must *not* be suffixed with `Interface`.
* Must be written as a noun (a thing) or as an adjective (suffixed with `-er`, `-able`, or `-ible`). For example, `Engine`, `Controller`, `Helper`, `Subject`, `Arrayable`, etc.

### Traits ###

* Must be suffixed with `Aware` if the trait provides a getter and a setter for a class (or interface). For example, `StorageAware`.
* Must be suffixed with `Mixin` if the trait provides a set of encapsulated reusable methods or properties. For example, `FilterMixin`. If there are multiple mixins in a package, they should be grouped into a `Mixin` namespace.
* All other instances must be written as an adjective (suffixed with `-able` or `-ible`) or as a verb that conveys an action. For example, `Mutable`, `EmitsEvents`, or `WiresAnnotations`.

### Methods ###

* Must be written as a verb that conveys an action. For example, `getValue()`, `setValue()`, `hasItem()`, `removeMember()`, `flushContents()`, etc.

The following actions should be used within the correct naming context, but are not mandatory.

* `get` - Get a value.
* `set` - Set a value. Only applicable if one possible value.
* `add` - Set a value to a collection or add multiple values.
* `has` - Check if a value exists or a key within a collection exists.
* `is` - Check if a state is truthy.
* `remove` - Remove a value.
* `flush` - Remove all values.
* `parse` - Extract values.
* `build` - Combine values.
* `run` - Process a value.
* `do` - Special case for event callbacks.

### Properties ###

* Must be written in camel case.
* Must use the protected visibility modifier (unless there's a good reason otherwise). 
* Must use a type alias for all non-primitive type hints.

## Type Hints ##

All properties and arguments must have type hints and all methods and functions must have return type hints.

Nullable types should be used sparingly. 

## Type Aliases ##

All type aliases must be declared in StudlyCaps (similar to classes) within the current namespace. 
The declaration must reside in the `bootstrap.hh` file found at the root of the package; 
opaque type aliases are except from this rule as they must be declared in the class file that uses them.

```hack
namespace Titon\Package {
    type FooBar = Map<string, mixed>;
}
```

When importing the type alias, the full declaration must be used.

```hack
use Titon\Package\FooBar;
```

And when type hinting with the alias, the alias name must be used.

```hack
public function doAction(FooBar $map): void {}
```

When naming a type alias, the following suffixes and conventions must be adhered to.

* Must use `Callback` suffix when declaring a callable.
* Must use `Container` suffix when declaring a map that contains another map, vector, set, pair, or array.
* Must use `List` suffix when declaring a vector.
* Must use `Map` suffix when declaring a map.
* Must use nouns (a thing) when declaring a shape.

## Exceptions ##

All packages should have an `Exception` folder and namespace.

## Global Functions ##

All global functions must be declared in the `bootstrap.hh` file found at the root of the package. 
Function names should be written in snake case (lowercase and underscores).

```hack
namespace {
    function do_action(): string {}
}
```
