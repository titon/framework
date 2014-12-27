<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Io\ResourceMap;
use Titon\Io\Exception\ReadErrorException;
use Titon\Type\XmlDocument;

/**
 * A reader that loads its configuration from an XML file.
 * Must have the SimpleXML module installed.
 *
 * @package Titon\Io\Reader
 */
class XmlReader extends AbstractReader {

    /**
     * {@inheritdoc}
     */
    public function getResourceExt(): string {
        return 'xml';
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function readResource(): ResourceMap {
        if ($this->exists()) {
            return XmlDocument::fromFile($this->getPath())->toMap(false)->toMap();
        }

        throw new ReadErrorException(sprintf('XmlReader failed to parse %s', $this->getPath()));
    }

}