<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Debug;

use Psr\Log\LoggerInterface;
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
     * Complete list of all internal errors types.
     *
     * @type array
     */
    public static $errorTypes = [
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
    ];

    /**
     * Callback to handle exceptions.
     *
     * @type callable
     */
    protected static $_handler;

    /**
     * Logger instance.
     *
     * @type \Psr\Log\LoggerInterface
     */
    protected static $_logger;

    /**
     * Initialize the error, exception, and debug handling.
     */
    public static function initialize() {
        ini_set('log_errors', true);
        ini_set('report_memleaks', true);

        set_error_handler([__CLASS__, 'handleError']);
        register_shutdown_function([__CLASS__, 'handleFatalError']);

        static::setHandler([__CLASS__, 'handleException']);
        static::enable(false);
    }

    /**
     * Create a custom backtraced array based on the debug_backtrace() output.
     *
     * @param \Exception $exception
     * @return string
     */
    public static function backtrace(Exception $exception = null) {
        if (error_reporting() <= 0) {
            return null;
        }

        if ($exception) {
            $stack = $exception->getTrace();
        } else {
            $stack = debug_backtrace();
        }

        $backtrace = [];

        foreach ($stack as $trace) {
            if (in_array($trace['function'], get_class_methods(__CLASS__)) || (isset($trace['file']) && strpos($trace['file'], 'Debugger'))) {
                continue;
            }

            $current = [
                'line' => null,
                'method' => null,
                'file' => null,
                'args' => null
            ];

            $current['file'] = isset($trace['file']) ? $trace['file'] : '[Internal]';
            $current['line'] = isset($trace['line']) ? $trace['line'] : 0;

            $method = $trace['function'];

            if (isset($trace['class'])) {
                $method = $trace['class'] . $trace['type'] . $method;
            }

            if (strpos($method, '{closure}') === false) {
                $method .= '()';
            }

            $current['method'] = $method;
            $current['args'] = $trace['args'];

            $backtrace[] = $current;
        }

        return static::_renderTemplate('backtrace', [
            'backtrace' => array_reverse($backtrace)
        ]);
    }

    /**
     * Outputs multiple variables using print_r().
     *
     * @param mixed $var,...
     * @return string
     */
    public static function debug() {
        if (error_reporting() <= 0) {
            return null;
        }

        $file = $line = null;

        foreach (debug_backtrace() as $trace) {
            if (isset($trace['function']) && $trace['function'] === 'debug' && isset($trace['file'])) {
                $file = $trace['file'];
                $line = $trace['line'];
                break;
            }
        }

        return static::_renderTemplate('debug', [
            'file' => $file,
            'line' => $line,
            'vars' => func_get_args()
        ]);
    }

    /**
     * Output multiple variables in a nested HTML table.
     *
     * @param mixed $var,...
     * @return string
     */
    public static function dump() {
        if (error_reporting() <= 0) {
            return null;
        }

        $file = $line = null;

        foreach (debug_backtrace() as $trace) {
            if (isset($trace['function']) && $trace['function'] === 'dump' && isset($trace['file'])) {
                $file = $trace['file'];
                $line = $trace['line'];
                break;
            }
        }

        return static::_renderTemplate('debug', [
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
    public static function enable($enabled = true) {
        $enabled = (bool) $enabled;

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
    public static function export($var, $short = true) {
        if (error_reporting() <= 0) {
            return null;
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
    public static function getError($code = null) {
        if (isset(static::$errorTypes[$code])) {
            return static::$errorTypes[$code];
        }

        return 'Unknown Error';
    }

    /**
     * Return the current exception handler.
     *
     * @return callable
     */
    public static function getHandler() {
        return static::$_handler;
    }

    /**
     * Return the logger.
     *
     * @return \Psr\Log\LoggerInterface
     */
    public static function getLogger() {
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
    public static function handleError($code, $message, $file = null, $line = null) {
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
     * @codeCoverageIgnore
     */
    public static function handleException(Exception $exception) {
        static::logException($exception);
        echo static::printException($exception);
        exit();
    }

    /**
     * Handle fatal errors by passing an ErrorException to the custom exception handler.
     *
     * @codeCoverageIgnore
     */
    public static function handleFatalError() {
        $error = error_get_last();

        if ($error['type'] != E_ERROR) {
            return;
        }

        if (error_reporting() > 0) {
            $exception = 'Titon\Debug\Exception\FatalErrorException';
        } else {
            $exception = 'Titon\Debug\Exception\InternalErrorException';
        }

        call_user_func(static::getHandler(), new $exception($error['message'], E_ERROR, 1, $error['file'], $error['line']));
    }

    /**
     * Log an uncaught exception.
     *
     * @param \Exception $exception
     */
    public static function logException(Exception $exception) {
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
     * @return array
     */
    public static function mapErrorCode($code) {
        $log = Logger::DEBUG;
        $error = null;

        if ($code instanceof Exception) {
            $class = get_class($code);

            if ($class !== 'ErrorException') {
                $error = $class;
            }

            $code = $code->getCode();
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
                $log = Logger::ERROR;
            break;
            case E_WARNING:
            case E_CORE_WARNING:
            case E_COMPILE_WARNING:
            case E_USER_WARNING:
            case E_RECOVERABLE_ERROR:
                $log = Logger::WARNING;
            break;
            case E_STRICT:
            case E_DEPRECATED:
            case E_USER_DEPRECATED:
                $log = Logger::INFO;
            break;
            case E_NOTICE:
            case E_USER_NOTICE:
                $log = Logger::NOTICE;
            break;
        }

        return [
            'error' => $error,
            'level' => $log
        ];
    }

    /**
     * Parse a value and return a string for the type of value.
     *
     * @param mixed $value
     * @return string
     */
    public static function parseType($value) {
        if (is_callable($value)) {
            return 'callable';
        }

        return strtolower(gettype($value));
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
    public static function parseValue($value, $wrap = false, $depth = 0) {
        $var = null;

        if (is_numeric($value)) {
            $var = $value;

        } else if (is_bool($value)) {
            $var = $value ? 'true' : 'false';

        } else if (is_string($value)) {
            $var = '"' . Sanitize::escape($value, ['flags' => ENT_NOQUOTES]) . '"';

        } else if (is_array($value)) {
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
            $var = mb_strtolower(get_resource_type($value));
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
    public static function printException(Exception $exception) {
        if (error_reporting() <= 0) {
            return null;
        }

        return static::_renderTemplate('error', [
            'exception' => $exception
        ]);
    }

    /**
     * Set the exception handler.
     *
     * @param callable $callback
     */
    public static function setHandler($callback) {
        static::$_handler = $callback;

        set_exception_handler($callback);
    }

    /**
     * Set the logger to be used for debugging.
     *
     * @param \Psr\Log\LoggerInterface $logger
     */
    public static function setLogger(LoggerInterface $logger) {
        static::$_logger = $logger;
    }

    /**
     * Use a local template for outputting debug and error markup.
     *
     * @param string $template
     * @param array $variables
     * @return string
     */
    protected static function _renderTemplate($template, array $variables = []) {
        if ($variables) {
            extract($variables, EXTR_OVERWRITE);
        }

        ob_start();

        include sprintf('%s/views/%s.php', dirname(dirname(__DIR__)), $template);

        return ob_get_clean();
    }

}
