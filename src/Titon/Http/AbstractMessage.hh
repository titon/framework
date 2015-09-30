<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Psr\Http\Message\MessageInterface;
use Psr\Http\Message\StreamInterface;
use Titon\Http\Bag\HeaderBag;
use Titon\Http\Stream\MemoryStream;

/**
 * Provides shared message functionality for request and response classes.
 *
 * @package Titon\Http
 */
abstract class AbstractMessage implements MessageInterface {

    /**
     * The request or response body.
     *
     * @var \Psr\Http\Message\StreamInterface
     */
    protected StreamInterface $body;

    /**
     * Headers to include in the request or response.
     *
     * @var \Titon\Http\Bag\HeaderBag
     */
    protected HeaderBag $headers;

    /**
     * The HTTP protocol version.
     *
     * @var string
     */
    protected string $protocolVersion = '1.1';

    /**
     * Store and create a new stream instance.
     *
     * @param \Psr\Http\Message\StreamInterface $body
     * @param \Titon\Http\HeaderMap $headers
     */
    public function __construct(?StreamInterface $body = null, HeaderMap $headers = Map {}) {
        if ($body === null) {
            $body = new MemoryStream();
        }

        $this->body = $body;
        $this->headers = new HeaderBag($headers);
    }

    /**
     * Clone all bags and streams.
     */
    public function __clone() {
        $this->body = clone $this->body;
        $this->headers = clone $this->headers;
    }

    /**
     * {@inheritdoc}
     */
    public function getBody(): StreamInterface {
        return $this->body;
    }

    /**
     * {@inheritdoc}
     */
    public function getHeader($name): array<string> {
        return $this->headers->get((string) $name) ?: [];
    }

    /**
     * {@inheritdoc}
     */
    public function getHeaderLine($name): string {
        return implode(', ', $this->getHeader($name));
    }

    /**
     * {@inheritdoc}
     */
    public function getHeaders(): array<string, array<string>> {
        return $this->headers->all()->toArray();
    }

    /**
     * {@inheritdoc}
     */
    public function getProtocolVersion(): string {
        return $this->protocolVersion;
    }

    /**
     * {@inheritdoc}
     */
    public function hasHeader($name): bool {
        return $this->headers->has((string) $name);
    }

    /**
     * {@inheritdoc}
     */
    public function withAddedHeader($name, $value): this {
        if (!$this->hasHeader($name)) {
            return $this->withHeader($name, $value);
        }

        $self = clone $this;
        $name = (string) $name;

        if (is_array($value)) {
            foreach ($value as $val) {
                $self->headers->append($name, (string) $val);
            }
        } else {
            $self->headers->append($name, (string) $value);
        }

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withBody(StreamInterface $body): this {
        if ($this->body === $body) {
            return $this;
        }

        $self = clone $this;
        $self->body = $body;

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withHeader($name, $value): this {
        if (!is_array($value)) {
            $value = [$value];
        }

        $self = clone $this;
        $self->headers->set((string) $name, array_map(fun('strval'), $value));

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withProtocolVersion($version): this {
        $self = clone $this;
        $self->protocolVersion = (string) $version;

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withoutHeader($name): this {
        if (!$this->hasHeader($name)) {
            return $this;
        }

        $self = clone $this;
        $self->headers->remove((string) $name);

        return $self;
    }
}
