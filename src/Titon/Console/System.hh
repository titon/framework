<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

/**
 * The `System` class handles operating-specific methods.
 *
 * @package Titon\Console
 */
interface System {

    /**
     * Retrieve the height of the current terminal window.
     *
     * @return int
     */
    public function getHeight(): int;

    /**
     * Retrieve the width of the current terminal window.
     *
     * @return int
     */
    public function getWidth(): int;

    /**
     * Determine if the current command is piped to another command.
     *
     * @return bool
     */
    public function isPiped(): bool;

    /**
     * Determine if the current system supports ANSI output.
     *
     * @return bool
     */
    public function supportsAnsi(): bool;
}