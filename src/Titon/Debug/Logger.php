<?hh
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Debug;

use Psr\Log\AbstractLogger;
use Psr\Log\LogLevel;
use Titon\Debug\Exception\InvalidDirectoryException;
use Titon\Debug\Exception\UnwritableDirectoryException;
use Titon\Utility\Path;
use Titon\Utility\String;
use \DateTime;
use \Exception;

/**
 * Default logger that inherits functionality from the PSR-3 Logger interfaces.
 * Will log messages to the local file system.
 *
 * @package Titon\Debug
 */
class Logger extends AbstractLogger {

    const EMERGENCY = LogLevel::EMERGENCY; // 0
    const ALERT = LogLevel::ALERT; // 1
    const CRITICAL = LogLevel::CRITICAL; // 2
    const ERROR = LogLevel::ERROR; // 3
    const WARNING = LogLevel::WARNING; // 4
    const NOTICE = LogLevel::NOTICE; // 5
    const INFO = LogLevel::INFO; // 6
    const DEBUG = LogLevel::DEBUG; // 7

    /**
     * Directory to store log files.
     *
     * @type string
     */
    protected $_directory;

    /**
     * Set the directory to log to.
     *
     * @uses Titon\Utility\Path
     *
     * @param string $dir
     * @throws \Titon\Debug\Exception\InvalidDirectoryException
     * @throws \Titon\Debug\Exception\UnwritableDirectoryException
     */
    public function __construct($dir) {
        if (!is_dir($dir)) {
            throw new InvalidDirectoryException(sprintf('Invalid log directory %s', $dir));

        } else if (!is_writable($dir)) {
            throw new UnwritableDirectoryException('Log directory is not writable');
        }

        $this->_directory = $dir;
    }

    /**
     * Return the directory.
     *
     * @return string
     */
    public function getDirectory() {
        return $this->_directory;
    }

    /**
     * Log a message by default to the file system. The message will be sent to the directory
     * passed through the constructor into a file that matches the level name.
     * Should conform to PSR-3 and RFC 5424.
     *
     * @link https://github.com/php-fig/fig-standards/blob/master/accepted/PSR-3-logger-interface.md
     * @link http://tools.ietf.org/html/rfc5424
     *
     * @uses Titon\Utility\String
     *
     * @param string $level
     * @param string $message
     * @param array $context
     * @return int|bool
     */
    public function log($level, $message, array $context = []) {
        return file_put_contents(
            sprintf('%s/%s-%s.log', $this->getDirectory(), $level, date('Y-m-d')),
            static::createMessage($level, $message, $context),
            FILE_APPEND);
    }

    /**
     * Sharable message parsing and building method. Conforms to the PSR spec.
     *
     * @param string $level
     * @param string $message
     * @param array $context
     * @return string
     */
    public static function createMessage($level, $message, array $context = []) {
        $exception = null;
        $url = null;

        if (isset($context['exception'])) {
            $exception = $context['exception'];
            unset($context['exception']);
        }

        if (isset($context['url'])) {
            $url = $context['url'];
        } else if (isset($_SERVER['PATH_INFO'])) {
            $url = $_SERVER['PATH_INFO'];
        } else if (isset($_SERVER['REQUEST_URI'])) {
            $url = $_SERVER['REQUEST_URI'];
        }

        $message = sprintf('[%s] %s %s',
            date(DateTime::RFC3339),
            String::insert($message, $context, ['escape' => false]),
            $url ? '[' . $url . ']' : '') . PHP_EOL;

        if ($exception instanceof Exception) {
            $message .= $exception->getTraceAsString() . PHP_EOL;
        }

        return $message;
    }

}
