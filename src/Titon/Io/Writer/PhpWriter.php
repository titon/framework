<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Io\ResourceMap;

/**
 * A file writer that generates PHP files.
 *
 * @package Titon\Io\Writer
 */
class PhpWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     */
    public function write(ResourceMap $data) {
        return parent::write(sprintf('<?php return %s;', var_export($data, true)));
    }

}