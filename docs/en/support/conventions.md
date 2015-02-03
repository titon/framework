# Conventions #

The following conventions must be followed when contributing to the framework.

## PSR-1 ##

Titon follows the official [PSR-1 Basic Coding Standard](http://www.php-fig.org/psr/psr-1/) with the following additions.

* Properties must be declared in camel case.

## PSR-2 ##

Titon follows the majority of the official [PSR-2 Style Guide](http://www.php-fig.org/psr/psr-1/) with the following exceptions.

* Opening bracket `{` must be on the same line instead of a new line for all instances.
* There must be a single new line between every method, property, and class declaration.
* The `elseif` is written as `else if`.

## PSR-4 ##

All packages follow the official [PSR-4 Autoloader Standard](http://www.php-fig.org/psr/psr-4/). 
Each package must have a `composer.json` within the root of the package folder.

## Namespace Organization ##

The term "class" refers to classes, interfaces, traits, and other similar structures.

When declaring a class, it must be organized into the following namespace structure `Titon(\<PackageName>)*\<ClassName>`. 
All implementations of an interface (including an optional abstract class) must be organized in a sub-folder 
that matches the interface name.

Say we have the following setup. A class named `Foo`, an interface named `Bar`, its implementations, 
and a few traits. We would have the following organization.

```
Titon\Package\Bar               // interface
Titon\Package\Bar\AbstractBar   // abstract class
Titon\Package\Bar\PrimaryBar    // class
Titon\Package\Bar\SecondaryBar  // class
Titon\Package\Bazable           // trait
Titon\Package\Foo               // class
Titon\Package\FooAware          // trait
```

The exception to this interface sub-folder organization rule is when a package is based solely around a single interface, 
usually one that shares the same name. An example of this is the controller and view packages. 
This exception exists primarily to stop duplicate folder names, like `Titon\Controller\Controller\AbstractController`.

## Interface Names ##

All interface names *should not* be suffixed with `Interface` -- instead they should be written as a noun (a thing), 
or as an adjective (suffixed with `-er`, `-able`, or `-ible`). For example, `Engine`, `Controller`, `Helper`, 
`Subject`, `Arrayable`, etc.

## Trait Names ##

All trait names must be written as an adjective with a suffix that ends in `-able` or `-ible`. 
There are unique exceptions to this rule, which are:

* If the trait provides a getter and a setter for a class (or interface), it must end in `Aware`. 
    For example, `StorageAware`.
* If the trait provides annotation wiring, it must end in `Annotation`, with the prefix being the annotation name.
    For example, `RouteAnnotation`.
* If the trait provides a set of encapsulated reusable methods or properties, it must end in `Mixin`.
    For example, `FilterMixin`. If there are multiple mixins in a package, they should be grouped into a `Mixin` namespace.

## Method Names ##

All method names, excluding special cases, must be written as a verb and must convey an action. 
For example, `getValue()`, `setValue()`, `hasItem()`, `removeMember()`, `flushContents()`, etc.

The following actions must be used within the correct method context.

* `get()` - Get a value.
* `set()` - Set a value. Only applicable if one possible value.
* `add()` - Set a value to a collection or add multiple values.
* `has()` - Check if a value exists or a key within a collection exists.
* `is()` - Check if a state is truthy.
* `remove()` - Remove a value.
* `flush()` - Remove all values.
* `parse()` - Extract values.
* `build()` - Combine values.
* `run()` - Process a value.
* `do()` - Special case for event callbacks.

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

When naming a type alias, the following suffixes and conventions must be used.

* `Callback` suffix when using a callable.
* `Container` suffix with using a map that contains a map or list.
* `List` suffix when using a vector.
* `Map` suffix when using a map.
* Shapes must be nouns.

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
