# Type Aliases #

The Common package comes bundled with type aliases that are used throughout the entire framework. Each alias has a specific use case that should be followed accordingly.

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
            <td>Titon\Common\CacheMap</td>
            <td>Map&lt;string, mixed&gt;</td>
            <td>
                Represents a set of data used for caching, with each key being a string, 
                and the value being anything. This type is used when data needs to be cached internally within a class.
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
