<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Debug\Dumper;

use Titon\Debug\Debugger;
use Titon\Debug\Dumper;
use Titon\Utility\Path;
use Exception;

/**
 * The CliDumper is used for outputting variables while in a command line interface.
 *
 * @package Titon\Debug\Dumper
 */
class CliDumper implements Dumper {

    /**
     * {@inheritdoc}
     */
    public function backtrace(?Exception $exception = null): string {
        if ($exception) {
            $stack = $exception->getTrace();
        } else {
            $stack = debug_backtrace();
        }

        $output = '';

        foreach ($stack as $i => $trace) {
            $file = array_key_exists('file', $trace) ? Path::alias($trace['file']) : '[internal]';
            $line = array_key_exists('line', $trace) ? (int) $trace['line'] : 0;

            if (array_key_exists('class', $trace)) {
                $method = $trace['class'] . $trace['type'] . $trace['function'];
            } else {
                $method = $trace['function'];
            }

            if (strpos($method, '{closure}') === false) {
                $method .= '()';
            }

            $output .= sprintf('#%s: %s - %s:%d',
                str_pad($i + 1, strlen(count($stack)), '0', STR_PAD_LEFT),
                $method, $file, $line) . PHP_EOL;
        }

        return $output;
    }

    /**
     * {@inheritdoc}
     */
    public function debug(...$vars): string {
        $caller = Debugger::getCaller('debug');
        $output = $caller['file'] . ':' . $caller['line'];
        $length = strlen($output);

        foreach ($vars as $var) {
            $output .= $this->hr($length);
            $output .= print_r($var, true);
        }

        return $output;
    }

    /**
     * {@inheritdoc}
     */
    public function dump(...$vars): string {
        $caller = Debugger::getCaller('dump');
        $output = $caller['file'] . ':' . $caller['line'];
        $length = strlen($output);

        foreach ($vars as $var) {
            $output .= $this->hr($length);
            ob_start();
            var_dump($var);
            $output .= trim(ob_get_clean());
        }

        return $output;
    }

    /**
     * Generate a horizontal rule.
     *
     * @param int $length
     * @return string
     */
    public function hr(int $length): string {
        return PHP_EOL . str_pad('', $length, '-', STR_PAD_BOTH) . PHP_EOL;
    }

    /**
     * {@inheritdoc}
     */
    public function inspect(Exception $exception): string {
        $output = [
            get_class($exception) . ' - ' . $exception->getMessage(),
            Path::alias($exception->getFile()) . ':' . $exception->getLine()
        ];

        $length = max(array_map(fun('strlen'), $output));

        $output = implode(PHP_EOL, $output);
        $output .= $this->hr($length);
        $output .= $this->backtrace($exception);

        return $output;
    }

}
