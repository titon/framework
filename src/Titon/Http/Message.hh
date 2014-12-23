<?hh // partial
// Because of PSR HTTP Message
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Psr\Http\Message\MessageInterface;
use Psr\Http\Message\StreamableInterface;
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
    public HeaderBag<string, array<string>> $headers;

    /**
     * The request or response body.
     *
     * @var \Psr\Http\Message\StreamableInterface
     */
    protected ?StreamableInterface $_body = null;

    /**
     * Instantiate a new header bag.
     */
    public function __construct() {
        $this->headers = new HeaderBag();
    }

    /**
     * {@inheritdoc}
     */
    public function getBody(): ?StreamableInterface {
        return $this->_body;
    }

    /**
     * {@inheritdoc}
     */
    public function getHeader($key): string {
        return implode(', ', $this->getHeaderAsArray($key));
    }

    /**
     * {@inheritdoc}
     */
    public function getHeaderAsArray($key): array<string> {
        if ($value = $this->headers->get($key)) {
            invariant(is_array($value), 'Headers must be an array.');

            return $value;
        }

        return [];
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
    public function hasHeader($key): bool {
        return $this->headers->has($key);
    }

}