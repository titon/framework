<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Debug;

use Psr\Log\LoggerInterface;
use Titon\Debug\Dumper\CliDumper;
use Titon\Debug\Dumper\HtmlDumper;
use Titon\Debug\Exception\FatalErrorException;
use Titon\Debug\Exception\InternalErrorException;
use Titon\Type\Type;
use Titon\Utility\Sanitize;
use \Exception;
use \ErrorException;

/**
 * Custom system to manage all errors and thrown/uncaught exceptions.
 *
 * @package Titon\Debug
 */
class Debugger {

    /**
     * Dumper instance.
     *
     * @var \Titon\Debug\Dumper
     */
    protected static ?Dumper $dumper;

    /**
     * Complete list of all internal errors types.
     *
     * @var \Titon\Debug\ErrorCodeMap
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
     * @var \Titon\Debug\ExceptionHandler
     */
    protected static ?ExceptionHandler $handler;

    /**
     * Has the debugger been initialized?
     * This stops multiple calls to handler registers.
     *
     * @var bool
     */
    protected static bool $initialized = false;

    /**
     * Logger instance.
     *
     * @var \Psr\Log\LoggerInterface
     */
    protected static ?LoggerInterface $logger;

    /**
     * @see \Titon\Debug\Dumper::backtrace()
     */
    public static function backtrace(?Exception $exception = null): string {
        return (string) static::getDumper()->backtrace($exception);
    }

    /**
     * @see \Titon\Debug\Dumper::debug()
     */
    public static function debug(/* HH_FIXME[4033]: variadic + strict */ ...$vars): string {
        return (string) call_user_func_array(inst_meth(static::getDumper(), 'debug'), $vars);
    }

    /**
     * Disable error reporting.
     */
    public static function disable(): void {
        static::enable(false);
    }

    /**
     * @see \Titon\Debug\Dumper::dump()
     */
    public static function dump(/* HH_FIXME[4033]: variadic + strict */ ...$vars): string {
        return (string) call_user_func_array(inst_meth(static::getDumper(), 'dump'), $vars);
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
     * Export a formatted variable to be usable by PHP.
     *
     * @param mixed $var
     * @param bool $short
     * @return string
     */
    public static function export(mixed $var, bool $short = true): string {
        $export = var_export($var, true);

        if (is_array($var)) {
            $replace = [" (", "  ", "=> \n\t"];
            $with = ["(", "\t", "=> "];

            if ($short) {
                $replace[] = 'array(';
                $replace[] = '),';
                $replace[] = "\n)";
                $with[] = '[';
                $with[] = '],';
                $with[] = "\n]";
            }

            $export = str_replace($replace, $with, $export);
        }

        return $export;
    }

    /**
     * Return the file path and line number for the calling method.
     *
     * @param string $name
     * @return \Titon\Debug\Caller
     */
    public static function getCaller(string $name): Caller {
        $file = '';
        $line = 0;

        foreach (debug_backtrace() as $trace) {
            if (array_key_exists('function', $trace) && $trace['function'] === $name && array_key_exists('file', $trace)) {
                $file = $trace['file'];
                $line = (int) $trace['line'];
                break;
            }
        }

        return shape(
            'file' => $file,
            'line' => $line
        );
    }

    /**
     * Return the dumper. If no dumper has been defined, automatically set one.
     *
     * @return \Titon\Debug\Dumper
     */
    public static function getDumper(): Dumper {
        if (self::$dumper) {
            return self::$dumper;
        }

        if (substr(PHP_SAPI, 0, 3) === 'cli') {
            self::$dumper = new CliDumper();
        } else {
            self::$dumper = new HtmlDumper();
        }

        return self::$dumper;
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
        return static::$handler;
    }

    /**
     * Return the logger.
     *
     * @return \Psr\Log\LoggerInterface
     */
    public static function getLogger(): ?LoggerInterface {
        return static::$logger;
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
        if (static::isOff()) {
            static::logException($exception);

        // Output error in development
        } else {
            echo static::inspect($exception);
        }
    }

    /**
     * Handler for catching uncaught exceptions. By default will output a stack trace and stop execution.
     *
     * @param \Exception $exception
     */
    public static function handleException(Exception $exception): void {
        static::logException($exception);

        echo static::inspect($exception);

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

        if (static::isOn()) {
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
    public static function handleInvariant(string $message, /* HH_FIXME[4033]: variadic + strict */ ...$args): void {
        static::getLogger()?->log(Logger::INFO, $message, $args);
    }

    /**
     * Initialize error, exception, and debug handling.
     */
    public static function initialize(): void {
        if (static::$initialized) {
            return;
        }

        ini_set('log_errors', true);
        ini_set('report_memleaks', true);

        set_error_handler(class_meth(__CLASS__, 'handleError'));
        register_shutdown_function(class_meth(__CLASS__, 'handleFatalError'));
        invariant_callback_register(class_meth(__CLASS__, 'handleInvariant'));

        static::setHandler(class_meth(__CLASS__, 'handleException'));
        static::disable();

        static::$initialized = true;
    }

    /**
     * @see \Titon\Debug\Dumper::inspect()
     */
    public static function inspect(Exception $exception): string {
        return static::getDumper()->inspect($exception);
    }

    /**
     * Return true if error reporting is enabled.
     *
     * @return bool
     */
    public static function isOn(): bool {
        return (error_reporting() > 0);
    }

    /**
     * Return true if error reporting is disabled.
     *
     * @return bool
     */
    public static function isOff(): bool {
        return (error_reporting() <= 0);
    }

    /**
     * Log an uncaught exception.
     *
     * @param \Exception $exception
     */
    public static function logException(Exception $exception): void {
        $map = static::mapErrorCode($exception);

        $message = sprintf('%s: %s [%s:%s]',
            $map['error'],
            $exception->getMessage(),
            $exception->getFile(),
            $exception->getLine());

        static::getLogger()?->log($map['level'], $message, [
            'exception' => $exception
        ]);
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
            if (is_array($value)) {
                $open = '[';
                $close = ']';
            } else {
                $open = str_replace('HH\\', '', get_class($value)) . ' {';
                $close = '}';
            }

            if ($depth >= 3) {
                $var = $open . '...' . $close;

            } else {
                $values = [];
                $depth++;

                foreach ($value as $a) {
                    $values[] = static::parseValue($a, $wrap, $depth);
                }

                $var = $open . implode(', ', $values) . $close;
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
     * Set the dumper for outputting variables.
     *
     * @param \Titon\Debug\Dumper
     */
    public static function setDumper(Dumper $dumper): void {
        static::$dumper = $dumper;
    }

    /**
     * Set the exception handler.
     *
     * @param \Titon\Debug\ExceptionHandler $callback
     */
    public static function setHandler(ExceptionHandler $callback): void {
        static::$handler = $callback;

        set_exception_handler($callback);
    }

    /**
     * Set the logger to be used for debugging.
     *
     * @param \Psr\Log\LoggerInterface $logger
     */
    public static function setLogger(LoggerInterface $logger): void {
        static::$logger = $logger;
    }

}
