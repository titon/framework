# Type Aliases #

The following type aliases are available in the Context package.

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
            <td>Titon\Context\AliasMap</td>
            <td>Map&lt;string, string&gt;</td>
            <td>
                A mapping of container aliases to class names or other container registrars.
            </td>
        </tr>
        <tr>
            <td>Titon\Context\ClassList</td>
            <td>Vector&lt;string&gt;</td>
            <td>
                A list of fully qualified class names used as providers in <code>Titon\Context\ServiceProvider</code>.
            </td>
        </tr>
        <tr>
            <td>Titon\Context\Item</td>
            <td>
                shape('definition' =&gt; Titon\Context\Definition, 'singleton' =&gt; bool)
            </td>
            <td>A shape that represents an item registered in a <code>Titon\Context\Depository</code>.</td>
        </tr>
        <tr>
            <td>Titon\Context\ItemMap</td>
            <td>Map&lt;string, Titon\Context\Item&gt;</td>
            <td>
                A mapping of <code>Titon\Context\Item</code> shapes to their class name or alias.
            </td>
        </tr>
        <tr>
            <td>Titon\Context\Method</td>
            <td>
                shape('method' =&gt; string, 'arguments' =&gt; array&lt;mixed&gt;)
            </td>
            <td>A shape that represents a method and its arguments for use in method injection.</td>
        </tr>
        <tr>
            <td>Titon\Context\MethodList</td>
            <td>Vector&lt;Titon\Context\Method&gt;</td>
            <td>
                A list of <code>Titon\Context\Method</code> shapes used in method injection.
            </td>
        </tr>
        <tr>
            <td>Titon\Context\ProviderList</td>
            <td>Vector&lt;Titon\Context\ServiceProvider&gt;</td>
            <td>
                A list of <code>Titon\Context\ServiceProvider</code>s registered in a container.
            </td>
        </tr>
        <tr>
            <td>Titon\Context\SingletonMap</td>
            <td>Map&lt;string, mixed&gt;</td>
            <td>
                A mapping of objects (instantiated classes) to their class name or alias.
            </td>
        </tr>
    </tbody>
</table>
