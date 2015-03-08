<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

/**
 * User input handles presenting a prompt to the user and
 *
 * @package Titon\Console
 */
interface UserInput {

    /**
     * Present the user with a prompt and return the inputted value.
     *
     * @param string $message   The message to present with the prompt.
     *
     * @return mixed
     */
    public function prompt(string $message): mixed;
}