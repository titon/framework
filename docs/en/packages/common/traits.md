# Traits #

There are a few traits found in the common package that cannot be grouped with the other guides, but are helpful 
nonetheless.

## Mutable<`Tk`, `Tv`> ##

The `Mutable` trait allows for data to be read and written to from within the class layer. 
The data is represented as a `Map<Tk, Tv>` with generics that should be defined when inheriting the trait.
It could also be coupled with the `IteratorAggregate<Tv>` and `Countable` interfaces for better usability.

```hack
class Foo implements IteratorAggregate<int>, Countable {
    use Titon\Common\Mutable<string, int>;
}

$foo = new Foo();
$foo->set('key', 123);
$foo->get('key'); // 123
```

The trait also supports a dot-notated key in all its key based methods.

```hack
$foo->set('parent.child', 456);
$foo->all(); // Map {'parent' => Map {'child' => 456}}
```

### Methods ###

The following methods are available when inheriting the trait.

<table class="table is-striped">
    <thead>
        <tr>
            <th>Method</th>
            <th>Magic Method</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>add(Map&lt;Tk, Tv&gt; $params): this</td>
            <td></td>
            <td>Add multiple key-value parameters.</td>
        </tr>
        <tr>
            <td>all(): Map&lt;Tk, Tv&gt;</td>
            <td></td>
            <td>Return all parameters as a map.</td>
        </tr>
        <tr>
            <td>flush(): this</td>
            <td></td>
            <td>Remove all parameters.</td>
        </tr>
        <tr>
            <td>get(Tk $key[, ?Tv $default]): ?Tv</td>
            <td>__get(Tk $key): ?Tv</td>
            <td>Return a value defined by key. If no key is found, return the default argument, or null.</td>
        </tr>
        <tr>
            <td>has(Tk $key): bool</td>
            <td>__isset(Tk $key): bool</td>
            <td>Checks for a key in the parameters.</td>
        </tr>
        <tr>
            <td>keys(): Vector&lt;Tk&gt;</td>
            <td></td>
            <td>Return the keys as a list.</td>
        </tr>
        <tr>
            <td>remove(Tk $key): this</td>
            <td>__unset(Tk $key): void</td>
            <td>Remove a parameter defined by key.</td>
        </tr>
        <tr>
            <td>set(Tk $key, Tv $value): this</td>
            <td>__set(Tk $key, Tv $value): void</td>
            <td>Set a single key-value parameter.</td>
        </tr>
        <tr>
            <td>values(): Vector&lt;Tv&gt;</td>
            <td></td>
            <td>Return the values as a list.</td>
        </tr>
    </tbody>
</table>

## Stringable ##

The `Stringable` trait provides a very basic `toString()` implementation that simply returns the class name and namespace. 

```hack
class Bar {
    use Titon\Common\Stringable;
}

$bar = new Bar();
$bar->toString(); // Bar
(string) $bar; // Bar
```