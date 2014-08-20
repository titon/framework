<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Io\Writer\AbstractWriter;
use Titon\Utility\Hash;

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
    public function append($data) {
        if ($contents = $this->read()) {
            $data = Hash::merge(json_decode($contents, true), $data);
        }

        return $this->write($data);
    }

    /**
     * {@inheritdoc}
     */
    public function write($data) {
        return parent::write(json_encode($data, JSON_PRETTY_PRINT));
    }

}