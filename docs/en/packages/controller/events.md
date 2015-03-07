# Events #

The following events are available in the Controller package.

### controller.processing ###

The `controller.processing` event, represented by `Titon\Controller\Event\ProcessingEvent`, is triggered before the action is executed in `dispatchTo()`.

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
            <td>Controller</td>
            <td>Titon\Controller\Controller</td>
            <td>getController()</td>
            <td></td>
            <td>The controller in which the action is located and is doing the dispatching.</td>
        </tr>
        <tr>
            <td>Action</td>
            <td>string</td>
            <td>getAction()</td>
            <td></td>
            <td>The action being dispatched.</td>
        </tr>
        <tr>
            <td>Arguments</td>
            <td>Titon\Common\ArgumentList</td>
            <td>getArguments()</td>
            <td></td>
            <td>The arguments to pass to the action.</td>
        </tr>
    </tbody>
</table>

### controller.processed ###

The `controller.processed` event, represented by `Titon\Controller\Event\ProcessedEvent`, is triggered after the action has been executed in `dispatchTo()`, but before the response is returned. The response *can be* modified or replaced from this event. 

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
            <td>Controller</td>
            <td>Titon\Controller\Controller</td>
            <td>getController()</td>
            <td></td>
            <td>The controller in which the action is located and is doing the dispatching.</td>
        </tr>
        <tr>
            <td>Action</td>
            <td>string</td>
            <td>getAction()</td>
            <td></td>
            <td>The action being dispatched.</td>
        </tr>
        <tr>
            <td>Response</td>
            <td>Psr\Http\Message\OutgoingResponseInterface</td>
            <td>getResponse()</td>
            <td>setResponse()</td>
            <td>The HTTP response object in which the action output was set to.</td>
        </tr>
    </tbody>
</table>

### controller.error ###

The `controller.error` event, represented by `Titon\Controller\Event\ErrorEvent`, is triggered when the controllers `renderError()` method is called.

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
            <td>Controller</td>
            <td>Titon\Controller\Controller</td>
            <td>getController()</td>
            <td></td>
            <td>The controller in which the error occurred.</td>
        </tr>
        <tr>
            <td>Exception</td>
            <td>Exception</td>
            <td>getException()</td>
            <td></td>
            <td>The exception thrown to trigger the error.</td>
        </tr>
    </tbody>
</table>
