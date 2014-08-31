<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

use Titon\Utility\Config;
use Titon\Utility\Converter;
use Titon\Utility\Inflector;
use Titon\Utility\Path;
use Titon\Utility\Sanitize;
use Titon\Utility\Registry;
use Titon\Utility\Str;
use Titon\Utility\Traverse;

/**
 * @see Titon\Utility\Config::get()
 * @see Titon\Utility\Config::set()
 */
function config(string $key, mixed $value = null): mixed {
    if ($value !== null) {
        Config::set($key, $value);

        return $value;
    }

    return Config::get($key);
}

/**
 * @see Titon\Utility\Registry::get()
 * @see Titon\Utility\Registry::set()
 */
function registry(string $key, mixed $object = null): mixed {
    if ($object !== null) {
        return Registry::set($object, $key);
    }

    return Registry::get($key);
}

/**
 * @see Titon\Utility\Registry::factory()
 */
function factory(string $key): mixed {
    return Registry::factory($key);
}

/**
 * @see Titon\Utility\Traverse::depth()
 */
function map_depth(Traversable $collection = MB_CASE_TITLE): int {
    return Traverse::depth($collection);
}

/**
 * @see Titon\Utility\Traverse::each()
 */
function map_each(Traversable $collection, (function(mixed, mixed): mixed) $callback, bool $recursive = true): Traversable {
    return Traverse::each($collection, $callback, $recursive);
}

/**
 * @see Titon\Utility\Traverse::every()
 */
function map_every(Traversable $collection, (function(mixed, mixed): bool) $callback): bool {
    return Traverse::every($collection, $callback);
}

/**
 * @see Titon\Utility\Traverse::exclude()
 */
function map_exclude(Traversable $collection, Vector<string> $keys): Traversable {
    return Traverse::exclude($collection, $keys);
}

/**
 * @see Titon\Utility\Traverse::expand()
 */
function map_expand(Traversable $collection): Traversable {
    return Traverse::expand($collection);
}

/**
 * @see Titon\Utility\Traverse::filter()
 */
function map_filter(Traversable $collection, bool $recursive = true, ?(function(mixed): int) $callback = null): Traversable {
    return Traverse::filter($collection, $recursive, $callback);
}

/**
 * @see Titon\Utility\Traverse::flatten()
 */
function map_flatten(Traversable $collection, string $path = ''): Map<string, mixed> {
    return Traverse::flatten($collection, $path);
}

/**
 * @see Titon\Utility\Traverse::get()
 */
function map_get(Traversable $collection, string $path = ''): mixed {
    return Traverse::get($collection, $path);
}

/**
 * @see Titon\Utility\Traverse::has()
 */
function map_has(Traversable $collection, string $path): bool {
    return Traverse::has($collection, $path);
}

/**
 * @see Titon\Utility\Traverse::inject()
 */
function map_inject(Traversable $collection, string $path, mixed $value): Traversable {
    return Traverse::inject($collection, $path, $value);
}

/**
 * @see Titon\Utility\Traverse::keyOf()
 */
function map_key_of(Traversable $collection, mixed $match): mixed {
    return Traverse::keyOf($collection, $match);
}

/**
 * @see Titon\Utility\Traverse::pluck()
 */
function map_pluck(Traversable $collection, string $path): Vector<mixed> {
    return Traverse::pluck($collection, $path);
}

/**
 * @see Titon\Utility\Traverse::remove()
 */
function map_remove(Traversable $collection, string $path): Traversable {
    return Traverse::remove($collection, $path);
}

/**
 * @see Titon\Utility\Traverse::set()
 */
function map_set(Traversable $collection, mixed $path, mixed $value = null): Traversable {
    return Traverse::set($collection, $path, $value);
}

/**
 * @see Titon\Utility\Traverse::some()
 */
function map_some(Traversable $collection, Closure $callback): bool {
    return Traverse::some($collection, $callback);
}

/**
 * @see Titon\Utility\Str::compare()
 */
function str_compare(string $string, string $value, int $length = 0): int {
    return Str::compare($string, $value, $length);
}

/**
 * @see Titon\Utility\Str::contains()
 */
function str_contains(string $string, string $needle, bool $strict = true, int $offset = 0): bool {
    return Str::contains($string, $needle, $strict, $offset);
}

/**
 * @see Titon\Utility\Str::endsWith()
 */
function str_ends_with(string $string, string $needle, bool $strict = true): bool {
    return Str::endsWith($string, $needle, $strict);
}

/**
 * @see Titon\Utility\Str::generate()
 */
function str_generate(int $length, string $seed = Str::ALNUM): string {
    return Str::generate($length, $seed);
}

/**
 * @see Titon\Utility\Str::insert()
 */
function str_insert(string $string, Map<string, mixed> $data, Map<string, mixed> $options = Map {}): string {
    return Str::insert($string, $data, $options);
}

/**
 * @see Titon\Utility\Str::listing()
 */
function str_listing(Vector<string> $items, string $glue = ' &amp; ', string $sep = ', '): string {
    return Str::listing($items, $glue, $sep);
}

/**
 * @see Titon\Utility\Str::shorten()
 */
function str_shorten(string $string, int $limit = 25, string $glue = ' &hellip; '): string {
    return Str::shorten($string, $limit, $glue);
}

/**
 * @see Titon\Utility\Str::startsWith()
 */
function str_starts_with(string $string, string $needle, bool $strict = true): bool {
    return Str::startsWith($string, $needle, $strict);
}

/**
 * @see Titon\Utility\Str::truncate()
 */
function str_truncate(string $string, int $limit = 25, Map<string, mixed> $options = Map {}): string {
    return Str::truncate($string, $limit, $options);
}

/**
 * @see Titon\Utility\Converter::toArray()
 */
function to_array(mixed $data): array<mixed, mixed> {
    return Converter::toArray($data);
}

/**
 * @see Titon\Utility\Converter::toJson()
 */
function to_json(mixed $data): string {
    return Converter::toJson($data);
}

/**
 * @see Titon\Utility\Converter::toMap()
 */
function to_map(mixed $data): Map<mixed, mixed> {
    return Converter::toMap($data);
}

/**
 * @see Titon\Utility\Converter::toSerialize()
 */
function to_serialize(mixed $data): string {
    return Converter::toSerialize($data);
}

/**
 * @see Titon\Utility\Converter::toVector()
 */
function to_vector(mixed $data): Vector<mixed> {
    return Converter::toVector($data);
}

/**
 * @see Titon\Utility\Converter::toXml()
 */
function to_xml(mixed $data): string {
    return Converter::toXml($data);
}

/**
 * @see Titon\Utility\Inflector::camelCase()
 */
function camel_case(string $string): string {
    return Inflector::camelCase($string);
}

/**
 * @see Titon\Utility\Inflector::fileName()
 */
function file_name(string $string): string {
    return Inflector::fileName($string);
}

/**
 * @see Titon\Utility\Inflector::hyphenate()
 */
function hyphenate(string $string): string {
    return Inflector::hyphenate($string);
}

/**
 * @see Titon\Utility\Inflector::normalCase()
 */
function normal_case(string $string): string {
    return Inflector::normalCase($string);
}

/**
 * @see Titon\Utility\Inflector::slug()
 */
function slug(string $string): string {
    return Inflector::slug($string);
}

/**
 * @see Titon\Utility\Inflector::snakeCase()
 */
function snake_case(string $string): string {
    return Inflector::snakeCase($string);
}

/**
 * @see Titon\Utility\Inflector::titleCase()
 */
function title_case(string $string): string {
    return Inflector::titleCase($string);
}

/**
 * @see Titon\Utility\Inflector::underscore()
 */
function underscore(string $string): string {
    return Inflector::underscore($string);
}

/**
 * @see Titon\Utility\Path::className()
 */
function class_name(string $string, string $separator = Path::PACKAGE): string {
    return Path::className($string, $separator);
}

/**
 * @see Titon\Utility\Path::packageName()
 */
function package_name(string $string, string $separator = Path::PACKAGE): string {
    return Path::packageName($string, $separator);
}

/**
 * @see Titon\Utility\Path::alias()
 */
function path_alias(string $file, Map<string, string> $paths = Map {}): string {
    return Path::alias($file, $paths);
}

/**
 * @see Titon\Utility\Path::ds()
 */
function ds(string $string): string {
    return Path::ds($string);
}

/**
 * @see Titon\Utility\Path::ext()
 */
function ext(string $string): string {
    return Path::ext($string);
}

/**
 * @see Titon\Utility\Sanitize::escape()
 */
function esc(string $value, Map<string, mixed> $options = Map {}): string {
    return Sanitize::escape($value, $options);
}

/**
 * @see Titon\Utility\Sanitize::html()
 */
function html(string $value, Map<string, mixed> $options = Map {}): string {
    return Sanitize::html($value, $options);
}

/**
 * @see Titon\Utility\Sanitize::newlines()
 */
function nl(string $value, Map<string, mixed> $options = Map {}): string {
    return Sanitize::newlines($value, $options);
}

/**
 * @see Titon\Utility\Sanitize::whitespace()
 */
function ws(string $value, Map<string, mixed> $options = Map {}): string {
    return Sanitize::whitespace($value, $options);
}

/**
 * @see Titon\Utility\Sanitize::xss()
 */
function xss(string $value, Map<string, mixed> $options = Map {}): string {
    return Sanitize::xss($value, $options);
}