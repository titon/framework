<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View;

/**
 * Enumerable for the types of templates.
 *
 * @package Titon\View
 */
enum Template: int {
    OPEN = 1; // public
    CLOSED = 2; // private
    LAYOUT = 3;
    WRAPPER = 4;
    PARTIAL = 5;
}