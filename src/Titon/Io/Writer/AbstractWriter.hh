<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Io\File;
use Titon\Io\Writer;

/**
 * Abstract class for readers that also provides file access.
 *
 * @package Titon\Io\Writer
 */
abstract class AbstractWriter extends File implements Writer {

    /**
     * {@inheritdoc}
     */
    public function getPath(): string {
        return $this->path();
    }

}