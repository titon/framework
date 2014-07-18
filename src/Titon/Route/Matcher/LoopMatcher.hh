<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route\Matcher;

use Titon\Route\Route;
use Titon\Route\Matcher;

/**
 * Loops through each route until a match is found.
 *
 * @package Titon\Route\Matcher
 */
class LoopMatcher implements Matcher {

    /**
     * {@inheritdoc}
     */
    public function match(string $url, Map<string, Route> $routes): ?Route {
        foreach ($routes as $route) {
            if ($route->isMatch($url)) {
                return $route;
            }
        }

        return null;
    }

}