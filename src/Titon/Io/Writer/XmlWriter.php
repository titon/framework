<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Utility\Col;
use Titon\Utility\Converter;

/**
 * A file writer that generates XML files.
 *
 * @package Titon\Io\Writer
 */
class XmlWriter extends AbstractWriter {

    /**
     * Allow for format overrides.
     *
     * @var int
     */
    public static $format = Converter::XML_MERGE;

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Hash
     * @uses Titon\Utility\Col
     */
    public function append($data) {
        if ($contents = $this->read()) {
            $data = Col::merge(Converter::xmlToArray(simplexml_load_string($contents), self::$format), $data);
        }

        return $this->write($data);
    }

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Col
     */
    public function write($data) {
        return parent::write(Converter::toXml($data));
    }

}