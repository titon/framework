# Type Aliases #

The following type aliases are available in the Intl package.

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
            <td>Titon\Intl\CatalogMap</td>
            <td>Map&lt;string, Titon\Intl\Catalog&gt;</td>
            <td>
                A mapping of <code>Titon\Intl\Catalog</code>s to unique keys.
            </td>
        </tr>
        <tr>
            <td>Titon\Intl\LocaleMap</td>
            <td>Map&lt;string, Titon\Intl\Locale&gt;</td>
            <td>
                A mapping of <code>Titon\Intl\Locale</code>s to locale codes.
            </td>
        </tr>
        <tr>
            <td>Titon\Intl\LocaleTagMap</td>
            <td>Map&lt;string, string&gt;</td>
            <td>
                A map of locale tags when composing and decomposing.
            </td>
        </tr>
        <tr>
            <td>Titon\Intl\MessageKey</td>
            <td>
                shape('domain' =&gt; string, 'catalog' =&gt; string, 'key' =&gt; string)
            </td>
            <td>
                A shape that represents all the segments that make up a message key.
            </td>
        </tr>
        <tr>
            <td>Titon\Intl\MessageMap</td>
            <td>Map&lt;string, string&gt;</td>
            <td>
                A mapping of message keys to message strings.
            </td>
        </tr>
        <tr>
            <td>Titon\Intl\ParamList</td>
            <td>Vector&lt;mixed&gt;</td>
            <td>
                A list of parameters used for interpolation during message translation.
            </td>
        </tr>
    </tbody>
</table>
