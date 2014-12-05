<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Common\DataMap;
use Titon\Io\Reader\XmlReader;
use Titon\Type\XmlDocument;
use Titon\Utility\Col;

/**
 * A file writer that generates XML files.
 *
 * @package Titon\Io\Writer
 */
class XmlWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Col
     */
    public function append(DataMap $data) {
        $reader = new XmlReader($this->path());

        if ($contents = $reader->read()) {
            $data = Col::merge($contents, $data);
        }

        return $this->write($data);
    }

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Type\XmlDocument
     */
    public function write(DataMap $data) {
        return parent::write(XmlDocument::fromMap($data)->toString());
    }

}