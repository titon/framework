<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

use Titon\Debug\Debugger;

/**
 * @see Titon\Debug\Debugger::printException()
 */
if (!function_exists('inspect')) {
    function inspect(Exception $e): void {
        echo Debugger::printException($e);
    }
}

/**
 * @see Titon\Debug\Debugger::backtrace()
 */
if (!function_exists('backtrace')) {
    function backtrace(?Exception $e = null): void {
        echo Debugger::backtrace($e);
    }
}

/**
 * @see Titon\Debug\Debugger::export()
 */
if (!function_exists('export')) {
    function export(mixed $var, bool $short = true): void {
        echo Debugger::export($var, $short);
    }
}

/**
 * @see Titon\Debug\Debugger::debug()
 */
if (!function_exists('debug')) {
    function debug(): void {
        echo call_user_func_array(class_meth('Titon\Debug\Debugger', 'debug'), func_get_args());
    }
}

/**
 * @see Titon\Debug\Debugger::dump()
 */
if (!function_exists('dump')) {
    function dump(): void {
        echo call_user_func_array(class_meth('Titon\Debug\Debugger', 'dump'), func_get_args());
    }
}