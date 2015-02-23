<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Debug;

use Exception;

/**
 * A Dumper is used for outputting the contents of variables and exceptions.
 *
 * @package Titon\Debug
 */
interface Dumper {

    /**
     * Output a stack trace at the time of execution.
     * If an exception is passed, use the stack trace in the exception.
     *
     * @param \Exception $exception
     * @return string
     */
    public function backtrace(?Exception $exception = null): string;

    /**
     * Output a variable, or multiple variables.
     * Include the file and line in which the `debug()` method was called.
     *
     * @param mixed ...$vars
     * @return string
     */
    public function debug(/* HH_FIXME[4033]: variadic + strict */ ...$vars): string;

    /**
     * Output a variable, or multiple variables, and include type information.
     * Include the file and line in which the `dump()` method was called.
     *
     * @param mixed ...$vars
     * @return string
     */
    public function dump(/* HH_FIXME[4033]: variadic + strict */ ...$vars): string;

    /**
     * Output an exception including any meta data and stack trace.
     *
     * @param \Exception $exception
     * @return string
     */
    public function inspect(Exception $exception): string;

}
