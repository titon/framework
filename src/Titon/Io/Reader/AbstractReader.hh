<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Io\Exception\MissingFileException;
use Titon\Io\File;
use Titon\Io\Reader;

/**
 * Abstract class for readers that also provides file access.
 *
 * @package Titon\Io\Reader
 */
abstract class AbstractReader extends File implements Reader {

    /**
     * Set the path during construction.
     * The path is optional as we don't want to create the file under some situations.
     *
     * @param string $path
     * @throws \Titon\Io\Exception\MissingFileException
     */
    public function __construct(string $path = '') {
        if ($path && !file_exists($path)) {
            throw new MissingFileException(sprintf('File %s does not exist', $path));
        }

        parent::__construct($path);
    }

    /**
     * {@inheritdoc}
     */
    public function getPath(): string {
        return $this->path();
    }

}
