<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\System;

/**
 * Linux-specific information.
 *
 * @package Titon\Console\System
 */
class Linux extends AbstractSystem {

    /**
     * {@inheritdoc}
     */
    public function getHeight(): int {
        return (int)exec('tput lines');
    }

    /**
     * {@inheritdoc}
     */
    public function getWidth(): int {
        return (int)exec('tput cols');
    }

    /**
     * {@inheritdoc}
     */
    public function supportsAnsi(): bool {
        return (function_exists('posix_isatty') && @posix_isatty(STDOUT));
    }
}