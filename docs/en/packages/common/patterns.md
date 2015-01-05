# Instantiation Patterns #

There are many ways to instantiate a class besides the standard `new` declaration. The first being the factory pattern, 
the second being a class registry, and the third being a multiton pattern.

## Factory ##

Add the `Titon\Common\FactoryAware` trait to a class to support the `factory()` method.
This method will create a new instance of the class each time it's called.

```hack
class Foo {
    use Titon\Common\FactoryAware;
}

$foo1 = Foo::factory(); // New instance
$foo2 = Foo::factory(); // New instance
```

An optional list of arguments can be passed to the `factory()` method which will be passed to the class constructor.

```hack
$foo = Foo::factory($bar, $baz);
```

## Registry ##

Add the `Titon\Common\FactoryAware` trait to a class to support the `registry()` method. 
This method will create and return the same instance of the class each time it's called (like a singleton).
The instance is stored in the `Titon\Utility\Registry` class.

```hack
class Bar {
    use Titon\Common\FactoryAware;
}

$bar1 = Bar::registry(); // Same instance
$bar2 = Bar::registry(); // Same instance
```

An optional list of arguments can also be passed to `registry()`, similarly to `factory()`.

## Multiton ##

Add the `Titon\Common\Instanceable` trait to a class with generics properly defined. 
This trait provides a handful of methods for managing multiple instances (multiton) or a single instance (singleton). 
The method `getInstance($key)` can be called to create and return an instance of the class. If `$key` is omitted, 
it will be treated like a singleton.

The difference between `Instanceable` and `FactoryAware` mentioned above, is that `Instanceable` instances 
are managed as a protected static property in the class itself, instead of an external class.

```hack
class Baz {
    use Titon\Common\Instanceable<Baz>;
}

$baz1 = Baz::getInstance();
$baz2 = Baz::getInstance(); // Same as $baz1
$baz3 = Baz::getInstance('foo'); // New instance
$baz4 = Baz::getInstance('bar'); // Another new instance
```

An optional list of arguments can be passed to `getInstance()`. Any argument defined after `$key` will be passed 
to the class constructor.

```hack
$baz = Baz::getInstance('default', $foo, $bar);
```

To return all instances of the class, use `getInstances()`.

```hack
$bazs = Baz::getInstances(); // Map<string, Baz>
```

To remove an instance defined by a key, use `removeInstance()`.

```hack
Baz::removeInstance(); // Singleton
Baz::removeInstance('foo');
```

To remove all instances of the class, use `flushInstances()`.

```hack
Baz::flushInstances();
```