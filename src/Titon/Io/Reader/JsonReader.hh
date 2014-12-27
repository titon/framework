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
 * A file reader that parses JSON files; must have the JSON module installed.
 *
 * @package Titon\Io\Reader
 */
class JsonReader extends AbstractReader {

    /**
     * {@inheritdoc}
     */
    public function getResourceExt(): string {
        return 'json';
    }

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Col
     *
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function readResource(): ResourceMap {
        if ($this->exists()) {
            return Col::toMap(@json_decode($this->read(), true))->toMap();
        }

        throw new ReadErrorException(sprintf('JsonReader failed to parse %s', $this->getPath()));
    }

}