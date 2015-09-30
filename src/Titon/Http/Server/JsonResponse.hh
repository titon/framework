<?hh // partial
// Because of PSR HTTP Message
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Psr\Http\Message\StreamInterface;
use Titon\Http\Exception\MalformedResponseException;
use Titon\Http\HeaderMap;
use Titon\Http\StatusCode;
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
    protected string $callback = '';

    /**
     * Set the body, status code, and optional JSON encoding options.
     * If no options are defined, fallback to escaping standard entities.
     * Also convert the resource to JSON. If an error arises, throw an exception.
     *
     * @param mixed $body
     * @param int $status
     * @param int $flags
     * @param string $callback
     * @param \Titon\Http\HeaderMap $headers
     */
    public function __construct(mixed $body, int $status = StatusCode::OK, int $flags = -1, string $callback = '', HeaderMap $headers = Map {}) {
        $this->callback = $callback;

        // Convert the body to JSON
        if (!$body instanceof StreamInterface) {
            $body = new MemoryStream($this->encode($body, $flags));
        }

        // Wrap the body if the callback is defined
        if ($callback) {
            $body = new MemoryStream(sprintf('%s(%s);', $callback, (string) $body));
            $headers['Content-Type'] = [$this->validateContentType('js')]; // Older browsers

        } else {
            $headers['Content-Type'] = [$this->validateContentType('json')];
        }

        parent::__construct($body, $status, $headers);

    }

    /**
     * Return the JSONP callback.
     *
     * @return string
     */
    public function getCallback(): string {
        return $this->callback;
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
     * Encode the data into valid JSON.
     *
     * @param mixed $data
     * @param int $flags
     * @return string
     * @throws \Titon\Http\Exception\MalformedResponseException
     */
    public function encode(mixed $data, int $flags = -1): string {
        if ($flags === -1) {
            $flags = JSON_HEX_TAG | JSON_HEX_APOS | JSON_HEX_QUOT | JSON_HEX_AMP;
        }

        $json = json_encode($data, $flags);

        if (json_last_error() !== JSON_ERROR_NONE) {
            throw new MalformedResponseException($this->getErrorMessage());
        }

        return $json;
    }

}
