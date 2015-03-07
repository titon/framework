<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\System;

use Titon\Console\System;

abstract class AbstractSystem implements System {

    protected bool $forceAnsi = false;

    public function forceAnsiSupport(bool $force): this {
        $this->forceAnsi = $force;

        return $this;
    }

    public function hasAnsiSupport(): bool {
        if ($this->forceAnsi === true) {
            return true;
        }

        return $this->supportsAnsi();
    }

    public function isPiped(): bool {
        $shellPipe = getenv('SHELL_PIPE');
        if ($shellPipe !== false) {
            return filter_var($shellPipe, FILTER_VALIDATE_BOOLEAN);
        } else {
            return function_exists('posix_isatty') && !@posix_isatty(STDOUT);
        }
    }
}