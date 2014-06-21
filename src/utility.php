<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

use Titon\Utility\Converter;
use Titon\Utility\Hash;
use Titon\Utility\Inflector;
use Titon\Utility\Path;
use Titon\Utility\Sanitize;
use Titon\Utility\String;

/**
 * @see Titon\Utility\Hash::depth()
 */
if (!function_exists('array_depth')) {
    function array_depth($set) {
        return Hash::depth($set);
    }
}

/**
 * @see Titon\Utility\Hash::each()
 */
if (!function_exists('array_each')) {
    function array_each(array $set, Closure $callback, $recursive = true) {
        return Hash::each($set, $callback, $recursive);
    }
}

/**
 * @see Titon\Utility\Hash::every()
 */
if (!function_exists('array_every')) {
    function array_every(array $set, Closure $callback) {
        return Hash::every($set, $callback);
    }
}

/**
 * @see Titon\Utility\Hash::exclude()
 */
if (!function_exists('array_exclude')) {
    function array_exclude(array $set, array $keys) {
        return Hash::exclude($set, $keys);
    }
}

/**
 * @see Titon\Utility\Hash::expand()
 */
if (!function_exists('array_expand')) {
    function array_expand(array $set) {
        return Hash::expand($set);
    }
}

/**
 * @see Titon\Utility\Hash::flatten()
 */
if (!function_exists('array_flatten')) {
    function array_flatten(array $set, $path = null) {
        return Hash::flatten($set, $path);
    }
}

/**
 * @see Titon\Utility\Hash::get()
 */
if (!function_exists('array_get')) {
    function array_get(array $set, $path = null) {
        return Hash::get($set, $path);
    }
}

/**
 * @see Titon\Utility\Hash::has()
 */
if (!function_exists('array_has')) {
    function array_has(array $set, $path) {
        return Hash::has($set, $path);
    }
}

/**
 * @see Titon\Utility\Hash::inject()
 */
if (!function_exists('array_inject')) {
    function array_inject(array $set, $path, $value) {
        return Hash::inject($set, $path, $value);
    }
}

/**
 * @see Titon\Utility\Hash::keyOf()
 */
if (!function_exists('array_key_of')) {
    function array_key_of(array $set, $match) {
        return Hash::keyOf($set, $match);
    }
}

/**
 * @see Titon\Utility\Hash::pluck()
 */
if (!function_exists('array_pluck')) {
    function array_pluck(array $set, $path) {
        return Hash::pluck($set, $path);
    }
}

/**
 * @see Titon\Utility\Hash::remove()
 */
if (!function_exists('array_remove')) {
    function array_remove(array $set, $path) {
        return Hash::remove($set, $path);
    }
}

/**
 * @see Titon\Utility\Hash::set()
 */
if (!function_exists('array_set')) {
    function array_set(array $set, $path, $value) {
        return Hash::set($set, $path, $value);
    }
}

/**
 * @see Titon\Utility\Hash::some()
 */
if (!function_exists('array_some')) {
    function array_some(array $set, Closure $callback) {
        return Hash::some($set, $callback);
    }
}

/**
 * @see Titon\Utility\String::compare()
 */
if (!function_exists('str_compare')) {
    function str_compare($string, $value, $strict = true, $length = 0) {
        return String::compare($string, $value, $strict, $length);
    }
}

/**
 * @see Titon\Utility\String::endsWith()
 */
if (!function_exists('str_ends_with')) {
    function str_ends_with($string, $needle, $strict = true) {
        return String::endsWith($string, $needle, $strict);
    }
}

/**
 * @see Titon\Utility\String::generate()
 */
if (!function_exists('str_generate')) {
    function str_generate($length, $seed = String::ALNUM) {
        return String::generate($length, $seed);
    }
}

/**
 * @see Titon\Utility\String::insert()
 */
if (!function_exists('str_insert')) {
    function str_insert($string, array $data, array $options = array()) {
        return String::insert($string, $data, $options);
    }
}

/**
 * @see Titon\Utility\String::listing()
 */
if (!function_exists('str_listing')) {
    function str_listing($items, $glue = ' &amp; ', $sep = ', ') {
        return String::listing($items, $glue, $sep);
    }
}

/**
 * @see Titon\Utility\String::shorten()
 */
if (!function_exists('str_shorten')) {
    function str_shorten($string, $limit = 25, $glue = ' &hellip; ') {
        return String::shorten($string, $limit, $glue);
    }
}

/**
 * @see Titon\Utility\String::startsWith()
 */
if (!function_exists('str_starts_with')) {
    function str_starts_with($string, $needle, $strict = true) {
        return String::startsWith($string, $needle, $strict);
    }
}

/**
 * @see Titon\Utility\String::truncate()
 */
if (!function_exists('str_truncate')) {
    function str_truncate($string, $limit = 25, array $options = array()) {
        return String::truncate($string, $limit, $options);
    }
}

/**
 * @see Titon\Utility\Converter::toArray()
 */
if (!function_exists('to_array')) {
    function to_array($data) {
        return Converter::toArray($data);
    }
}

/**
 * @see Titon\Utility\Converter::toJson()
 */
if (!function_exists('to_json')) {
    function to_json($data) {
        return Converter::toJson($data);
    }
}

/**
 * @see Titon\Utility\Converter::toObject()
 */
if (!function_exists('to_object')) {
    function to_object($data) {
        return Converter::toObject($data);
    }
}

/**
 * @see Titon\Utility\Converter::toSerialize()
 */
if (!function_exists('to_serialize')) {
    function to_serialize($data) {
        return Converter::toSerialize($data);
    }
}

/**
 * @see Titon\Utility\Converter::toXml()
 */
if (!function_exists('to_xml')) {
    function to_xml($data) {
        return Converter::toXml($data);
    }
}

/**
 * @see Titon\Utility\Inflector::camelCase()
 */
if (!function_exists('camel_case')) {
    function camel_case($string) {
        return Inflector::camelCase($string);
    }
}

/**
 * @see Titon\Utility\Inflector::fileName()
 */
if (!function_exists('file_name')) {
    function file_name($string) {
        return Inflector::fileName($string);
    }
}

/**
 * @see Titon\Utility\Inflector::hyphenate()
 */
if (!function_exists('hyphenate')) {
    function hyphenate($string) {
        return Inflector::hyphenate($string);
    }
}

/**
 * @see Titon\Utility\Inflector::normalCase()
 */
if (!function_exists('normal_case')) {
    function normal_case($string) {
        return Inflector::normalCase($string);
    }
}

/**
 * @see Titon\Utility\Inflector::slug()
 */
if (!function_exists('slug')) {
    function slug($string) {
        return Inflector::slug($string);
    }
}

/**
 * @see Titon\Utility\Inflector::snakeCase()
 */
if (!function_exists('snake_case')) {
    function snake_case($string) {
        return Inflector::snakeCase($string);
    }
}

/**
 * @see Titon\Utility\Inflector::titleCase()
 */
if (!function_exists('title_case')) {
    function title_case($string) {
        return Inflector::titleCase($string);
    }
}

/**
 * @see Titon\Utility\Inflector::underscore()
 */
if (!function_exists('underscore')) {
    function underscore($string) {
        return Inflector::underscore($string);
    }
}

/**
 * @see Titon\Utility\Path::className()
 */
if (!function_exists('class_name')) {
    function class_name($string, $separator = Path::PACKAGE) {
        return Path::className($string, $separator);
    }
}

/**
 * @see Titon\Utility\Path::packageName()
 */
if (!function_exists('package_name')) {
    function package_name($string, $separator = Path::PACKAGE) {
        return Path::packageName($string, $separator);
    }
}

/**
 * @see Titon\Utility\Path::ds()
 */
if (!function_exists('ds')) {
    function ds($string) {
        return Path::ds($string);
    }
}

/**
 * @see Titon\Utility\Path::ext()
 */
if (!function_exists('ext')) {
    function ext($string) {
        return Path::ext($string);
    }
}

/**
 * @see Titon\Utility\Sanitize::escape()
 */
if (!function_exists('esc')) {
    function esc($value, array $options = array()) {
        return Sanitize::escape($value, $options);
    }
}

/**
 * @see Titon\Utility\Sanitize::html()
 */
if (!function_exists('html')) {
    function html($value, array $options = array()) {
        return Sanitize::html($value, $options);
    }
}

/**
 * @see Titon\Utility\Sanitize::newlines()
 */
if (!function_exists('nl')) {
    function nl($value, array $options = array()) {
        return Sanitize::newlines($value, $options);
    }
}

/**
 * @see Titon\Utility\Sanitize::whitespace()
 */
if (!function_exists('ws')) {
    function ws($value, array $options = array()) {
        return Sanitize::whitespace($value, $options);
    }
}

/**
 * @see Titon\Utility\Sanitize::xss()
 */
if (!function_exists('xss')) {
    function xss($value, array $options = array()) {
        return Sanitize::xss($value, $options);
    }
}