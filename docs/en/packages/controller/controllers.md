# Controllers #

A controller acts as the median between the request and response within the dispatch cycle.
It splits up its responsibility into multiple actions, where each action deals with specific business logic.

All controllers should extend from the `Titon\Controller\AbstractController` base class 
or implement the `Titon\Controller\Controller` interface.

```hack
class ExampleController extends Titon\Controller\AbstractController {
    // ...
}

$controller = new ExampleController();
```

## Request & Response ##

The primary role of a controller is to receive an HTTP request and to return an HTTP response. 
A controller cannot fulfill this without a request and response object -- which can be set using `setRequest()` and `setResponse()`. 
These methods support the [PHP-FIG HTTP Message specification](https://github.com/php-fig/http-message) 
and will accept any object that implements the PSR interfaces `Psr\Http\Message\IncomingRequestInterface` and 
`Psr\Http\Message\OutgoingResponseInterface`.

One such library that implements the PSR is the [HTTP package](../http/index.md), which can be used like so.

```hack
$controller->setRequest(Titon\Http\Server\Request::createFromGlobals());
$controller->setResponse(Titon\Http\Server\Response::factory());
```

## Creating Actions ##

An action handles the processing of a request and is represented by a method in a controller. 
The action can do anything it needs to -- query the database, load configuration, process forms, call APIs, 
render templates -- but it should always return a response. 

```hack
class ExampleController extends Titon\Controller\AbstractController {
    public function index(): mixed {
        // ...
    }
}
```

<div class="notice is-info">
    An action should always return <code>mixed</code> or <code>void</code>.
</div>

### Returning A Response ###

There are many ways to set the response, most of which require custom implementations outside of the controller, 
but we'll get to that in a moment.

The first option is by returning a string, which is usually reserved for any text or HTML. 
The string will need to be set in the response outside of the controller.

```hack
public function foo(): mixed {
    return 'Hello world';
}
```

To simplify this, we can use the `renderView()` method. More information can be found on this below.

```hack
public function foo(): mixed {
    return $this->renderView();
}
```

The second option is to set the body of the response into the response object directly. 
This approach follows the [HTTP Message specification](https://github.com/php-fig/http-message) and requires a stream.

```hack
public function bar(): void {
    $this->getResponse()
        ->setBody(new Titon\Http\Stream\MemoryStream('Hello world'));
}
```

The last option is to return a response object directly. This is very handy in returning special response, like JSON or XML. 
This example makes use of the `Response` class in the [HTTP package](../http/index.md), 
in which it returns a new `Titon\Http\Server\JsonResponse`.

```hack
public function baz(): mixed {
    return Titon\Http\Server\Response::json($data);
}
```

### Action Classes ###

Jump the the [actions documentation](actions.md) for more information on using the `Titon\Controller\Action` class.

## Dispatching Actions ##

Once an action has been determined, either through a router, or through some other means, it must be dispatched from a controller. 
The method can be dispatched to using the `dispatchTo()` method. The action name can either be in camel case or dashed format, 
and an optional list of arguments can be passed as the 2nd argument.

```hack
$controller->dispatchTo('action-name');
$controller->dispatchTo('action-name', Vector {'foo', 'bar'});
```

The previous example will dispatch and execute the `actionName()` method on the controller.

```hack
class ExampleController extends Titon\Controller\AbstractController {
    public function actionName(string $foo, string $bar): mixed {
        // ...
    }
}
```

If you need to forward one action to another, use the `forwardTo()` method. 

```hack
class ExampleController extends Titon\Controller\AbstractController {
    public function oldAction(string $foo, string $bar): mixed {
        // Some processing...
        
        return $this->forwardTo('newAction', Vector {$foo, $bar});
    }
    
    public function newAction(string $foo, string $bar): mixed {
        // ...
    }
}
```

### Missing Actions ###

If the dispatch process encounters an action that does not exist in the current controller, 
the `missingAction()` method is called. This method can be used to return a customized error page,
or can easily just throw an exception instead.

```hack
public function missingAction(): mixed {
    return 'An error has occurred!';
}
```

By default, this action throws an exception.

### Handling A Response ###

Depending on the application, or the implementation, the handling of the response may differ. 
This is simply a quick example on how to utilize the controller and action to output a response to the browser.

```hack
$response = $controller->getResponse();

try {
    $output = $controller->dispatchTo('action-name', $args);
    
    if (!$response->getBody()) {
    
        // Replace the response with a new one
        if ($output instanceof Psr\Http\Message\OutgoingResponseInterface) {
            $response = $output;
            
        // Output is a string, so use it directly
        } else if (is_string($output) && $output) {
            $response->setBody(new Titon\Http\Stream\MemoryStream($output));
            
        // Either no output, or something we don't want to support, so render a view automatically
        } else {
            $controller->renderView();
        }
    }
    
// An exception was thrown at some point, so lets render an error
} catch (Exception $e) {
    $controller->renderError($e);
}

// Output the response to the browser
$response->prepare($controller->getRequest());
$response->send();
```

## Template Rendering ##

The Controller package has built-in template rendering support through the [View package](../view/index.md). 
However, nothing is stopping you from using a third-party templating engine.

To begin, simply set the view into the controller.

```hack
$controller->setView(new Titon\View\EngineView(Vector {'/template/lookup/path'}));
```

And then call the `renderView()` method. This can either be called in the action, or outside of it, like the example above. 
This method will automatically set the rendered template as the response body (same with `renderError()` below).

```hack
$controller->renderView();
```

The template path is based on the controller and action name. So if we had the controller 
`Foo\BarBazController::actionName()`, the template path would be `/bar-baz/action-name`.

### Passing Data ###

To pass data to the view, simply set a variable through the view layer. 

```hack
public function action(): mixed {
    $this->getView()->setVariable('foo', 'bar');
}
```

[Jump to the official view documentation for more information.](../view/index.md)

### Rendering Errors ###

Similar to rendering views, rendering errors is handled through the `renderError()` method, which accepts an `Exception` as its only argument. 
If error reporting is enabled, then the private `/errors/error` template is rendered, else the private `/errors/http` template is. 

The template is passed the following variables: `$pageTitle` (status reason), `$error` (the exception), `$code` (exception or status code),
`$message`, and `$url`.

```hack
$controller->renderError(new RuntimeException('Oops...'));
```

## Events ##

<table class="table is-striped">
    <thead>
        <tr>
            <th>Event</th>
            <th>Arguments</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>controller.processing</td>
            <td>Titon\Controller\Controller $controller, string $action, Titon\Common\ArgumentList $args</td>
            <td>Triggered after <code>dispatchTo()</code> is called but before the action is executed.</td>
        </tr>
        <tr>
            <td>controller.processed</td>
            <td>Titon\Controller\Controller $controller, string $action, mixed $response</td>
            <td>Triggered after the action is executed but before <code>dispatchTo()</code> is finished.</td>
        </tr>
        <tr>
            <td>controller.error</td>
            <td>Titon\Controller\Controller $controller, Exception $exception</td>
            <td>Triggered after <code>renderError()</code> is called but before the view is rendered.</td>
        </tr>
    </tbody>
</table>

<div class="notice is-warning">
    These events are only triggered if the <code>Titon\Controller\AbstractController</code> is extended from.
</div>