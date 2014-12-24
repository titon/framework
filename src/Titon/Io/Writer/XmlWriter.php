<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Io\ResourceMap;
use Titon\Type\XmlDocument;

/**
 * A file writer that generates XML files.
 *
 * @package Titon\Io\Writer
 */
class XmlWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     */
    public function getResourceExt(): string {
        return 'xml';
    }

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Type\XmlDocument
     */
    public function writeResource(ResourceMap $data): bool {
        return $this->write(XmlDocument::fromMap($data)->toString());
    }

}