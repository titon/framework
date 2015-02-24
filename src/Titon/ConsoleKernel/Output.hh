<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\ConsoleKernel;

use Titon\Kernel\Output as BaseOutput;

interface Output extends BaseOutput {

    const OUTPUT_NORMAL = 0;
    const OUTPUT_RAW = 1;
    const OUTPUT_PLAIN = 2;

    public function getVerbosity(): int;

    public function setVerbosity(int $level): this;

    public function write(string $message, int $nl = 0, int $type = self::OUTPUT_NORMAL): void;

}
