<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Psr\Http\Message\RequestInterface;
use Psr\Http\Message\StreamInterface;
use Psr\Http\Message\UriInterface;
use Titon\Http\Exception\InvalidMethodException;

/**
 * Provides shared functionality for request classes.
 *
 * @package Titon\Http
 */
abstract class AbstractRequest extends AbstractMessage implements RequestInterface {

    /**
     * The current HTTP request method.
     *
     * @var string
     */
    protected string $method = '';

    /**
     * The current request target.
     *
     * @var string
     */
    protected string $requestTarget = '';

    /**
     * The current request URI.
     *
     * @var \Psr\Http\Message\UriInterface
     */
    protected UriInterface $uri;

    /**
     * Store the URI, HTTP method, and stream.
     *
     * @param \Psr\Http\Message\UriInterface $uri
     * @param string $method
     * @param \Psr\Http\Message\StreamInterface $body
     */
    public function __construct(UriInterface $uri, string $method = 'GET', StreamInterface $body = null) {
        parent::__construct($body);

        $this->uri = $uri;
        $this->method = $this->validateMethod($method);

        if ($host = $uri->getHost()) {
            $this->headers->set('Host', [$host]);
        }
    }

    /**
     * {@inheritdoc}
     */
    public function getMethod(): string {
        return $this->method;
    }

    /**
     * {@inheritdoc}
     */
    public function getRequestTarget(): string {
        if ($target = $this->requestTarget) {
            return $target;
        }

        $target = $this->getUri()->getPath() ?: '/';

        if ($query = $this->getUri()->getQuery()) {
            $target .= '?' . $query;
        }

        return $this->requestTarget = $target;
    }

    /**
     * {@inheritdoc}
     */
    public function getUri(): UriInterface {
        return $this->uri;
    }

    /**
     * {@inheritdoc}
     */
    public function withMethod($method): this {
        $self = clone $this;
        $self->method = $this->validateMethod((string) $method);

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withRequestTarget($requestTarget): this {
        $self = clone $this;
        $self->requestTarget = (string) $requestTarget;

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withUri(UriInterface $uri, $preserveHost = false): this {
        $self = clone $this;
        $self->uri = $uri;

        $host = $uri->getHost();

        if ($preserveHost) {
            if (!$this->getHeader('Host') && $host) {
                $self->headers->set('Host', [$host]);
            }
        } else {
            $self->headers->set('Host', [$host]);
        }

        return $self;
    }

    /**
     * Validate the HTTP method passed in is valid.
     *
     * @param string $method
     * @return string
     * @throws \Titon\Http\Exception\InvalidMethodException
     */
    protected function validateMethod(string $method): string {
        if (!Http::getMethodTypes()->contains($method)) {
            throw new InvalidMethodException(sprintf('Unknown HTTP method [%s]', $method));
        }

        return $method;
    }

}
