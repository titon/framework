<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Titon\Http\Http;
use Titon\Utility\Converter;

/**
 * Output XML as the response by converting any type of resource to XML.
 *
 * @package Titon\Http\Server
 */
class XmlResponse extends Response {

    /**
     * Configuration.
     *
     * @type array {
     *      @type string $root  Name of the root node in the XML document
     * }
     */
    protected $_config = [
        'root' => 'root'
    ];

    /**
     * Set the body, status code, and optional XML root node.
     *
     * @param string $body
     * @param int $status
     * @param string $root
     */
    public function __construct($body = '', $status = Http::OK, $root = 'root') {
        parent::__construct($body, $status, ['root' => $root]);
    }

    /**
     * Convert the resource to XML.
     *
     * @param string $body
     * @return $this
     */
    public function setBody($body = null) {
        $this->_body = Converter::toXml($body, $this->getConfig('root'));

        return $this;
    }

    /**
     * Set the content type and length before sending.
     *
     * @return string
     */
    public function send() {
        $this
            ->contentType('xml')
            ->contentLength(mb_strlen($this->getBody()));

        return parent::send();
    }

}