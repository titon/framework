<?hh //strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

use Titon\Type\Collection;
use Titon\Type\Map;
use Titon\Type\String;

/**
 * @see Titon\Type\String
 */
if (!function_exists('string')) {
    function string($string) {
        return new String($string);
    }
}

/**
 * @see Titon\Type\Map
 */
if (!function_exists('map')) {
    function map(array $array) {
        return new Map($array);
    }
}

/**
 * @see Titon\Type\Collection
 */
if (!function_exists('collection')) {
    function collection(array $array) {
        return new Collection($array);
    }
}