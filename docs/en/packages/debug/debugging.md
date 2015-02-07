# Debugging #

The `Titon\Debug\Debugger` is a static class that handles all aspects of debugging and error / exception handling. The class must be initialized near the start of an applications bootstrapping process so that handlers are registered and error reporting is disabled (production safe by default).

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

The `Debugger` removes the concept of errors by overriding the error handler and providing a custom implementation that throws `ErrorException`s instead. Why? Because exceptions are better in every aspect.

However, the case in which errors are thrown as exceptions may differ between the type of error occurring. The types of errors that are thrown are: `E_ERROR`, `E_PARSE`, `E_CORE_ERROR`, `E_COMPILE_ERROR`, `E_USER_ERROR`, and `E_RECOVERABLE_ERROR`. All other types of errors (like notices and warnings) are output as stack traces when error reporting is enabled (development) or logged when reporting is disabled (production).

### Uncaught Exceptions ###

When an exception is thrown and not caught, the default exception handler is called. This handler will log the exception, output a stack trace, and exit the interpreter. This handler *should* be modified with the `setHandler()` method.

The method accepts a callable, which in turn accepts the exception. This handler can be used to display catch-all error pages.

```hack
Debugger::setHandler(function(Exception $e) {
    // Render error view and output
});
```

### Fatal Errors ###

When a fatal error occurs that halts the interpreter, the exception handler mentioned previously will be called. This handler will be passed a `Titon\Debug\Exception\FatalErrorException` when error reporting is enabled or a `Titon\Debug\Exception\InternalErrorException` when it is disabled. These exception classes can be used for differentiation.

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

If a [logger](logging.md) has been defined and an `invariant()` call fails an assertion, the associated message will be logged alongside any arguments. Very useful for discovering type issues.

```hack
$foo = new Foo();

invariant($foo instanceof Bar, 'Value must be a Bar'); // Will be logged
```

## Dumping Variables ##

There are 5 methods available that aide in the debugging of variables. Each method makes use of a dumper class that dumps the variable in different formats. When using the command line, the `Titon\Debug\Dumper\CliDumper` is used, which formats the variable for easy reading in a CLI. When testing an application in the browser, the `Titon\Debug\Dumper\HtmlDumper` is used, which formats the variable using HTML.

The `debug()` and `dump()` methods can be used for easily outputting a variables value. These methods are rather similar to `print_r()` or `var_export()` but allow for multiple arguments to be passed.

```hack
Debugger::debug($foo);
Debugger::dump($bar, $baz);
```

The `backtrace()` method will output a stack trace. If an exception is passed to the method, the exception's stack trace will be used.

```hack
Debugger::backtrace(); // Uses debug_backtrace()
Debugger::backtrace($exception); // Uses $exception->getTrace()
```

The `inspect()` method requires an exception and is similar to `backtrace()` as it outputs a stack trace, but will also output the class name, file path, line number, and error message. This is the go to method for debugging exceptions.

```hack
Debugger::inspect($exception);
```

And finally, the `export()` method can be used for outputting a variable in a PHP.Hack valid format, similar to `var_export()`.

```hack
Debugger::export($foo);
```

Each of these methods have an [equivalent global function](functions.md). When using global functions, they take into account the current error reporting setting and *will not* output anything if error reporting is disabled.

### Custom Dumpers ###

A dumper can be customized at anytime using the `setDumper()` method. This method requires a class that extends the `Titon\Debug\Dumper` interface, as well as implements the `debug()`, `dump()`, `backtrace()`, and `inspect()` methods.

```hack
class ExampleDumper implements Titon\Debug\Dumper {
    public function backtrace(?Exception $exception = null): string {}

    public function debug(...$vars): string {}

    public function dump(...$vars): string {}

    public function inspect(Exception $exception): string {}
}

Debugger::setDumper(new ExampleDumper());
```

For guidance on how to create a dumper, we suggest taking a look at the built-in `CliDumper` and `HtmlDumper`. 

## Logging ##

[Jump to the logging docs for more information.](logging.md)
