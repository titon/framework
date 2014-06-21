<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

use Titon\Common\Config;
use Titon\Common\Registry;

/**
 * @see Titon\Common\Config::get()
 * @see Titon\Common\Config::set()
 */
if (!function_exists('config')) {
    function config($key, $value = null) {
        if ($value !== null) {
            Config::set($key, $value);
            return true;
        }

        return Config::get($key);
    }
}

/**
 * @see Titon\Common\Registry::get()
 * @see Titon\Common\Registry::set()
 */
if (!function_exists('registry')) {
    function registry($key, $object = null) {
        if ($object !== null) {
            return Registry::set($object, $key);
        }

        return Registry::get($key);
    }
}

/**
 * @see Titon\Common\Registry::factory()
 */
if (!function_exists('factory')) {
    function factory($key) {
        return Registry::factory($key);
    }
}