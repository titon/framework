<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Io\ResourceMap;
use Titon\Io\Exception\ReadErrorException;
use Titon\Utility\Col;

/**
 * A file reader that parses PHP files.
 * The PHP file must contain a return statement that returns an array.
 *
 * @package Titon\Io\Reader
 */
class PhpReader extends AbstractReader {

    /**
     * {@inheritdoc}
     */
    public function getResourceExt(): string {
        return 'php';
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function readResource(): ResourceMap {
        if ($this->exists()) {
            return Col::toMap(include_file($this->getPath()))->toMap();
        }

        throw new ReadErrorException(sprintf('PhpReader failed to parse %s', $this->getPath()));
    }

}