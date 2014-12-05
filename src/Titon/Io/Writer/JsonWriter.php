<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Common\DataMap;
use Titon\Io\Reader\JsonReader;
use Titon\Utility\Col;

/**
 * A file writer that generates JSON files.
 *
 * @package Titon\Io\Writer
 */
class JsonWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Hash
     */
    public function append(DataMap $data) {
        $reader = new JsonReader($this->path());

        if ($contents = $reader->read()) {
            $data = Col::merge($contents, $data);
        }

        return parent::write($data);
    }

    /**
     * {@inheritdoc}
     */
    public function write(DataMap $data) {
        return parent::write(json_encode($data, JSON_PRETTY_PRINT));
    }

}