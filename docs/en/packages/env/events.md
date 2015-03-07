# Events #

The following events are available in the Environment package.

### env.initializing ###

The `env.initializing` event, represented by `Titon\Environment\Event\InitializingEvent`, is triggered before the environment is detected in `initialize()`.

<table class="table is-striped">
    <thead>
        <tr>
            <th>Argument</th>
            <th>Type</th>
            <th>Getter</th>
            <th>Setter</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Environment</td>
            <td>Titon\Environment\Environment</td>
            <td>getEnvironment()</td>
            <td></td>
            <td>The environment instance doing the detecting.</td>
        </tr>
    </tbody>
</table>

### env.initialized ###

The `env.initialized` event, represented by `Titon\Environment\Event\InitializedEvent`, is triggered after the environment is detected in `initialize()`. The matched host is passed as an argument.

<table class="table is-striped">
    <thead>
        <tr>
            <th>Argument</th>
            <th>Type</th>
            <th>Getter</th>
            <th>Setter</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Environment</td>
            <td>Titon\Environment\Environment</td>
            <td>getEnvironment()</td>
            <td></td>
            <td>The environment instance doing the detecting.</td>
        </tr>
        <tr>
            <td>Host</td>
            <td>Titon\Environment\Host</td>
            <td>getHost()</td>
            <td></td>
            <td>The host that was detected.</td>
        </tr>
    </tbody>
</table>
