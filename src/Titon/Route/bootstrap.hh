<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

use Titon\Route\ParamMap;
use Titon\Route\QueryMap;
use Titon\Route\Router;

/**
 * @see Titon\Route\Router::build()
 */
function url(string $key, ParamMap $params = Map {}, QueryMap $query = Map {}): string {
    return Router::registry()->build($key, $params, $query);
}