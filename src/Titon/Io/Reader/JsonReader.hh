<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

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
     *
     * @uses Titon\Utility\Col
     *
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function read<Tk, Tv>(): Map<Tk, Tv> {
        if ($this->exists()) {
            return Col::toMap(@json_decode(parent::read(), true));
        }

        throw new ReadErrorException(sprintf('JsonReader failed to parse %s', $this->path()));
    }

}