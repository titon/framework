<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\ConsoleKernel;

use Titon\Kernel\Input as BaseInput;

interface Input extends BaseInput {

    public function getArguments(): Vector<string>;

    public function getFlag(string $key): bool;

    public function getFlags(): Map<string, bool>;

    public function getOption(string $key): string;

    public function getOptions(): Map<string, string>;

}
