<?hh // partial
// Because of PSR HTTP Message
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Psr\Http\Message\OutgoingResponseInterface;

/**
 * Permits a class to interact with an outgoing response object.
 *
 * @package Titon\Http
 */
trait OutgoingResponseAware {

    /**
     * Response object.
     *
     * @var \Psr\Http\Message\OutgoingResponseInterface
     */
    protected ?OutgoingResponseInterface $response;

    /**
     * Return the response object.
     *
     * @return \Psr\Http\Message\OutgoingResponseInterface
     */
    public function getResponse(): ?OutgoingResponseInterface {
        return $this->response;
    }

    /**
     * Set the response object.
     *
     * @param \Psr\Http\Message\OutgoingResponseInterface $response
     * @return $this
     */
    public function setResponse(OutgoingResponseInterface $response): this {
        $this->response = $response;

        return $this;
    }

}
