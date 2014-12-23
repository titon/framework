<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

/**
 * A matcher is used for matching a route against a defined URL.
 *
 * @package Titon\Route
 */
interface Matcher {

    /**
     * Attempt to match a route against a URL.
     *
     * @param string $url
     * @param \Titon\Route\RouteMap $routes
     * @return \Titon\Route\Route
     */
    public function match(string $url, RouteMap $routes): ?Route;

}