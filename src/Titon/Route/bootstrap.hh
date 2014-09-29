<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

use Titon\Route\Router;

/**
 * @see Titon\Route\Router::build()
 */
function url(mixed $url): string {
    return Router::registry()->build($url);
}