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
 * The Deprecated annotation marks a class as deprecated and will log a deprecation
 * message each time the class is instantiated.
 *
 * <<Deprecated('Message to log.')>>
 *
 * @package Titon\Debug\Annotation
 */
class Deprecated extends Annotation implements Wireable {

    /**
     * Message why it was deprecated.
     *
     * @var string
     */
    protected string $message;

    /**
     * Store the deprecation message.
     *
     * @param string $message
     */
    public function __construct(string $message) {
        $this->message = $message;
    }

    /**
     * Return the deprecation message.
     *
     * @return string
     */
    public function getMessage(): string {
        return $this->message;
    }

    /**
     * Log the deprecation message.
     *
     * @param T $class
     * @param string $method
     * @return $this
     */
    public function wire<T>(T $class, string $method = ''): this {
        Debugger::getLogger()?->log(Logger::NOTICE, '{class} is deprecated. {message}', [
            'class' => get_class($class),
            'message' => $this->getMessage()
        ]);

        return $this;
    }

}
