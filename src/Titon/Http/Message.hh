<?hh // partial
// Because of PSR HTTP Message
/**
 * @copyright   2010-2015, The Titon Project
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
    protected ?StreamInterface $body = null;

    /**
     * Instantiate a new header bag.
     */
    public function __construct() {
        $this->headers = new HeaderBag();
    }

    /**
     * {@inheritdoc}
     */
    public function getBody(): ?StreamInterface {
        return $this->body;
    }

    /**
     * {@inheritdoc}
     */
    public function getHeader($header): string {
        return implode(', ', $this->getHeaderAsArray($header));
    }

    /**
     * {@inheritdoc}
     */
    public function getHeaderAsArray($header): array<string> {
        return $this->headers->get($header) ?: [];
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
        return '1.1';
    }

    /**
     * {@inheritdoc}
     */
    public function hasHeader($header): bool {
        return $this->headers->has($header);
    }

}
