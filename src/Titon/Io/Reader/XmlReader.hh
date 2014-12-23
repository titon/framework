<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Io\Exception\ReadErrorException;
use Titon\Utility\Converter;

/**
 * A reader that loads its configuration from an XML file.
 * Must have the SimpleXML module installed.
 *
 * @package Titon\Io\Reader
 */
class XmlReader extends AbstractReader {

    /**
     * Allow for format overrides.
     *
     * @var int
     */
    public static int $format = Converter::XML_MERGE;

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Col
     *
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function read(): Map<string, mixed> {
        return $this->cache([__METHOD__, $this->path()], function() {
            if ($this->exists()) {
                return Converter::toMap(Converter::xmlToArray(simplexml_load_string(parent::read()), static::$format));
            }

            throw new ReadErrorException(sprintf('XmlReader failed to parse %s', $this->name()));
        });
    }

}