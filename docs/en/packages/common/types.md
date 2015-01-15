# Type Aliases #

The Common package comes bundled with type aliases that are used throughout the entire framework. 
Each alias has a specific use case that should be followed accordingly.

<table class="table is-striped">
    <thead>
        <tr>
            <th>Alias</th>
            <th>Type</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Titon\Common\ArgumentList</td>
            <td>Vector&lt;mixed&gt;</td>
            <td>
                Represents a list of values to be used as arguments to a function or method. 
                This type is primarily used for factories and annotations.
            </td>
        </tr>
        <tr>
            <td>Titon\Common\CacheMap</td>
            <td>Map&lt;string, mixed&gt;</td>
            <td>
                Represents a set of data used for caching, with each key being a string, 
                and the value being anything. This type is used when data needs to be cached internally within a class.
            </td>
        </tr>
        <tr>
            <td>Titon\Common\DataMap</td>
            <td>Map&lt;string, mixed&gt;</td>
            <td>
                Represents a set of data from any kind of source, whether it be 
                request data (GET, POST, etc) or key-values passed to functions, methods, and classes.
            </td>
        </tr>
        <tr>
            <td>Titon\Common\OptionMap</td>
            <td>Map&lt;string, mixed&gt;</td>
            <td>
                Represents a mapping of customizable options for methods. 
                These options should always have a default value set within the method, that gets overridden. 
                Continue on for more information on this alias.
            </td>
        </tr>
        <tr class="table-divider">
            <td colspan="3">Annotations</td>
        </tr>
        <tr>
            <td>Titon\Common\AnnotationMap</td>
            <td>Map&lt;string, Titon\Common\ArgumentList&gt;</td>
            <td>
                Maps class or method annotation names to their arguments.
            </td>
        </tr>
        <tr class="table-divider">
            <td colspan="3">Instances</td>
        </tr>
        <tr>
            <td>Titon\Common\InstanceMap&lt;T&gt;</td>
            <td>Map&lt;string, T&gt;</td>
            <td>
                Maps singleton and multiton instances by key, with the value being the respective class.
            </td>
        </tr>
        <tr class="table-divider">
            <td colspan="3">Macros</td>
        </tr>
        <tr>
            <td>Titon\Common\MacroCallback</td>
            <td>(function(...): mixed)</td>
            <td>
                A callable that defines the arguments and return type for macros.
            </td>
        </tr>
        <tr>
            <td>Titon\Common\MacroMap</td>
            <td>Map&lt;string, Titon\Common\MacroCallback&gt;</td>
            <td>
                Maps custom macros by name to their callable definition.
            </td>
        </tr>
    </tbody>
</table>

## Usage ##

Examples on how to use the more complex type aliases.

### OptionMap ###

```hack
use Titon\Common\OptionMap;

function foo(OptionMap $options = Map {}): void {
    $options = (Map {
        'bar' => 123,
        'baz' => true
    })->setAll($options);
    
    // ...
}

foo(Map {'baz' => false});
```

<div class="notice is-warning">
    Since option values are <code>mixed</code>, they will need to be casted or passed through <code>invariant()</code> 
    before being passed to other type hinted methods.
</div>