<?hh
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Titon\Http\Exception\MalformedResponseException;
use Titon\Http\Http;
use Titon\Utility\Converter;

/**
 * Output JSON as the response by converting any type of resource to JSON.
 * Has optional support for outputting as JSONP by defining a callback function.
 *
 * @package Titon\Http\Server
 */
class JsonResponse extends Response {

    /**
     * Configuration.
     *
     * @type array {
     *      @type int $flags    JSON encoding options
     * }
     */
    protected $_config = [
        'flags' => 0
    ];

    /**
     * JSONP callback function.
     *
     * @type string
     */
    protected $_callback;

    /**
     * Set the body, status code, and optional JSON encoding options.
     * If no options are defined, fallback to escaping standard entities.
     *
     * @param string $body
     * @param int $status
     * @param int $flags
     * @param string $callback
     */
    public function __construct($body = '', $status = Http::OK, $flags = null, $callback = null) {
        if ($flags === null) {
            $flags = JSON_HEX_TAG | JSON_HEX_APOS | JSON_HEX_QUOT | JSON_HEX_AMP;
        }

        parent::__construct($body, $status, ['flags' => $flags]);

        $this->setCallback($callback);
    }

    /**
     * Return the JSONP callback.
     *
     * @return string
     */
    public function getCallback() {
        return $this->_callback;
    }

    /**
     * Return an error message for the last occurring JSON error.
     *
     * @return string
     * @codeCoverageIgnore
     */
    public function getErrorMessage() {
        if (function_exists('json_last_error_msg')) {
            return json_last_error_msg();
        }

        $error = json_last_error();
        $messages = [
            JSON_ERROR_NONE             => true,
            JSON_ERROR_DEPTH            => 'Maximum stack depth exceeded',
            JSON_ERROR_STATE_MISMATCH   => 'Underflow or the modes mismatch',
            JSON_ERROR_CTRL_CHAR        => 'Unexpected control character found',
            JSON_ERROR_SYNTAX           => 'Syntax error, malformed JSON',
            JSON_ERROR_UTF8             => 'Malformed UTF-8 characters, possibly incorrectly encoded'
        ];

        if (isset($messages[$error])) {
            return $messages[$error];
        }

        return sprintf('Unknown error (%s)', $error);
    }

    /**
     * Convert the resource to JSON. If an error arises, throw an exception.
     *
     * @param string $body
     * @return $this
     * @throws \Titon\Http\Exception\MalformedResponseException
     */
    public function setBody($body = null) {
        $this->_body = Converter::toJson($body, $this->getConfig('flags'));

        // @codeCoverageIgnoreStart
        if (json_last_error() !== JSON_ERROR_NONE) {
            throw new MalformedResponseException($this->getErrorMessage());
        }
        // @codeCoverageIgnoreEnd

        return $this;
    }

    /**
     * Set the JSONP callback function name.
     *
     * @param string $callback
     * @return $this
     */
    public function setCallback($callback) {
        $this->_callback = (string) $callback;

        return $this;
    }

    /**
     * Before sending, wrap the body in the JSONP callback if one has been defined.
     * Update the content type and length depending on the type of operation.
     *
     * @return string
     */
    public function send() {
        if ($callback = $this->getCallback()) {
            $this->_body = sprintf('%s(%s);', $callback, $this->getBody());
            $this->contentType('text/javascript'); // older browsers
        } else {
            $this->contentType('application/json');
        }

        $this->contentLength(mb_strlen($this->getBody()));

        return parent::send();
    }

}