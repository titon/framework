<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

use Titon\Route\ParamMap;
use Titon\Route\QueryMap;
use Titon\Route\Router;
use Titon\Route\UrlBuilder;

/**
 * --------------------------------------------------------------
 *  Helper Functions
 * --------------------------------------------------------------
 *
 * Defines global helper functions for common use cases.
 */

/**
 * @see Titon\Route\UrlBuilder::build()
 */
function link_to(string $key, ParamMap $params = Map {}, QueryMap $query = Map {}): string {
    $router = Router::registry();

    invariant($router instanceof Router, 'Must be a Router');

    return UrlBuilder::registry($router)->build($key, $params, $query);
}

/**
 * @see Titon\Route\UrlBuilder::url()
 */
function url(): string {
    $router = Router::registry();

    invariant($router instanceof Router, 'Must be a Router');

    return UrlBuilder::registry($router)->url();
}