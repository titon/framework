<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Debug\Debugger;
use Titon\Io\ResourceMap;
use Titon\Utility\Col;

/**
 * A file writer that generates PHP files.
 *
 * @package Titon\Io\Writer
 */
class PhpWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     */
    public function getResourceExt(): string {
        return 'php';
    }

    /**
     * {@inheritdoc}
     */
    public function writeResource(ResourceMap $data): bool {
        $data = Col::toArray($data);

        if (class_exists('Titon\Debug\Debugger')) {
            $export = Debugger::export($data, true);
        } else {
            $export = var_export($data, true);
        }

        return $this->write(sprintf('<?php return %s;', $export));
    }

}