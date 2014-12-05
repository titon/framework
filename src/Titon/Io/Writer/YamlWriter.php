<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Common\DataMap;
use Titon\Io\Exception\MissingExtensionException;
use Titon\Io\Reader\YamlReader;
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
     * @uses Titon\Utility\Hash
     *
     * @throws \Titon\Io\Exception\MissingExtensionException
     */
    public function append(DataMap $data) {
        if (!extension_loaded('yaml')) {
            throw new MissingExtensionException('YAML extension must be installed to use the YamlWriter');
        }

        $reader = new YamlReader($this->path());

        if ($contents = $reader->read()) {
            $data = Col::merge($contents, $data);
        }

        return $this->write($data);
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\MissingExtensionException
     */
    public function write(DataMap $data) {
        if (!extension_loaded('yaml')) {
            throw new MissingExtensionException('YAML extension must be installed to use the YamlWriter');
        }

        $data = yaml_emit(Col::toArray($data), YAML_UTF8_ENCODING);

        return parent::write($data);
    }

}