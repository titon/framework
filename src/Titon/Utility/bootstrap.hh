<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

/**
 * --------------------------------------------------------------
 *  Type Aliases
 * --------------------------------------------------------------
 *
 * Defines type aliases that are used by the utility package.
 */

namespace Titon\Utility {
    type ArgumentList = array<mixed>;
    type ConfigMap = Map<string, mixed>;
    type OptionMap = Map<string, mixed>;
    type RegistryCallback<T> = (function(): T);
    type RegistryMap<T> = Map<string, T>;
    type TimeMessageMap = Map<string, Map<int, string>>;
}

namespace Titon\Utility\State {
    type GlobalMap = Map<string, mixed>;
}

/**
 * --------------------------------------------------------------
 *  Global State Initialization
 * --------------------------------------------------------------
 *
 * Initialize the global state by bootstrapping all the super
 * globals into immutable read-only collections.
 */

namespace {
    use Titon\Utility\State\Cookie;
    use Titon\Utility\State\Env;
    use Titon\Utility\State\Files;
    use Titon\Utility\State\Get;
    use Titon\Utility\State\Post;
    use Titon\Utility\State\Server;
    use Titon\Utility\State\Session;

    Cookie::initialize($_COOKIE);
    Env::initialize($_ENV);
    Files::initialize($_FILES);
    Get::initialize($_GET);
    Post::initialize($_POST);
    Server::initialize($_SERVER);
    Session::initialize($_SESSION);
}

/**
 * --------------------------------------------------------------
 *  Helper Functions
 * --------------------------------------------------------------
 *
 * Defines global helper functions for common use cases.
 */

namespace {
    use Titon\Utility\ArgumentList;
    use Titon\Utility\Col;
    use Titon\Utility\Config;
    use Titon\Utility\Inflect;
    use Titon\Utility\Path;
    use Titon\Utility\Sanitize;
    use Titon\Utility\Registry;
    use Titon\Utility\Str;

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
    function factory(string $key, ArgumentList $args): mixed {
        return Registry::factory($key, $args);
    }

    /**
     * @see Titon\Utility\Col::depth()
     */
    function col_depth<Tv>(Traversable<Tv> $collection): int {
        return Col::depth($collection);
    }

    /**
     * @see Titon\Utility\Col::each()
     */
    function col_each<Tk, Tv>(Indexish<Tk, Tv> $collection, (function(Tk, Tv): Tv) $callback): Indexish<Tk, Tv> {
        return Col::each($collection, $callback);
    }

    /**
     * @see Titon\Utility\Col::every()
     */
    function col_every<Tk, Tv>(Indexish<Tk, Tv> $collection, (function(Tk, Tv): bool) $callback): bool {
        return Col::every($collection, $callback);
    }

    /**
     * @see Titon\Utility\Col::exclude()
     */
    function col_exclude<Tk, Tv>(Map<Tk, Tv> $map, Vector<Tk> $keys): Map<Tk, Tv> {
        return Col::exclude($map, $keys);
    }

    /**
     * @see Titon\Utility\Col::expand()
     */
    function col_expand<Tk, Tv>(Map<Tk, Tv> $map): Map<Tk, Tv> {
        return Col::expand($map);
    }

    /**
     * @see Titon\Utility\Col::flatten()
     */
    function col_flatten<Tk, Tv>(Map<Tk, Tv> $map, string $path = ''): Map<Tk, Tv> {
        return Col::flatten($map, $path);
    }

    /**
     * @see Titon\Utility\Col::get()
     */
    function col_get<Tk, Tv>(Map<Tk, Tv> $map, Tk $path, ?Tv $default = null): ?Tv {
        return Col::get($map, $path, $default);
    }

    /**
     * @see Titon\Utility\Col::has()
     */
    function col_has<Tk, Tv>(Map<Tk, Tv> $map, Tk $path): bool {
        return Col::has($map, $path);
    }

    /**
     * @see Titon\Utility\Col::inject()
     */
    function col_inject<Tk, Tv>(Map<Tk, Tv> $map, Tk $path, Tv $value): Map<Tk, Tv> {
        return Col::inject($map, $path, $value);
    }

    /**
     * @see Titon\Utility\Col::keyOf()
     */
    function col_key_of<Tk, Tv, Tm>(Indexish<Tk, Tv> $collection, Tm $match): string {
        return Col::keyOf($collection, $match);
    }

    /**
     * @see Titon\Utility\Col::pluck()
     */
    function col_pluck<Tk, Tv>(Map<Tk, Tv> $map, Tk $path): Vector<Tv> {
        return Col::pluck($map, $path);
    }

    /**
     * @see Titon\Utility\Col::remove()
     */
    function col_remove<Tk, Tv>(Map<Tk, Tv> $map, Tk $path): Map<Tk, Tv> {
        return Col::remove($map, $path);
    }

    /**
     * @see Titon\Utility\Col::set()
     */
    function col_set<Tk, Tv>(Map<Tk, Tv> $map, Tk $path, ?Tv $value = null): Map<Tk, Tv> {
        return Col::set($map, $path, $value);
    }

    /**
     * @see Titon\Utility\Col::some()
     */
    function col_some<Tk, Tv>(Indexish<Tk, Tv> $collection, (function(Tk, Tv): bool) $callback): bool {
        return Col::some($collection, $callback);
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
     * @see Titon\Utility\Col::toArray()
     */
    function to_array<Tk, Tv, Tr>(Tr $data): array<Tk, Tv> {
        return Col::toArray($data);
    }

    /**
     * @see Titon\Utility\Col::toMap()
     */
    function to_map<Tk, Tv, Tr>(Tr $data): Map<Tk, Tv> {
        return Col::toMap($data);
    }

    /**
     * @see Titon\Utility\Col::toVector()
     */
    function to_vector<Tv, Tr>(Tr $data): Vector<Tv> {
        return Col::toVector($data);
    }

    /**
     * @see Titon\Utility\Inflector::camelCase()
     */
    function camel_case(string $string): string {
        return Inflect::camelCase($string);
    }

    /**
     * @see Titon\Utility\Inflector::fileName()
     */
    function file_name(string $string, string $ext = 'php'): string {
        return Inflect::fileName($string, $ext);
    }

    /**
     * @see Titon\Utility\Inflector::hyphenate()
     */
    function hyphenate(string $string): string {
        return Inflect::hyphenate($string);
    }

    /**
     * @see Titon\Utility\Inflector::normalCase()
     */
    function normal_case(string $string): string {
        return Inflect::normalCase($string);
    }

    /**
     * @see Titon\Utility\Inflector::slug()
     */
    function slug(string $string): string {
        return Inflect::slug($string);
    }

    /**
     * @see Titon\Utility\Inflector::snakeCase()
     */
    function snake_case(string $string): string {
        return Inflect::snakeCase($string);
    }

    /**
     * @see Titon\Utility\Inflector::titleCase()
     */
    function title_case(string $string): string {
        return Inflect::titleCase($string);
    }

    /**
     * @see Titon\Utility\Inflector::underscore()
     */
    function underscore(string $string): string {
        return Inflect::underscore($string);
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
}
