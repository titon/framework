<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Common\DataMap;
use Titon\Io\Reader\PhpReader;
use Titon\Utility\Col;

/**
 * A file writer that generates PHP files.
 *
 * @package Titon\Io\Writer
 */
class PhpWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Col
     */
    public function append(DataMap $data) {
        $reader = new PhpReader($this->path());

        if ($contents = $reader->read()) {
            $data = Col::merge($contents, $data);
        }

        return parent::write($data);
    }

    /**
     * {@inheritdoc}
     */
    public function write(DataMap $data) {
        return parent::write(sprintf('<?php return %s;', var_export($data, true)));
    }

}