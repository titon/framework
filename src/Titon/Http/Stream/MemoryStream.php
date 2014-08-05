<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Stream;

/**
 * The MemoryStream will use the built-in PHP memory resource as the stream.
 *
 * @package Titon\Http\Stream
 */
class MemoryStream extends AbstractStream {

    /**
     * Initialize the stream.
     *
     * @param string $content
     */
    public function __construct(string $content) {
        $this->setStream(fopen('php://memory', 'r+b'));
        $this->write($content);
    }

}