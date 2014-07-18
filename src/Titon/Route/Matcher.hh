<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

/**
 * A matcher is used for finding a route that matches the URL passed in.
 *
 * @package Titon\Route
 */
interface Matcher {

    /**
     * Attempt to match a route against the URL.
     *
     * @param string $url
     * @param Map<string, Route> $routes
     * @return \Titon\Route\Route
     */
    public function match(string $url, Map<string, Route> $routes): ?Route;

}