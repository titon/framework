<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

use Titon\Route\ParamMap;
use Titon\Route\QueryMap;
use Titon\Route\Router;
use Titon\Route\UrlBuilder;

/**
 * @see Titon\Route\UrlBuilder::build()
 */
function link_to(string $key, ParamMap $params = Map {}, QueryMap $query = Map {}): string {
    return UrlBuilder::registry(Router::registry())->build($key, $params, $query);
}

/**
 * @see Titon\Route\UrlBuilder::url()
 */
function url(): string {
    return UrlBuilder::registry(Router::registry())->url();
}