# Type Aliases #

The following type aliases are available in the Debug package.

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
            <td>Titon\Debug\Metric</td>
            <td>
                shape('running' =&gt; bool, 'time.start' =&gt; float, 'time.stop' =&gt; float, 'time.avg' =&gt; float,
                    'memory.start' =&gt; int, 'memory.stop' =&gt; int, 'memory.avg' =&gt; int, 'memory.peak' =&gt; int)
            </td>
            <td>A shape that represents a benchmarking metric.</td>
        </tr>
        <tr>
            <td>Titon\Debug\MetricMap</td>
            <td>Map&lt;int, Titon\Debug\Metric&gt;</td>
            <td>A mapping of metrics to unique keys.</td>
        </tr>
        <tr>
            <td>Titon\Debug\Error</td>
            <td>shape('error' =&gt; string, 'level' =&gt; string)</td>
            <td>A shape that maps a logging level to an error code.</td>
        </tr>
        <tr>
            <td>Titon\Debug\ErrorCodeMap</td>
            <td>Map&lt;int, string&gt;</td>
            <td>A mapping of error codes to human readable titles.</td>
        </tr>
        <tr>
            <td>Titon\Debug\ExceptionHandler</td>
            <td>(function(Exception): void)</td>
            <td>A callable that handles the uncaught exception and fatal error scenarios.</td>
        </tr>
    </tbody>
</table>
