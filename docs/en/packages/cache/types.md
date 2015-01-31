# Type Aliases #

The following type aliases are available in the Cache package.

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
            <td>Titon\Cache\CacheCallback</td>
            <td>(function(): mixed)</td>
            <td>
                A callable that defines the return type for callback saving.
            </td>
        </tr>
        <tr>
            <td>Titon\Cache\ItemList</td>
            <td>Vector&lt;Titon\Cache\Item&gt;</td>
            <td>
                A list of <code>Titon\Cache\Item</code> objects used in deferred saving.
            </td>
        </tr>
        <tr>
            <td>Titon\Cache\ItemMap</td>
            <td>Map&lt;string, Titon\Cache\Item&gt;</td>
            <td>
                A mapping of <code>Titon\Cache\Item</code> objects to their cache keys.
            </td>
        </tr>
        <tr>
            <td>Titon\Cache\StatsMap</td>
            <td>Map&lt;string, mixed&gt;</td>
            <td>
                A mapping of key-value statistics.
            </td>
        </tr>
        <tr>
            <td>Titon\Cache\StorageMap</td>
            <td>Map&lt;string, Titon\Cache\Storage&gt;</td>
            <td>
                A mapping of <code>Titon\Cache\Storage</code> engines to unique keys.
            </td>
        </tr>
    </tbody>
</table>
