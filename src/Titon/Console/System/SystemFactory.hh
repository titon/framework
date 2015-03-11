<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\System;

use Titon\Console\System;

/**
 * Factory class for returning the proper `System` based on the operating system
 * the application is running on.
 *
 * @package Titon\Console\System
 */
class SystemFactory {

    /**
     * Return the proper `System` class based on the system's operating system.
     *
     * @return \Titon\Console\System
     */
    public static function factory(): System {
        if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
            return new Windows();
        }

        return new Linux();
    }
}