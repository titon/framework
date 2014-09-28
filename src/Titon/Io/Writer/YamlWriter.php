<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

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
     * @uses Titon\Utility\Hash
     *
     * @throws \Titon\Io\Exception\MissingExtensionException
     */
    public function append($data) {
        if (!extension_loaded('yaml')) {
            throw new MissingExtensionException('YAML PECL extension must be installed to use the YamlWriter');
        }

        if ($contents = $this->read()) {
            $data = Col::merge(yaml_parse($contents), $data);
        }

        return $this->write($data);
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\MissingExtensionException
     */
    public function write($data) {
        if (!extension_loaded('yaml')) {
            throw new MissingExtensionException('YAML PECL extension must be installed to use the YamlWriter');
        }

        if (is_array($data)) {
            $data = yaml_emit($data, YAML_UTF8_ENCODING);
        }

        return parent::write($data);
    }

}