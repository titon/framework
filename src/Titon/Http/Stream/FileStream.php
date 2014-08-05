<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Stream;

/**
 * The FileStream will load a local file defined by path as the stream resource.
 *
 * @package Titon\Http\Stream
 */
class FileStream extends AbstractStream {

    /**
     * Initialize the stream.
     *
     * @param string $path
     */
    public function __construct(string $path) {
        $this->setStream(fopen($path, 'r+b'));
    }

}