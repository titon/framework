<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Io\ResourceMap;

/**
 * A file writer that generates JSON files.
 *
 * @package Titon\Io\Writer
 */
class JsonWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     */
    public function getResourceExt(): string {
        return 'json';
    }

    /**
     * {@inheritdoc}
     */
    public function writeResource(ResourceMap $data): bool {
        return $this->write(json_encode($data, JSON_PRETTY_PRINT));
    }

}