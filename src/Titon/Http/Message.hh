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
class Message implements MessageInterface {

    /**
     * Headers to include in the request or response.
     *
     * @var \Titon\Http\Bag\HeaderBag
     */
    public HeaderBag $headers;

    /**
     * The request or response body.
     *
     * @var \Psr\Http\Message\StreamInterface
     */
    protected StreamInterface $body;

    /**
     * The HTTP protocol version.
     *
     * @var string
     */
    protected string $protocolVersion = '1.1';

    /**
     * Store and create a new stream instance.
     *
     * @param Psr\Http\Message\StreamInterface $stream
     */
    public function __construct(?StreamInterface $stream = null) {
        if (!$stream) {
            $stream = new MemoryStream();
        }

        $this->body = $stream;
    }

    /**
     * Clone all bags.
     */
    public function __clone() {
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
    public function getHeader(string $name): array<string> {
        return $this->headers->get($name) ?: [];
    }

    /**
     * {@inheritdoc}
     */
    public function getHeaderLine(string $name): string {
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
    public function hasHeader(string $name): bool {
        return $this->headers->has($name);
    }

    /**
     * {@inheritdoc}
     */
    public function withAddedHeader(string $name, mixed $value): this {
        $self = clone $this;

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
        $self = clone $this;
        $self->body = $body;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function withHeader(string $name, mixed $value): this {
        if (!is_array($value)) {
            $value = [ (string) $value ];
        }

        $self = clone $this;
        $self->headers->set($name, $value);

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withProtocolVersion(string $version): this {
        $self = clone $this;
        $self->protocolVersion = $version;

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withoutHeader(string $name): this {
        $self = clone $this;
        $self->headers->remove($name);

        return $self;
    }
}
