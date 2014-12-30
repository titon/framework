# Debug #

Provides a debugging and logging layer that is easy to integrate and easy to configure.
The `Debugger` toggles environment settings, replaces errors with exceptions, handles uncaught exceptions,
intercepts fatal errors, and analyzes data for debugging.

```hack
use Titon\Debug\Debugger;

Debugger::initialize(); // Setup settings and handlers
Debugger::enable(); // Enable errors in develop
```

All of the analyzation methods can be found as global functions for easier use.

```hack
inspect($exception); // Debug exceptions
backtrace(); // Trace function and exception calls
export($var); // Export a variable
debug($var); // Print a variable
dump($var); // Dump a variable
```

On top of the functionality mentioned above, the `Debugger` also manages a global `Logger` that logs exceptions
when error reporting is turned off. The logging layer is built around [PSR-3](http://www.php-fig.org/psr/psr-3/) and will accept any logger that
implements the required interfaces. To keep it simple, a lightweight file system logger comes bundled with the package.

```hack
Debugger::setLogger(new Titon\Debug\Logger('/tmp/logs'));
```