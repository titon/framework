# Controllers #

A controller acts as the median between the request and response within the dispatch cycle. It splits up its responsibility into multiple actions, where each action deals with specific business logic.

All controllers should extend from the `Titon\Controller\AbstractController` base class or implement the `Titon\Controller\Controller` interface.

```hack
class ExampleController extends Titon\Controller\AbstractController {
    // ...
}

$controller = new ExampleController($request, $response);
```

## Request & Response ##

The primary role of a controller is to receive an HTTP request and to return an HTTP response. A controller cannot fulfill this without a request and response object -- which can be set through the constructor. The controller abides the [PHP-FIG HTTP Message specification](https://github.com/php-fig/http-message) and will accept any object that implements the PSR interfaces `Psr\Http\Message\IncomingRequestInterface` and `Psr\Http\Message\OutgoingResponseInterface`.

One such library that implements the PSR is the [HTTP package](../http/index.md), which can be used like so.

```hack
$request = Titon\Http\Server\Request::createFromGlobals();
$response = new Titon\Http\Server\Response();
$controller = new ExampleController($request, $response);
```

## Creating Actions ##

An action handles the processing of a request and is represented by a method in a controller. The action can do anything it needs to -- query the database, load configuration, process forms, call APIs, render templates, etc -- but it should always return a response. 

```hack
class ExampleController extends Titon\Controller\AbstractController {
    public function index(): mixed {
        // ...
    }
}
```

<div class="notice is-info">
    An action should always return <code>mixed</code>.
</div>

### Returning A Response ###

There are three ways to set the response, the first by returning a string, which is typically text or HTML. If no return value is defined for an action, the `renderView()` method will automatically be called.

```hack
public function foo(): mixed {
    return 'Hello world';
}
```

The second option is to return a stream. This approach follows the [HTTP Message specification](https://github.com/php-fig/http-message).

```hack
public function bar(): mixed {
    return new Titon\Http\Stream\MemoryStream('Hello world');
}
```

The last option is to return a response object directly. This is useful in returning special response, like JSON or XML. This example makes use of the `Response` class in the [HTTP package](../http/index.md), in which it returns a new `Titon\Http\Server\JsonResponse`.

```hack
public function baz(): mixed {
    return Titon\Http\Server\Response::json($data);
}
```

### Action Classes ###

Jump the the [actions documentation](actions.md) for more information on using the `Titon\Controller\Action` class.

## Dispatching Actions ##

Once an action has been determined, either through a router, or through some other means, it must be dispatched from a controller. The action can be dispatched to using the `dispatchTo()` method. The action name can either be in camel case or dashed format, and an optional list of arguments can be passed as the 2nd argument.

```hack
$response = $controller->dispatchTo('action-name');
$response = $controller->dispatchTo('action-name', ['foo', 'bar']);
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
        
        return $this->forwardTo('newAction', [$foo, $bar]);
    }
    
    public function newAction(string $foo, string $bar): mixed {
        // ...
    }
}
```

Both of these methods will return a `Psr\Http\Message\OutgoingResponseInterface` object.

### Missing Actions ###

If the dispatch process encounters an action that does not exist in the current controller, the `missingAction()` method is called. This method can be used to return a customized error page, or can easily just throw an exception instead.

```hack
public function missingAction(): mixed {
    return 'An error has occurred!';
}
```

By default, this action throws an exception.

## Template Rendering ##

The controller package has built-in template rendering support through the [View package](../view/index.md). However, nothing is stopping you from using a third-party templating engine.

To begin, simply set the view into the controller.

```hack
$controller->setView(new Titon\View\EngineView(Vector {'/template/lookup/path'}));
```

The `renderView()` method can be called manually within an action, or will be called automatically during `dispatchTo()` if the action returns no output.

```hack
public function action(string $foo, string $bar): mixed {
    return $this->renderView();
}
```

The template path is based on the controller and action name. So if we had the controller `Foo\BarBazController::actionName()`, the template path would be `/bar-baz/action-name`.

### Passing Data ###

To pass data to the view, simply set a variable through the view layer. 

```hack
public function action(): mixed {
    $this->getView()->setVariable('foo', 'bar');
}
```

[Jump to the official view documentation for more information.](../view/index.md)

### Rendering Errors ###

Similar to rendering views, rendering errors is handled through the `renderError()` method, which accepts an `Exception` as its only argument. This method is automatically called in `dispatchTo()` if an action throws an exception.

If error reporting is enabled, then the private `/errors/error` template is rendered, else the private `/errors/http` template is. The template is then passed the following variables: `$pageTitle` (status reason), `$error` (the exception), `$code` (exception or status code), `$message`, and `$url`.
