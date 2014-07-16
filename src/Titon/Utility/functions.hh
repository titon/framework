<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

use Titon\Utility\Config;
use Titon\Utility\Converter;
use Titon\Utility\Traverse;
use Titon\Utility\Inflector;
use Titon\Utility\Path;
use Titon\Utility\Registry;
use Titon\Utility\Sanitize;
use Titon\Utility\Str;

/**
 * @see Titon\Utility\Config::get()
 * @see Titon\Utility\Config::set()
 */
if (!function_exists('config')) {
    function config(string $key, ?mixed $value = null): ?mixed {
        if ($value !== null) {
            Config::set($key, $value);
            return true;
        }

        return Config::get($key);
    }
}

/**
 * @see Titon\Utility\Registry::get()
 * @see Titon\Utility\Registry::set()
 */
if (!function_exists('registry')) {
    function registry(string $key, ?mixed $object = null): ?mixed {
        if ($object !== null) {
            return Registry::set($object, $key);
        }

        return Registry::get($key);
    }
}

/**
 * @see Titon\Utility\Registry::factory()
 */
if (!function_exists('factory')) {
    function factory(string $key): ?mixed {
        return Registry::factory($key);
    }
}

/**
 * @see Titon\Utility\Traverse::depth()
 */
if (!function_exists('map_depth')) {
    function map_depth(Collection $collection): Collection {
        return Traverse::depth($collection);
    }
}

/**
 * @see Titon\Utility\Traverse::each()
 */
if (!function_exists('map_each')) {
    function map_each(Collection $collection, Closure $callback, bool $recursive = true): Collection {
        return Traverse::each($collection, $callback, $recursive);
    }
}

/**
 * @see Titon\Utility\Traverse::every()
 */
if (!function_exists('map_every')) {
    function map_every(Collection $collection, Closure $callback): bool {
        return Traverse::every($collection, $callback);
    }
}

/**
 * @see Titon\Utility\Traverse::exclude()
 */
if (!function_exists('map_exclude')) {
    function map_exclude(Map $collection, Vector<string> $keys): Map<string, mixed> {
        return Traverse::exclude($set, $keys);
    }
}

/**
 * @see Titon\Utility\Traverse::expand()
 */
if (!function_exists('map_expand')) {
    function map_expand(Map $collection): Map<string, mixed> {
        return Traverse::expand($set);
    }
}

/**
 * @see Titon\Utility\Traverse::filter()
 */
if (!function_exists('map_filter')) {
    function map_filter(Collection $collection, bool $recursive = true, ?Closure $callback = null): Collection {
        return Traverse::filter($collection, $recursive, $callback);
    }
}

/**
 * @see Titon\Utility\Traverse::flatten()
 */
if (!function_exists('map_flatten')) {
    function map_flatten(Collection $collection, string $path = ''): Map<string, mixed> {
        return Traverse::flatten($collection, $path);
    }
}

/**
 * @see Titon\Utility\Traverse::get()
 */
if (!function_exists('map_get')) {
    function map_get(Map $collection, string $path = ''): ?mixed {
        return Traverse::get($collection, $path);
    }
}

/**
 * @see Titon\Utility\Traverse::has()
 */
if (!function_exists('map_has')) {
    function map_has(Map $collection, string $path): bool {
        return Traverse::has($collection, $path);
    }
}

/**
 * @see Titon\Utility\Traverse::inject()
 */
if (!function_exists('map_inject')) {
    function map_inject(Map $collection, string $path, mixed $value): Map<string, mixed> {
        return Traverse::inject($collection, $path, $value);
    }
}

/**
 * @see Titon\Utility\Traverse::keyOf()
 */
if (!function_exists('map_key_of')) {
    function map_key_of(Collection $collection, mixed $match): ?mixed {
        return Traverse::keyOf($collection, $match);
    }
}

/**
 * @see Titon\Utility\Traverse::pluck()
 */
if (!function_exists('map_pluck')) {
    function map_pluck(Collection $collection, string $path): Vector<mixed> {
        return Traverse::pluck($collection, $path);
    }
}

/**
 * @see Titon\Utility\Traverse::remove()
 */
if (!function_exists('map_remove')) {
    function map_remove(Map $collection, string $path): Map<string, mixed> {
        return Traverse::remove($collection, $path);
    }
}

/**
 * @see Titon\Utility\Traverse::set()
 */
if (!function_exists('map_set')) {
    function map_set(Collection $collection, mixed $path, ?mixed $value = null): Map<string, mixed> {
        return Traverse::set($collection, $path, $value);
    }
}

/**
 * @see Titon\Utility\Traverse::some()
 */
if (!function_exists('map_some')) {
    function map_some(Collection $collection, Closure $callback): bool {
        return Traverse::some($collection, $callback);
    }
}

/**
 * @see Titon\Utility\Str::compare()
 */
if (!function_exists('str_compare')) {
    function str_compare(string $string, string $value, int $length = 0): int {
        return Str::compare($string, $value, $length);
    }
}

/**
 * @see Titon\Utility\Str::contains()
 */
if (!function_exists('str_contains')) {
    function str_contains(string $string, string $needle, bool $strict = true, int $offset = 0): bool {
        return Str::contains($string, $value, $length);
    }
}

/**
 * @see Titon\Utility\Str::endsWith()
 */
if (!function_exists('str_ends_with')) {
    function str_ends_with(string $string, string $needle, bool $strict = true): bool {
        return Str::endsWith($string, $needle, $strict);
    }
}

/**
 * @see Titon\Utility\Str::generate()
 */
if (!function_exists('str_generate')) {
    function str_generate(int $length, string $seed = Str::ALNUM): string {
        return Str::generate($length, $seed);
    }
}

/**
 * @see Titon\Utility\Str::insert()
 */
if (!function_exists('str_insert')) {
    function str_insert(string $string, Map<string, mixed> $data, Map<string, mixed> $options = Map {}): string {
        return Str::insert($string, $data, $options);
    }
}

/**
 * @see Titon\Utility\Str::listing()
 */
if (!function_exists('str_listing')) {
    function str_listing(Vector<string> $items, string $glue = ' &amp; ', string $sep = ', '): string {
        return Str::listing($items, $glue, $sep);
    }
}

/**
 * @see Titon\Utility\Str::shorten()
 */
if (!function_exists('str_shorten')) {
    function str_shorten(string $string, int $limit = 25, string $glue = ' &hellip; '): string {
        return Str::shorten($string, $limit, $glue);
    }
}

/**
 * @see Titon\Utility\Str::startsWith()
 */
if (!function_exists('str_starts_with')) {
    function str_starts_with(string $string, string $needle, bool $strict = true): bool {
        return Str::startsWith($string, $needle, $strict);
    }
}

/**
 * @see Titon\Utility\Str::truncate()
 */
if (!function_exists('str_truncate')) {
    function str_truncate(string $string, int $limit = 25, Map<string, mixed> $options = Map {}): string {
        return Str::truncate($string, $limit, $options);
    }
}

/**
 * @see Titon\Utility\Converter::toArray()
 */
if (!function_exists('to_array')) {
    function to_array(mixed $data): array {
        return Converter::toArray($data);
    }
}

/**
 * @see Titon\Utility\Converter::toJson()
 */
if (!function_exists('to_json')) {
    function to_json(mixed $data): string {
        return Converter::toJson($data);
    }
}

/**
 * @see Titon\Utility\Converter::toSerialize()
 */
if (!function_exists('to_serialize')) {
    function to_serialize(mixed $data): string {
        return Converter::toSerialize($data);
    }
}

/**
 * @see Titon\Utility\Converter::toXml()
 */
if (!function_exists('to_xml')) {
    function to_xml(mixed $data): string {
        return Converter::toXml($data);
    }
}

/**
 * @see Titon\Utility\Inflector::camelCase()
 */
if (!function_exists('camel_case')) {
    function camel_case(string $string): string {
        return Inflector::camelCase($string);
    }
}

/**
 * @see Titon\Utility\Inflector::fileName()
 */
if (!function_exists('file_name')) {
    function file_name(string $string): string {
        return Inflector::fileName($string);
    }
}

/**
 * @see Titon\Utility\Inflector::hyphenate()
 */
if (!function_exists('hyphenate')) {
    function hyphenate(string $string): string {
        return Inflector::hyphenate($string);
    }
}

/**
 * @see Titon\Utility\Inflector::normalCase()
 */
if (!function_exists('normal_case')) {
    function normal_case(string $string): string {
        return Inflector::normalCase($string);
    }
}

/**
 * @see Titon\Utility\Inflector::slug()
 */
if (!function_exists('slug')) {
    function slug(string $string): string {
        return Inflector::slug($string);
    }
}

/**
 * @see Titon\Utility\Inflector::snakeCase()
 */
if (!function_exists('snake_case')) {
    function snake_case(string $string): string {
        return Inflector::snakeCase($string);
    }
}

/**
 * @see Titon\Utility\Inflector::titleCase()
 */
if (!function_exists('title_case')) {
    function title_case(string $string): string {
        return Inflector::titleCase($string);
    }
}

/**
 * @see Titon\Utility\Inflector::underscore()
 */
if (!function_exists('underscore')) {
    function underscore(string $string): string {
        return Inflector::underscore($string);
    }
}

/**
 * @see Titon\Utility\Path::className()
 */
if (!function_exists('class_name')) {
    function class_name(string $string, string $separator = Path::PACKAGE): string {
        return Path::className($string, $separator);
    }
}

/**
 * @see Titon\Utility\Path::packageName()
 */
if (!function_exists('package_name')) {
    function package_name(string $string, string $separator = Path::PACKAGE): string {
        return Path::packageName($string, $separator);
    }
}

/**
 * @see Titon\Utility\Path::alias()
 */
if (!function_exists('path_alias')) {
    function path_alias(?string $file, ?Map<string, string> $paths = Map {}): string {
        return Path::alias($file, $paths);
    }
}

/**
 * @see Titon\Utility\Path::ds()
 */
if (!function_exists('ds')) {
    function ds(string $string): string {
        return Path::ds($string);
    }
}

/**
 * @see Titon\Utility\Path::ext()
 */
if (!function_exists('ext')) {
    function ext(string $string): string {
        return Path::ext($string);
    }
}

/**
 * @see Titon\Utility\Sanitize::escape()
 */
if (!function_exists('esc')) {
    function esc(string $value, Map<string, mixed> $options = Map {}): string {
        return Sanitize::escape($value, $options);
    }
}

/**
 * @see Titon\Utility\Sanitize::html()
 */
if (!function_exists('html')) {
    function html(string $value, Map<string, mixed> $options = Map {}): string {
        return Sanitize::html($value, $options);
    }
}

/**
 * @see Titon\Utility\Sanitize::newlines()
 */
if (!function_exists('nl')) {
    function nl(string $value, Map<string, mixed> $options = Map {}): string {
        return Sanitize::newlines($value, $options);
    }
}

/**
 * @see Titon\Utility\Sanitize::whitespace()
 */
if (!function_exists('ws')) {
    function ws(string $value, Map<string, mixed> $options = Map {}): string {
        return Sanitize::whitespace($value, $options);
    }
}

/**
 * @see Titon\Utility\Sanitize::xss()
 */
if (!function_exists('xss')) {
    function xss(string $value, Map<string, mixed> $options = Map {}): string {
        return Sanitize::xss($value, $options);
    }
}