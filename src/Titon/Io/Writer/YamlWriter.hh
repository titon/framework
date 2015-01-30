<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Common\Exception\MissingExtensionException;
use Titon\Io\ResourceMap;
use Titon\Utility\Col;

/**
 * A file writer that generates YAML files.
 *
 * @package Titon\Io\Writer
 */
class YamlWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     */
    public function getResourceExt(): string {
        return 'yml';
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Common\Exception\MissingExtensionException
     */
    public function writeResource(ResourceMap $data): bool {
        if (!extension_loaded('yaml')) {
            throw new MissingExtensionException('YAML extension must be installed to use the YamlWriter');
        }

        return $this->write(yaml_emit(Col::toArray($data), YAML_UTF8_ENCODING));
    }

}
