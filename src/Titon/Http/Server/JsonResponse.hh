<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Psr\Http\Message\StreamableInterface;
use Titon\Http\Http;
use Titon\Http\Exception\MalformedResponseException;
use Titon\Http\Stream\MemoryStream;

/**
 * Output JSON as the response by converting any type of resource to JSON.
 * Has optional support for outputting as JSONP by defining a callback function.
 *
 * @package Titon\Http\Server
 */
class JsonResponse extends Response {

    /**
     * JSONP callback function.
     *
     * @var string
     */
    protected string $_callback = '';

    /**
     * Set the body, status code, and optional JSON encoding options.
     * If no options are defined, fallback to escaping standard entities.
     * Also convert the resource to JSON. If an error arises, throw an exception.
     *
     * @param mixed $body
     * @param int $status
     * @param int $flags
     * @param string $callback
     * @throws \Titon\Http\Exception\MalformedResponseException
     */
    public function __construct(mixed $body, int $status = Http::OK, int $flags = -1, string $callback = '') {
        if ($flags === -1) {
            $flags = JSON_HEX_TAG | JSON_HEX_APOS | JSON_HEX_QUOT | JSON_HEX_AMP;
        }

        if (!$body instanceof StreamableInterface) {
            $body = new MemoryStream(json_encode($body, $flags));
        }

        parent::__construct($body, $status);

        if (json_last_error() !== JSON_ERROR_NONE) {
            throw new MalformedResponseException($this->getErrorMessage());
        }

        $this->setCallback($callback);
    }

    /**
     * Return the JSONP callback.
     *
     * @return string
     */
    public function getCallback(): string {
        return $this->_callback;
    }

    /**
     * Return an error message for the last occurring JSON error.
     *
     * @return string
     */
    public function getErrorMessage(): string {
        if (function_exists('json_last_error_msg')) {
            return json_last_error_msg();
        }

        $error = json_last_error();
        $messages = Map {
            JSON_ERROR_NONE             => '',
            JSON_ERROR_DEPTH            => 'Maximum stack depth exceeded',
            JSON_ERROR_STATE_MISMATCH   => 'Underflow or the modes mismatch',
            JSON_ERROR_CTRL_CHAR        => 'Unexpected control character found',
            JSON_ERROR_SYNTAX           => 'Syntax error, malformed JSON',
            JSON_ERROR_UTF8             => 'Malformed UTF-8 characters, possibly incorrectly encoded'
        };

        if ($messages->contains($error)) {
            return (string) $messages[$error];
        }

        return sprintf('Unknown error (%s)', $error);
    }

    /**
     * Set the JSONP callback function name.
     *
     * @param string $callback
     * @return $this
     */
    public function setCallback(string $callback): this {
        $this->_callback = $callback;

        return $this;
    }

    /**
     * Before sending, wrap the body in the JSONP callback if one has been defined.
     * Update the content type and length depending on the type of operation.
     *
     * @return string
     */
    public function send(): string {
        if ($callback = $this->getCallback()) {
            $this->setBody(new MemoryStream(sprintf('%s(%s);', $callback, (string) $this->getBody())));
            $this->contentType('text/javascript'); // Older browsers
        } else {
            $this->contentType('application/json');
        }

        if ($body = $this->getBody()) {
            $this->contentLength($body->getSize());
        }

        return parent::send();
    }

}