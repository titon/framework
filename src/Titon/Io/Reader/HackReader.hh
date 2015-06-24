<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Io\ResourceMap;
use Titon\Io\Exception\ReadErrorException;

/**
 * A file reader that parses Hack files.
 * The Hack file must contain a return statement that returns a map.
 *
 * @package Titon\Io\Reader
 */
class HackReader extends AbstractReader {

    /**
     * {@inheritdoc}
     */
    public function getResourceExt(): string {
        return 'hh';
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function readResource(): ResourceMap {
        if ($this->exists()) {
            return include_file($this->getPath())->toMap();
        }

        throw new ReadErrorException(sprintf('HackReader failed to parse %s', $this->getPath()));
    }

}
