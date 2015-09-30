<?hh // partial
// Because of PSR HTTP Message
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Titon\Http\HeaderMap;
use Titon\Http\StatusCode;
use Titon\Type\Xml;

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
     * @param \Titon\Http\HeaderMap $headers
     */
    public function __construct(mixed $body = null, int $status = StatusCode::OK, string $root = 'root', HeaderMap $headers = Map {}) {
        parent::__construct(null, $status, $headers);

        // Update state
        $this->body->write(Xml::from($body, $root)->toString());
        $this->headers->set('Content-Type', [$this->validateContentType('xml')]);
    }

}
