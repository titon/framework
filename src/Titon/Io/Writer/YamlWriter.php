<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Io\ResourceMap;
use Titon\Io\Exception\MissingExtensionException;
use Titon\Utility\Col;

/**
 * A file writer that generates YAML files.
 *
 * @package Titon\Io\Writer
 */
class YamlWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\MissingExtensionException
     */
    public function write(ResourceMap $data) {
        if (!extension_loaded('yaml')) {
            throw new MissingExtensionException('YAML extension must be installed to use the YamlWriter');
        }

        $data = yaml_emit(Col::toArray($data), YAML_UTF8_ENCODING);

        return parent::write($data);
    }

}