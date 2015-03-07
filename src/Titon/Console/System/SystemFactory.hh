<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\System;

class SystemFactory {

    public static function factory() {
        if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
            return new Windows();
        }

        return new Linux();
    }
}