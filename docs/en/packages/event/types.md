# Type Aliases #

The following type aliases are available in the Event package.

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
            <td>Titon\Event\CallStackList</td>
            <td>Vector&lt;string&gt;</td>
            <td>A list of observer callable names in the current event stack.</td>
        </tr>
        <tr>
            <td>Titon\Event\EventMap</td>
            <td>Map&lt;string, Titon\Event\Event&gt;</td>
            <td>A mapping of <code>Titon\Event\Event</code> objects to their unique key.</td>
        </tr>
        <tr>
            <td>Titon\Event\ListenerMap</td>
            <td>Map&lt;string, mixed&gt;</td>
            <td>A mapping of observer configurations to event keys.</td>
        </tr>
        <tr>
            <td>Titon\Event\ObserverList</td>
            <td>Vector&lt;Titon\Event\Observer&gt;</td>
            <td>A list of <code>Titon\Event\Observer</code> objects.</td>
        </tr>
        <tr>
            <td>Titon\Event\ObserverContainer</td>
            <td>Map&lt;string, Titon\Event\ObserverList&gt;</td>
            <td>A mapping of <code>Titon\Event\ObserverList</code>s to event keys.</td>
        </tr>
        <tr>
            <td>Titon\Event\ObserverCallback</td>
            <td>(function(...): mixed)</td>
            <td>A callable that defines the declaration for observers.</td>
        </tr>
        <tr>
            <td>Titon\Event\ParamList</td>
            <td>array&lt;mixed&gt;</td>
            <td>An array of values to pass as arguments to observers.</td>
        </tr>
    </tbody>
</table>
