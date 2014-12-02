<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Io\Exception\ReadErrorException;
use Titon\Io\Exception\MissingExtensionException;
use Titon\Utility\Col;

/**
 * A reader that loads its configuration from an YAML file.
 * Must have the PECL YAML module installed.
 *
 * @package Titon\Io\Reader
 */
class YamlReader extends AbstractReader {

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\MissingExtensionException
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function read(): Map<string, mixed> {
        if (!extension_loaded('yaml')) {
            throw new MissingExtensionException('YAML PECL extension must be installed to use the YamlReader');
        }

        return $this->cache([__METHOD__, $this->path()], function() {
            if ($this->exists()) {
                return Col::toMap(yaml_parse_file($this->path()));
            }

            throw new ReadErrorException(sprintf('YamlReader failed to parse %s', $this->name()));
        });
    }

}