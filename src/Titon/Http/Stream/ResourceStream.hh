<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Stream;

/**
 * The ResourceStream will use an existing opened resource as the stream.
 *
 * @package Titon\Http\Stream
 */
class ResourceStream extends AbstractStream {

    /**
     * Initialize the stream.
     *
     * @param resource $resource
     */
    public function __construct(resource $resource) {
        $this->setStream($resource);
    }

}