<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Io\Writer\AbstractWriter;
use Titon\Utility\Hash;

/**
 * A file writer that generates PHP files.
 *
 * @package Titon\Io\Writer
 */
class PhpWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Hash
     */
    public function append($data) {
        if ($this->exists()) {
            if ($contents = include $this->path()) {
                $data = Hash::merge($contents, $data);
            }
        }

        return $this->write($data);
    }

    /**
     * {@inheritdoc}
     */
    public function write($data) {
        return parent::write(sprintf('<?php return %s;', var_export($data, true)));
    }

}