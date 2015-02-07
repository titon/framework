<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Debug\Dumper;

use Titon\Debug\Debugger;
use Titon\Debug\Dumper;
use Exception;

/**
 * The HtmlDumper is used for outputting variables in HTML while in a browser.
 *
 * @package Titon\Debug\Dumper
 */
class HtmlDumper implements Dumper {

    /**
     * {@inheritdoc}
     */
    public function backtrace(?Exception $exception = null): string {
        if ($exception) {
            $stack = $exception->getTrace();
        } else {
            $stack = debug_backtrace();
        }

        $backtrace = [];

        foreach ($stack as $trace) {
            if (in_array($trace['function'], get_class_methods(__CLASS__)) || (array_key_exists('file', $trace) && strpos($trace['file'], 'Debugger'))) {
                continue;
            }

            $current = [
                'line' => '',
                'method' => '',
                'file' => 0,
                'args' => []
            ];

            $current['file'] = array_key_exists('file', $trace) ? $trace['file'] : '[internal]';
            $current['line'] = array_key_exists('line', $trace) ? (int) $trace['line'] : 0;

            if (array_key_exists('class', $trace)) {
                $method = $trace['class'] . $trace['type'] . $trace['function'];
            } else {
                $method = $trace['function'];
            }

            if (strpos($method, '{closure}') === false) {
                $method .= '()';
            }

            $current['method'] = $method;
            $current['args'] = $trace['args'];

            $backtrace[] = $current;
        }

        return static::renderTemplate('backtrace', [
            'backtrace' => $backtrace
        ]);
    }

    /**
     * {@inheritdoc}
     */
    public function debug(...$vars): string {
        $caller = Debugger::getCaller('debug');

        return static::renderTemplate('debug', [
            'file' => $caller['file'],
            'line' => $caller['line'],
            'vars' => $vars
        ]);
    }

    /**
     * {@inheritdoc}
     */
    public function dump(...$vars): string {
        $caller = Debugger::getCaller('dump');

        return static::renderTemplate('debug', [
            'file' => $caller['file'],
            'line' => $caller['line'],
            'vars' => $vars,
            'dump' => true
        ]);
    }

    /**
     * {@inheritdoc}
     */
    public function inspect(Exception $exception): string {
        return static::renderTemplate('inspect', [
            'exception' => $exception
        ]);
    }

    /**
     * Use a local template for outputting debug and error markup.
     *
     * @param string $template
     * @param array $variables
     * @return string
     */
    public static function renderTemplate(string $template, array<string, mixed> $variables = []): string {
        return render_template(sprintf('%s/templates/%s.php', dirname(__DIR__), $template), $variables);
    }

}
