<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Psr\Http\Message\MessageInterface;
use Psr\Http\Message\StreamInterface;
use Titon\Http\Bag\HeaderBag;

/**
 * Provides shared functionality for request and response classes.
 *
 * @package Titon\Http
 */
abstract class AbstractMessage implements MessageInterface {

    /**
     * Headers to include in the request or response.
     *
     * @type \Titon\Http\Bag\HeaderBag
     */
    public HeaderBag $headers;

    /**
     * The request or response body.
     *
     * @type StreamInterface
     */
    protected ?StreamInterface $_body = null;

    /**
     * Instantiate a new header augment.
     *
     * @param Map<string, mixed> $config
     */
    public function __construct(Map<string, mixed> $config = Map {}) {
        parent::__construct($config);

        $this->headers = new HeaderBag();
    }

    /**
     * {@inheritdoc}
     */
    public function addHeader($key, $value): this {
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
    public function addHeaders(array $headers): this {
        foreach ($headers as $key => $value) {
            $this->addHeader($key, $value);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function hasHeader($key): bool {
        return $this->headers->has($key);
    }

    /**
     * {@inheritdoc}
     */
    public function getBody(): ?StreamInterface {
        return $this->_body;
    }

    /**
     * {@inheritdoc}
     */
    public function getHeader($key): string {
        $value = $this->headers->get($key) ?: '';

        return ($value instanceof Traversable) ? implode(', ', $value) : $value;
    }

    /**
     * {@inheritdoc}
     */
    public function getHeaderAsArray($key): array<string> {
        $value = $this->headers->get($key);

        return is_array($value) ? $value : [$value];
    }

    /**
     * {@inheritdoc}
     */
    public function getHeaders(): array<string, string> {
        return $this->headers->all()->toArray();
    }

    /**
     * {@inheritdoc}
     */
    public function setBody(?StreamInterface $body = null): this {
        $this->_body = $body;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setHeader($key, $value): this {
        if (is_array($value)) {
            $value = implode(', ', $value);
        }

        $this->headers->set($key, [$value]);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setHeaders(array $headers): this {
        foreach ($headers as $key => $value) {
            $this->setHeader($key, $value);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function removeHeader($key): this {
        $this->headers->remove($key);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function removeHeaders(array $keys): this {
        foreach ($keys as $key) {
            $this->removeHeader($key);
        }

        return $this;
    }

}