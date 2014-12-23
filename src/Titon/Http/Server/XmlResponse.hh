<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Psr\Http\Message\StreamableInterface;
use Titon\Http\Http;
use Titon\Http\Stream\MemoryStream;
use Titon\Type\XmlDocument;

/**
 * Output XML as the response by converting any type of resource to XML.
 *
 * @package Titon\Http\Server
 */
class XmlResponse extends Response {

    /**
     * Set the body, status code, and optional XML root node.
     * Convert the body to XML before passing along.
     *
     * @param mixed $body
     * @param int $status
     * @param string $root
     */
    public function __construct(mixed $body = null, int $status = Http::OK, string $root = 'root') {
        if (!$body instanceof StreamableInterface) {
            $body = new MemoryStream( XmlDocument::from($body, $root)->toString() );
        }

        parent::__construct($body, $status);
    }

    /**
     * Set the content type and length before sending.
     *
     * @return string
     */
    public function send(): string {
        $this->contentType('xml');

        if ($body = $this->getBody()) {
            $this->contentLength($body->getSize());
        }

        return parent::send();
    }

}