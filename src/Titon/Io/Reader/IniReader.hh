<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Io\Exception\ReadErrorException;
use Titon\Utility\Converter;

/**
 * A file reader that parses INI files.
 *
 * @package Titon\Io\Reader
 */
class IniReader extends AbstractReader {

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function read(): Map<string, mixed> {
        return $this->cache([__METHOD__, $this->path()], function() {
            if ($this->exists()) {
                return Converter::toMap(parse_ini_file($this->path(), true, INI_SCANNER_NORMAL));
            }

            throw new ReadErrorException(sprintf('IniReader failed to parse %s', $this->name()));
        });
    }

}