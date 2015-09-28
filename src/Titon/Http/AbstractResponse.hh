<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Psr\Http\Message\ResponseInterface;

/**
 * Provides shared functionality for response classes.
 *
 * @package Titon\Http
 */
abstract class AbstractResponse extends AbstractMessage implements ResponseInterface {

    /**
     * HTTP status code to respond.
     *
     * @var int
     */
    protected int $status = StatusCode::OK;

    /**
     * {@inheritdoc}
     */
    public function getReasonPhrase(): string {
        return $this->getHeaderLine('Reason-Phrase') ?: StatusCode::getReasonPhrase($this->getStatusCode());
    }

    /**
     * {@inheritdoc}
     */
    public function getStatusCode(): int {
        return $this->status;
    }

    /**
     * {@inheritdoc}
     */
    public function withStatus($code, $reasonPhrase = ''): this {
        $self = clone $this;
        $self->status = (int) $code;

        if ($reasonPhrase) {
            $self->headers->set('Reason-Phrase', [(string) $reasonPhrase]);
        }

        return $self;
    }
}
