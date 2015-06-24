<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Debug\Annotation;

use Titon\Annotation\Annotation;
use Titon\Annotation\Wireable;
use Titon\Debug\Debugger;
use Titon\Debug\Logger;

newtype MonitorCallback = (function(mixed, string): void);

/**
 * The Monitor annotation will monitor a class for instantiation and log a message with
 * the file location and line number of where it occurred.
 *
 * <<Monitor([$callback])>>
 *
 * @package Titon\Debug\Annotation
 */
class Monitor extends Annotation implements Wireable {

    /**
     * Callback to trigger when the class is monitored.
     *
     * @type \Titon\Debug\Annotation\MonitorCallback
     */
    protected ?MonitorCallback $callback = null;

    /**
     * Create a callback based on a string.
     *
     * @param string $callback
     */
    public function __construct(string $callback = '') {
        // UNSAFE
        // Since both `fun()` and `class_meth()` require literal strings and we're passing variables
        if (strpos($callback, '::')) {
            list($class, $method) = explode('::', $callback);

            $this->callback = class_meth($class, $method);

        } else if ($callback) {
            $this->callback = fun($callback);
        }
    }

    /**
     * Log the file and line number where the class was instantiated.
     *
     * @param T $class
     * @param string $method
     * @return $this
     */
    public function wire<T>(T $class, string $method = ''): this {
        $className = get_class($class);
        $file = '[unknown]';
        $line = 0;

        foreach (debug_backtrace() as $trace) {
            if (array_key_exists('function', $trace) && $trace['function'] === '__construct' && $trace['class'] === $className) {
                $file = $trace['file'];
                $line = (int) $trace['line'];
                break;
            }
        }

        // Log a message
        Debugger::getLogger()?->log(Logger::INFO, '{class} was instantiated in {file} on line {line}.', [
            'class' => $className,
            'file' => $file,
            'line' => $line
        ]);

        // Trigger a callback
        if ($callback = $this->callback) {
            call_user_func_array($callback, [$class, $method]);
        }

        return $this;
    }

}
