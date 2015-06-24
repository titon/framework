# Macros #

Macros allow for static class methods to be defined and called at runtime. To begin, add the `Titon\Common\Macroable` trait to a class.

```hack
class Foo {
    use Titon\Common\Macroable;
}
```

<div class="notice is-warning">
    Macros require the <code>__callStatic()</code> magic method to work properly.
</div>

## Adding A Macro ##

To add a macro, call the static `macro()` method with a unique name and a closure to be called.

```hack
Foo::macro('lower', ($value) ==> mb_strtolower($value));
```

## Calling A Macro ##

Once a macro has been added, you can simply call the macro by name as if it was a static method.

```hack
Foo::lower('BaR'); // bar
```

If a macro is called that has not been defined, a `Titon\Common\Exception\MissingMacroException` is thrown.
