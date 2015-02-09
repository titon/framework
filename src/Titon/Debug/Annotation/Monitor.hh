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

/**
 * The Monitor annotation will monitor a class for instantiation and log a message with
 * the file location and line number of where it occurred.
 *
 * <<Monitor>>
 *
 * @package Titon\Debug\Annotation
 */
class Monitor extends Annotation implements Wireable {

    /**
     * Log the file and line number where the class was instantiated.
     *
     * @param T $class
     * @param string $method
     * @return $this
     */
    public function wire<T>(T $class, string $method = ''): this {
        $class = get_class($class);
        $file = '[unknown]';
        $line = 0;

        foreach (debug_backtrace() as $trace) {
            if (array_key_exists('function', $trace) && $trace['function'] === '__construct' && $trace['class'] === $class) {
                $file = $trace['file'];
                $line = (int) $trace['line'];
                break;
            }
        }

        Debugger::getLogger()?->log(Logger::INFO, '{class} was instantiated in {file} on line {line}.', [
            'class' => $class,
            'file' => $file,
            'line' => $line
        ]);

        return $this;
    }

}
