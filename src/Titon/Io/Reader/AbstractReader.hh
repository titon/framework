<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Common\Cacheable;
use Titon\Common\Exception\MissingFileException;
use Titon\Io\File;
use Titon\Io\Reader;

/**
 * Abstract class for readers that also provides file access.
 *
 * @package Titon\Io\Reader
 */
abstract class AbstractReader extends File implements Reader {
    use Cacheable;

    /**
     * Set the path during construction.
     *
     * @param string $path
     * @throws \Titon\Common\Exception\MissingFileException
     */
    public function __construct(string $path = '') {
        if ($path) {
            if (!file_exists($path)) {
                throw new MissingFileException(sprintf('File %s does not exist', basename($path)));
            }

            parent::__construct($path);
        }
    }

    /**
     * {@inheritdoc}
     */
    public function reset(string $path = ''): this {
        $this->flushCache();

        return parent::reset($path);
    }

}