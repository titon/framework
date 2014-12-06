<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

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
     *
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function read<Tk, Tv>(): Map<Tk, Tv> {
        if ($this->exists()) {
            return XmlDocument::fromFile($this->path())->toMap(false);
        }

        throw new ReadErrorException(sprintf('XmlReader failed to parse %s', $this->path()));
    }

}