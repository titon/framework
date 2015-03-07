# Type Aliases #

The following type aliases are available in the Controller package.

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
            <td>Titon\Controller\ActionMap</td>
            <td>Map&lt;string, Titon\Controller\ArgumentList&gt;</td>
            <td>
                A mapping of action names to its arguments. 
                These values are usually parsed out of a routing layer.
            </td>
        </tr>
        <tr>
            <td>Titon\Controller\ArgumentList</td>
            <td>array&lt;mixed&gt;</td>
            <td>A list of arguments passed to an action when dispatched to.</td>
        </tr>
    </tbody>
</table>
