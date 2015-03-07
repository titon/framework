<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\System;

class Linux extends AbstractSystem {

    public function getHeight(): int {
        return (int)exec('tput lines');
    }

    public function getWidth(): int {
        return (int)exec('tput cols');
    }

    public function supportsAnsi(): bool {
        return (function_exists('posix_isatty') && @posix_isatty(STDOUT));
    }
}