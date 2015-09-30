<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Stream;

use Titon\Common\Exception\MissingFileException;
use Titon\Http\Exception\InvalidFileException;

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
     * @param string $mode
     * @throws \Titon\Common\Exception\MissingFileException
     * @throws \Titon\Http\Exception\InvalidFileException;
     */
    public function __construct(string $path, string $mode = 'r+b') {
        if (!file_exists($path)) {
            throw new MissingFileException(sprintf('File %s does not exist', basename($path)));

        } else if (!is_readable($path)) {
            throw new InvalidFileException(sprintf('File %s is not readable', basename($path)));
        }

        $this->setStream(fopen($path, $mode));
    }

}
