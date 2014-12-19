<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Debug;

use Psr\Log\LoggerInterface;
use Titon\Debug\Exception\FatalErrorException;
use Titon\Debug\Exception\InternalErrorException;
use Titon\Type\Type;
use Titon\Utility\Sanitize;
use \Exception;
use \ErrorException;

type Error = shape('error' => string, 'level' => string);
type ErrorCodeMap = Map<int, string>;
type ExceptionHandler = (function(Exception): void);

/**
 * Custom system to manage all errors and thrown/uncaught exceptions.
 *
 * @package Titon\Debug
 */
class Debugger {

    /**
     * Complete list of all internal errors types.
     *
     * @type \Titon\Debug\ErrorCodeMap
     */
    public static ErrorCodeMap $errorTypes = Map {
        E_ERROR             => 'Error',
        E_WARNING           => 'Warning',
        E_PARSE             => 'Parsing Error',
        E_NOTICE            => 'Notice',
        E_CORE_ERROR        => 'Core Error',
        E_CORE_WARNING      => 'Core Warning',
        E_COMPILE_ERROR     => 'Compile Error',
        E_COMPILE_WARNING   => 'Compile Warning',
        E_USER_ERROR        => 'User Error',
        E_USER_WARNING      => 'User Warning',
        E_USER_NOTICE       => 'User Notice',
        E_STRICT            => 'Strict Notice',
        E_RECOVERABLE_ERROR => 'Catchable Fatal Error',
        E_DEPRECATED        => 'Deprecated',
        E_USER_DEPRECATED   => 'User Deprecated',
        E_ALL               => 'All'
    };

    /**
     * Callback to handle exceptions.
     *
     * @type \Titon\Debug\ExceptionHandler
     */
    protected static ?ExceptionHandler $_handler;

    /**
     * Has the debugger been initialized?
     * This stops multiple calls to handler registers.
     *
     * @type bool
     */
    protected static bool $_initialized = false;

    /**
     * Logger instance.
     *
     * @type \Psr\Log\LoggerInterface
     */
    protected static ?LoggerInterface $_logger;

    /**
     * Initialize the error, exception, and debug handling.
     */
    public static function initialize(): void {
        if (static::$_initialized) {
            return;
        }

        ini_set('log_errors', true);
        ini_set('report_memleaks', true);

        set_error_handler(class_meth(__CLASS__, 'handleError'));
        register_shutdown_function(class_meth(__CLASS__, 'handleFatalError'));
        invariant_callback_register(class_meth(__CLASS__, 'handleInvariant'));

        static::setHandler(class_meth(__CLASS__, 'handleException'));
        static::enable(false);

        static::$_initialized = true;
    }

    /**
     * Create a custom backtraced array based on the debug_backtrace() output.
     *
     * @param \Exception $exception
     * @return string
     */
    public static function backtrace(?Exception $exception = null): string {
        if (error_reporting() <= 0) {
            return '';
        }

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

            $current['file'] = array_key_exists('file', $trace) ? $trace['file'] : '[Internal]';
            $current['line'] = array_key_exists('line', $trace) ? $trace['line'] : 0;

            $method = $trace['function'];

            if (array_key_exists('class', $trace)) {
                $method = $trace['class'] . $trace['type'] . $method;
            }

            if (strpos($method, '{closure}') === false) {
                $method .= '()';
            }

            $current['method'] = $method;
            $current['args'] = $trace['args'];

            $backtrace[] = $current;
        }

        return static::renderTemplate('backtrace', [
            'backtrace' => array_reverse($backtrace)
        ]);
    }

    /**
     * Outputs multiple variables using print_r().
     *
     * @param mixed $var,...
     * @return string
     */
    public static function debug(): string {
        if (error_reporting() <= 0) {
            return '';
        }

        $file = '';
        $line = 0;

        foreach (debug_backtrace() as $trace) {
            if (array_key_exists('function', $trace) && $trace['function'] === 'debug' && array_key_exists('file', $trace)) {
                $file = $trace['file'];
                $line = $trace['line'];
                break;
            }
        }

        return static::renderTemplate('debug', [
            'file' => $file,
            'line' => $line,
            'vars' => func_get_args()
        ]);
    }

    /**
     * Disable debugging.
     */
    public static function disable(): void {
        static::enable(false);
    }

    /**
     * Output multiple variables in a nested HTML table.
     *
     * @param mixed $var,...
     * @return string
     */
    public static function dump(): string {
        if (error_reporting() <= 0) {
            return '';
        }

        $file = '';
        $line = 0;

        foreach (debug_backtrace() as $trace) {
            if (array_key_exists('function', $trace) && $trace['function'] === 'dump' && array_key_exists('file', $trace)) {
                $file = $trace['file'];
                $line = $trace['line'];
                break;
            }
        }

        return static::renderTemplate('debug', [
            'file' => $file,
            'line' => $line,
            'vars' => func_get_args(),
            'dump' => true
        ]);
    }

    /**
     * Enable or disable error reporting dynamically during runtime.
     *
     * @param bool $enabled
     */
    public static function enable(bool $enabled = true): void {
        if ($enabled) {
            ini_set('error_reporting', ($enabled === true) ? E_ALL | E_STRICT : $enabled);
        } else {
            ini_set('error_reporting', 0);
        }

        ini_set('display_errors', $enabled);
        ini_set('display_startup_errors', $enabled);
        ini_set('track_errors', $enabled);
    }

    /**
     * Export a formatted variable to be used.
     *
     * @param mixed $var
     * @param bool $short
     * @return string
     */
    public static function export(mixed $var, bool $short = true): string {
        if (error_reporting() <= 0) {
            return '';
        }

        $export = var_export($var, true);

        if (is_array($var)) {
            $replace = [" (", "  ", "=> \n\t"];
            $with = ["(", "\t", "=> "];

            if ($short) {
                $replace[] = 'array(';
                $replace[] = '),';
                $replace[] = "\n)";
                $with[] = '[';
                $with[] = ']';
                $with[] = "\n]";
            }

            $export = str_replace($replace, $with, $export);
        }

        return $export;
    }

    /**
     * Determine the type of error received.
     *
     * @param int $code
     * @return string
     */
    public static function getError(int $code = 0): string {
        if (static::$errorTypes->contains($code)) {
            return static::$errorTypes[$code];
        }

        return 'Unknown Error';
    }

    /**
     * Return the current exception handler.
     *
     * @return \Titon\Debug\ExceptionHandler
     */
    public static function getHandler(): ?ExceptionHandler {
        return static::$_handler;
    }

    /**
     * Return the logger.
     *
     * @return \Psr\Log\LoggerInterface
     */
    public static function getLogger(): ?LoggerInterface {
        return static::$_logger;
    }

    /**
     * Overwrite the error handler and convert errors into ErrorExceptions.
     * All fatal/errors should be thrown and handled by the exception handler,
     * while all notices/warnings should be logged or printed.
     *
     * @param int $code
     * @param string $message
     * @param string $file
     * @param int $line
     * @return string
     * @throws \ErrorException
     */
    public static function handleError(int $code, string $message, string $file = '', int $line = 0): void {
        $exception = new ErrorException($message, $code, 0, $file, $line);

        switch ($code) {
            case E_ERROR:
            case E_PARSE:
            case E_CORE_ERROR:
            case E_COMPILE_ERROR:
            case E_USER_ERROR:
            case E_RECOVERABLE_ERROR:
                throw $exception;
            break;
        }

        // Log error in production
        if (error_reporting() <= 0) {
            static::logException($exception);

        // Output error in development
        } else {
            echo static::printException($exception);
        }
    }

    /**
     * Handler for catching uncaught exceptions. By default will output a stack trace and stop execution.
     *
     * @param \Exception $exception
     */
    public static function handleException(Exception $exception): void {
        static::logException($exception);

        echo static::printException($exception);

        exit();
    }

    /**
     * Handle fatal errors by passing an ErrorException to the custom exception handler.
     */
    public static function handleFatalError(): void {
        $error = error_get_last();
        $handler = static::getHandler();

        if ($error['type'] != E_ERROR || !$handler) {
            return;
        }

        if (error_reporting() > 0) {
            $exception = new FatalErrorException($error['message'], E_ERROR, 1, $error['file'], $error['line']);
        } else {
            $exception = new InternalErrorException($error['message'], E_ERROR, 1, $error['file'], $error['line']);
        }

        call_user_func($handler, $exception);
    }

    /**
     * Handle invariant violations by logging failures.
     *
     * @param string $message
     * @param array $args
     */
    public static function handleInvariant(string $message, ...$args): void {
        if ($logger = static::getLogger()) {
            $logger->log(Logger::INFO, $message, $args);
        }
    }

    /**
     * Log an uncaught exception.
     *
     * @param \Exception $exception
     */
    public static function logException(Exception $exception): void {
        if ($logger = static::getLogger()) {
            $map = static::mapErrorCode($exception);

            $message = sprintf('%s: %s [%s:%s]',
                $map['error'],
                $exception->getMessage(),
                $exception->getFile(),
                $exception->getLine());

            $logger->log($map['level'], $message, [
                'exception' => $exception
            ]);
        }
    }

    /**
     * Map an error code the a specific log level.
     *
     * @uses Titon\Debug\Logger
     *
     * @param int|\Exception $code
     * @return \Titon\Debug\Error
     */
    public static function mapErrorCode(mixed $code): Error {
        $error = '';

        if ($code instanceof Exception) {
            $class = get_class($code);

            if ($class !== 'ErrorException') {
                $error = $class;
            }

            $code = (int) $code->getCode();
        } else {
            $code = (int) $code;
        }

        if (!$error) {
            $error = static::getError($code);
        }

        switch ($code) {
            case E_ERROR:
            case E_PARSE:
            case E_CORE_ERROR:
            case E_COMPILE_ERROR:
            case E_USER_ERROR:
                $level = Logger::ERROR;
            break;
            case E_WARNING:
            case E_CORE_WARNING:
            case E_COMPILE_WARNING:
            case E_USER_WARNING:
            case E_RECOVERABLE_ERROR:
                $level = Logger::WARNING;
            break;
            case E_STRICT:
            case E_DEPRECATED:
            case E_USER_DEPRECATED:
                $level = Logger::INFO;
            break;
            case E_NOTICE:
            case E_USER_NOTICE:
                $level = Logger::NOTICE;
            break;
            default:
                $level = Logger::DEBUG;
            break;
        }

        return shape(
            'error' => $error,
            'level' => $level
        );
    }

    /**
     * Parse a value and return a string for the type of value.
     *
     * @param mixed $value
     * @return string
     */
    public static function parseType(mixed $value): string {
        return Type::is($value);
    }

    /**
     * Parse a value and return a formatted value.
     * If wrap is true, the value will be wrapped in a span with a class of that type.
     *
     * @uses Titon\Utility\Sanitize
     *
     * @param mixed $value
     * @param bool $wrap
     * @param int $depth
     * @return string
     */
    public static function parseValue(mixed $value, bool $wrap = false, int $depth = 0): string {
        $var = '';

        if (is_numeric($value)) {
            $var = (string) $value;

        } else if (is_bool($value)) {
            $var = $value ? 'true' : 'false';

        } else if (is_string($value)) {
            $var = '"' . Sanitize::escape($value, Map {'flags' => ENT_NOQUOTES}) . '"';

        } else if ($value instanceof Traversable) {
            if ($depth >= 3) {
                $var = '[...]';

            } else {
                $values = [];
                $depth++;

                foreach ($value as $a) {
                    $values[] = static::parseValue($a, $wrap, $depth);
                }

                $var = '[' . implode(', ', $values) . ']';
            }

        } else if (is_null($value)) {
            $var = 'null';

        } else if (is_object($value)) {
            $var = '{' . get_class($value) . '}';

        } else if (is_resource($value)) {
            $var = strtolower(get_resource_type($value));
        }

        if ($wrap) {
            return sprintf('<span class="debug-type %s">%s</span>', static::parseType($value), $var);
        }

        return $var;
    }

    /**
     * Renders a formatted error message to the view accompanied by a stack trace.
     *
     * @param \Exception $exception
     * @return string
     */
    public static function printException(Exception $exception): string {
        if (error_reporting() <= 0) {
            return '';
        }

        return static::renderTemplate('error', [
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
        return render_template(sprintf('%s/templates/%s.php', __DIR__, $template), $variables);
    }

    /**
     * Set the exception handler.
     *
     * @param \Titon\Debug\ExceptionHandler $callback
     */
    public static function setHandler(ExceptionHandler $callback): void {
        static::$_handler = $callback;

        set_exception_handler($callback);
    }

    /**
     * Set the logger to be used for debugging.
     *
     * @param \Psr\Log\LoggerInterface $logger
     */
    public static function setLogger(LoggerInterface $logger): void {
        static::$_logger = $logger;
    }

}
