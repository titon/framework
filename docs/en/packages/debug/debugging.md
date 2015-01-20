# Debugging #

The `Titon\Debug\Debugger` is a static class that handles all aspects of debugging and error / exception handling. 
The class must be initialized near the start of an applications bootstrapping process so that handlers are registered and 
error reporting is disabled (production safe by default).

```hack
use Titon\Debug\Debugger;

Debugger::initialize();
```

## Turning On & Off ##

Once debugging has been initialized, error reporting can be toggled through the `enable()` and `disable()` methods. 

```hack
Debugger::enable();
```

When enabled, error reporting will be set to `E_ALL | E_STRICT`. This can be customized by passing flags to the `enable()` method.

```hack
Debugger::enable(E_ALL);
```

The `isOn()` and `isOff()` methods can be used to check if error reporting is enabled or disabled, respectively. 

```hack
Debugger::isOn(); // true
```

## Error & Exception Handling ##

The `Debugger` removes the concept of errors by overriding the error handler and providing a custom implementation 
that throws `ErrorException`s instead. Why? Because exceptions are better in every aspect.

However, the case in which errors are thrown as exceptions may differ between the type of error occurring. 
The types of errors that are thrown are: `E_ERROR`, `E_PARSE`, `E_CORE_ERROR`, `E_COMPILE_ERROR`, `E_USER_ERROR`, 
and `E_RECOVERABLE_ERROR`. All other types of errors (like notices and warnings) are output as stack traces 
when error reporting is enabled (development) or logged when reporting is disabled (production).

### Uncaught Exceptions ###

When an exception is thrown and not caught, the default exception handler is called. This handler will log the exception, 
output a stack trace, and exit the interpreter. This handler *should* be modified with the `setHandler()` method.

The method accepts a callable, which in turn accepts the exception. This handler can be used to display catch-all error pages.

```hack
Debugger::setHandler(function(Exception $e) {
    // Render error view and output
});
```

### Fatal Errors ###

When a fatal error occurs that halts the interpreter, the exception handler mentioned previously will be called. 
This handler will be passed a `Titon\Debug\Exception\FatalErrorException` when error reporting is enabled or a 
`Titon\Debug\Exception\InternalErrorException` when it is disabled. These classes can be used for differentiation.

```hack
Debugger::setHandler(function(Exception $e) {
    if ($e instanceof Titon\Debug\Exception\FatalErrorException) {
        // Render development error view

    } else if ($e instanceof Titon\Debug\Exception\InternalErrorException) {
        // Render production error view

    } else {
        // Render all others
    }
});
```

## Invariant Handling ##

If a [logger](logging.md) has been defined and an `invariant()` call fails an assertion, 
the associated message will be logged alongside any arguments. Very useful for discovering type issues.

```hack
$foo = new Foo();

invariant($foo instanceof Bar, 'Value must be a Bar'); // Will be logged
```

## Dumping Variables ##

There are 5 global functions available that aide in the debugging of variables. Each function will parse and output 
a value into a human-readable HTML-ready format -- which allows for beautiful styling and functionality within browsers.

The `debug()` and `dump()` functions can be used for easily outputting a variables value. 
These functions are rather similar to `print_r()` or `var_export()` but allow for multiple arguments to be passed.

```hack
debug($foo);
dump($bar, $baz);
```

The `backtrace()` function will output a collapsible stack trace as an HTML table.
If an exception is passed to the function, the exception's stack trace will be used.

```hack
backtrace(); // Uses debug_backtrace()
backtrace($exception); // Uses $exception->getTrace()
```

The `inspect()` function works similar to `backtrace()` but will also output the class name, file path, line number, 
error message, and stack trace. This is the go to function for debugging exceptions.

```hack
inspect($exception);
```

And finally, the `export()` function can be used for outputting a variable in a PHP valid format, similar to `var_export()`.

```hack
export($foo);
```

All of these global functions will echo their content instead of returning. To grab the return, 
use their equivalent method found on the `Debugger` class.

## Logging ##

[Jump to the logging docs for more information.](logging.md)
