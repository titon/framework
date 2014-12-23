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
 * A file reader that parses INI files.
 *
 * @package Titon\Io\Reader
 */
class IniReader extends AbstractReader {

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Col
     *
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function read<Tk, Tv>(): Map<Tk, Tv> {
        if ($this->exists()) {
            return Col::toMap(parse_ini_file($this->path(), true, INI_SCANNER_NORMAL));
        }

        throw new ReadErrorException(sprintf('IniReader failed to parse %s', $this->path()));
    }

}