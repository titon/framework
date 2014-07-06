<?hh //strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Titon\Common\Base;
use Titon\Http\Bag\HeaderBag;

/**
 * Provides shared functionality for request and response classes.
 *
 * @package Titon\Http
 */
abstract class AbstractMessage extends Base implements Message {

    /**
     * Headers to include in the request or response.
     *
     * @type \Titon\Http\Bag\HeaderBag
     */
    public $headers;

    /**
     * The request or response body.
     *
     * @type string
     */
    protected $_body = null;

    /**
     * Instantiate a new header augment.
     *
     * @param array $config
     */
    public function __construct(array $config = []) {
        parent::__construct($config);

        $this->headers = new HeaderBag();
    }

    /**
     * {@inheritdoc}
     */
    public function addHeader($key, $value) {
        if (is_array($value)) {
            $value = implode(', ', $value);
        }

        $header = $this->headers->get($key, []);
        $header = array_merge($header, [$value]);

        $this->headers->set($key, $header);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addHeaders(array $headers) {
        foreach ($headers as $key => $value) {
            $this->addHeader($key, $value);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function hasHeader($key) {
        return $this->headers->has($key);
    }

    /**
     * {@inheritdoc}
     */
    public function getBody() {
        return $this->_body;
    }

    /**
     * {@inheritdoc}
     */
    public function getHeader($key, $asArray = false) {
        $value = $this->headers->get($key);
        $isArray = is_array($value);

        if ($asArray) {
            return $isArray ? $value : [$value];
        }

        return $isArray ? implode(', ', $value) : $value;
    }

    /**
     * {@inheritdoc}
     */
    public function getHeaders() {
        return $this->headers->all();
    }

    /**
     * {@inheritdoc}
     */
    public function setBody($body = null) {
        $this->_body = (string) $body;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setHeader($key, $value) {
        if (is_array($value)) {
            $value = implode(', ', $value);
        }

        $this->headers->set($key, [$value]);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setHeaders(array $headers) {
        foreach ($headers as $key => $value) {
            $this->setHeader($key, $value);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function removeHeader($key) {
        $this->headers->remove($key);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function removeHeaders(array $keys) {
        foreach ($keys as $key) {
            $this->removeHeader($key);
        }

        return $this;
    }

}