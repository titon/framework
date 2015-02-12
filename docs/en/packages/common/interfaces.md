# Interfaces #

The following interfaces provide conversion methods for specialized types.

## Arrayable<`Tk`, `Tv`> ##

The `Arrayable` interface provides a `toArray()` method that returns a key-value array.

```hack
class Foo<Tk, Tv> implements Titon\Common\Arrayable<Tk, Tv> {
    public function toArray(): array<Tk, Tv> {
        return [];
    }
}

$foo = new Foo();
$foo->toArray();
```

## Jsonable ##

The `Jsonable` interface provides a `toJson()` method that returns a JSON string. 
An optional [JSON flag](http://php.net/manual/en/json.constants.php) can be passed to the 1st argument. 

```hack
class Foo implements Titon\Common\Jsonable {
    public function toJson(int $options = 0): string {
        return (string) json_encode($value, $options);
    }
}

$foo = new Foo();
$foo->toJson();
```

## Mapable<`Tk`, `Tv`> ##

The `Mapable` interface provides a `toMap()` method that returns a key-value map.

```hack
class Foo<Tk, Tv> implements Titon\Common\Mapable<Tk, Tv> {
    public function toMap(): Map<Tk, Tv> {
        return Map {};
    }
}

$foo = new Foo();
$foo->toMap();
```

## Vectorable<`Tv`> ##

The `Vectorable` interface provides a `toVector()` method that returns an index based vector.

```hack
class Foo<Tv> implements Titon\Common\Vectorable<Tv> {
    public function toVector(): Vector<Tv> {
        return Vector {};
    }
}

$foo = new Foo();
$foo->toVector();
```

## Xmlable ##

The `Xmlable` interface provides a `toXml()` method that returns an XML structured string.
We suggest using the built-in [XML class](../type/xml.md).

```hack
class Foo implements Titon\Common\Xmlable {
    public function toXml(string $root = 'root'): string {
        return Titon\Type\Xml::from($value, $root)->toString();
    }
}

$foo = new Foo();
$foo->toXml();
```
