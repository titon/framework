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
 * A reader that loads its configuration from an YAML file.
 * Requires the YAML extension to be installed.
 *
 * @package Titon\Io\Reader
 */
class YamlReader extends AbstractReader {

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Col
     *
     * @throws \Titon\Io\Exception\MissingExtensionException
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function read<Tk, Tv>(): Map<Tk, Tv> {
        if (!extension_loaded('yaml')) {
            throw new MissingExtensionException('YAML extension must be installed to use the YamlReader');
        }

        if ($this->exists()) {
            return Col::toMap(yaml_parse_file($this->path()));
        }

        throw new ReadErrorException(sprintf('YamlReader failed to parse %s', $this->path()));
    }

}