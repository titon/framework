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
use Titon\Utility\State\Server;
use Titon\Utility\Str;
use \DateTime;
use \Exception;

/**
 * Default logger that inherits functionality from the PSR-3 Logger interfaces.
 * Will log messages to the local file system.
 *
 * @package Titon\Debug
 */
class Logger extends AbstractLogger {

    const string EMERGENCY = LogLevel::EMERGENCY; // 0
    const string ALERT = LogLevel::ALERT; // 1
    const string CRITICAL = LogLevel::CRITICAL; // 2
    const string ERROR = LogLevel::ERROR; // 3
    const string WARNING = LogLevel::WARNING; // 4
    const string NOTICE = LogLevel::NOTICE; // 5
    const string INFO = LogLevel::INFO; // 6
    const string DEBUG = LogLevel::DEBUG; // 7

    /**
     * Directory to store log files.
     *
     * @type string
     */
    protected string $_directory;

    /**
     * Set the directory to log to.
     *
     * @uses Titon\Utility\Path
     *
     * @param string $dir
     * @throws \Titon\Debug\Exception\InvalidDirectoryException
     * @throws \Titon\Debug\Exception\UnwritableDirectoryException
     */
    public function __construct(string $dir) {
        if (!is_dir($dir)) {
            throw new InvalidDirectoryException(sprintf('Invalid log directory %s', $dir));

        } else if (!is_writable($dir)) {
            throw new UnwritableDirectoryException('Log directory is not writable');
        }

        $this->_directory = Path::ds($dir, true);
    }

    /**
     * Return the directory.
     *
     * @return string
     */
    public function getDirectory(): string {
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
     * @param string $level
     * @param string $message
     * @param array $context
     * @return bool
     */
    public function log($level, $message, array<string, mixed> $context = []): bool { // No type hints as it inherits from PSR
        return (bool) file_put_contents(
            sprintf('%s/%s-%s.log', $this->getDirectory(), $level, date('Y-m-d')),
            static::createMessage($level, $message, $context),
            FILE_APPEND);
    }

    /**
     * Sharable message parsing and building method. Conforms to the PSR spec.
     *
     * @uses Titon\Utility\Str
     *
     * @param string $level
     * @param string $message
     * @param array $context
     * @return string
     */
    public static function createMessage(string $level, string $message, array<string, mixed> $context = []): string {
        $exception = null;
        $url = '';

        if (isset($context['exception'])) {
            $exception = $context['exception'];
            unset($context['exception']);
        }

        if (isset($context['url'])) {
            $url = $context['url'];

        } else if ($pathInfo = Server::get('PATH_INFO')) {
            $url = $pathInfo;

        } else if ($reqUri = Server::get('REQUEST_URI')) {
            $url = $reqUri;
        }

        $message = sprintf('[%s] %s %s',
            date(DateTime::RFC3339),
            Str::insert($message, new Map($context), Map {'escape' => false}),
            $url ? '[' . (string) $url . ']' : '') . PHP_EOL;

        if ($exception instanceof Exception) {
            $message .= $exception->getTraceAsString() . PHP_EOL;
        }

        return $message;
    }

}
