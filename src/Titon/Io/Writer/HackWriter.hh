<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Debug\Debugger;
use Titon\Io\ResourceMap;

/**
 * A file writer that generates Hack files.
 *
 * @package Titon\Io\Writer
 */
class HackWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     */
    public function getResourceExt(): string {
        return 'hh';
    }

    /**
     * {@inheritdoc}
     */
    public function writeResource(ResourceMap $data): bool {
        if (class_exists('Titon\Debug\Debugger')) {
            $export = Debugger::export($data);
        } else {
            $export = var_export($data, true);
        }

        return $this->write(sprintf('<?hh return %s;', $export));
    }

}
