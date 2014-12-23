<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route\Matcher;

use Titon\Route\Matcher;
use Titon\Route\Route;
use Titon\Route\RouteMap;

/**
 * Loops through each route until a match is found.
 *
 * @package Titon\Route\Matcher
 */
class LoopMatcher implements Matcher {

    /**
     * {@inheritdoc}
     */
    public function match(string $url, RouteMap $routes): ?Route {
        foreach ($routes as $route) {
            if ($route->isMatch($url)) {
                return $route;
            }
        }

        return null;
    }

}