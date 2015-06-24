<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

/**
 * --------------------------------------------------------------
 *  Type Aliases
 * --------------------------------------------------------------
 *
 * Defines type aliases that are used by the debug package.
 */

namespace Titon\Debug {
    use Exception;

    type Caller = shape('file' => string, 'line' => int);
    type Error = shape('error' => string, 'level' => string);
    type ErrorCodeMap = Map<int, string>;
    type ExceptionHandler = (function(Exception): void);
    type Metric = shape('running' => bool, 'time.start' => float, 'time.stop' => float, 'time.avg' => float,
        'memory.start' => int, 'memory.stop' => int, 'memory.avg' => int, 'memory.peak' => int);
    type MetricMap = Map<string, Metric>;
}

/**
 * --------------------------------------------------------------
 *  Annotations
 * --------------------------------------------------------------
 *
 * Registers all annotations declared in the debug packages.
 */

namespace {
    use Titon\Annotation\Registry;

    if (class_exists('Titon\Annotation\Annotation')) {
        Registry::map('Deprecated', 'Titon\Debug\Annotation\Deprecated');
        Registry::map('Monitor', 'Titon\Debug\Annotation\Monitor');
    }
}

/**
 * --------------------------------------------------------------
 *  Helper Functions
 * --------------------------------------------------------------
 *
 * Defines global helper functions for common use cases.
 */

namespace {
    use Titon\Debug\Debugger;

    /**
     * @see Titon\Debug\Debugger::inspect()
     */
    function inspect(Exception $e): void {
        if (Debugger::isOn()) {
            echo Debugger::inspect($e);
        }
    }

    /**
     * @see Titon\Debug\Debugger::backtrace()
     */
    function backtrace(?Exception $e = null): void {
        if (Debugger::isOn()) {
            echo Debugger::backtrace($e);
        }
    }

    /**
     * @see Titon\Debug\Debugger::export()
     */
    function export(mixed $var): void {
        if (Debugger::isOn()) {
            echo Debugger::export($var);
        }
    }

    /**
     * @see Titon\Debug\Debugger::debug()
     */
    function debug(/* HH_FIXME[4033]: variadic + strict */ ...$vars): void {
        if (Debugger::isOn()) {
            echo call_user_func_array(class_meth('Titon\Debug\Debugger', 'debug'), $vars);
        }
    }

    /**
     * @see Titon\Debug\Debugger::dump()
     */
    function dump(/* HH_FIXME[4033]: variadic + strict */ ...$vars): void {
        if (Debugger::isOn()) {
            echo call_user_func_array(class_meth('Titon\Debug\Debugger', 'dump'), $vars);
        }
    }
}
