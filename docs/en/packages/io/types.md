# Type Aliases #

The following type aliases are available in the IO package.

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
            <td>Titon\Io\DomainList</td>
            <td>Vector&lt;string&gt;</td>
            <td>
                A list of domain names mapped within a <code>Titon\Io\Bundle</code>.
            </td>
        </tr>
        <tr>
            <td>Titon\Io\PathList</td>
            <td>Vector&lt;string&gt;</td>
            <td>
                A list of filesystem paths mapped within a <code>Titon\Io\Bundle</code>.
            </td>
        </tr>
        <tr>
            <td>Titon\Io\DomainPathMap</td>
            <td>Map&lt;string, Titon\Io\PathList&gt;</td>
            <td>
                A mapping of <code>Titon\Io\PathList</code>s to domain names.
            </td>
        </tr>
        <tr>
            <td>Titon\Io\ReaderList</td>
            <td>Vector&lt;Titon\Io\Reader&gt;</td>
            <td>
                A list of <code>Titon\Io\Reader</code> objects.
            </td>
        </tr>
        <tr>
            <td>Titon\Io\ReaderMap</td>
            <td>Map&lt;string, Titon\Io\Reader&gt;</td>
            <td>
                A mapping of <code>Titon\Io\Reader</code> objects to a unique key.
            </td>
        </tr>
        <tr>
            <td>Titon\Io\ResourceMap</td>
            <td>Map&lt;string, mixed&gt;</td>
            <td>
                A basic key-value map structure used in reading and writing resource files.
            </td>
        </tr>
        <tr>
            <td>Titon\Io\WriterMap</td>
            <td>Map&lt;string, Titon\Io\Writer&gt;</td>
            <td>
                A mapping of <code>Titon\Io\Writer</code> objects to a unique key.
            </td>
        </tr>
    </tbody>
</table>
