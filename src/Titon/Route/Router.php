<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

use Titon\Common\Cacheable;
use Titon\Common\Configurable;
use Titon\Common\FactoryAware;
use Titon\Event\Emittable;
use Titon\Route\Exception\InvalidFilterException;
use Titon\Route\Exception\InvalidRouteException;
use Titon\Route\Exception\MissingFilterException;
use Titon\Route\Exception\MissingSegmentException;
use Titon\Route\Exception\MissingRouteException;
use Titon\Route\Exception\NoMatchException;
use Titon\Route\Matcher\LoopMatcher;
use Titon\Utility\Config;
use Titon\Utility\Inflector;
use Titon\Utility\Hash;
use Titon\Utility\String;
use \Closure;

/**
 * The Router determines the current routing request, based on the URL address and environment.
 * Stores the current route, its parsed segments and the base URL.
 * Additionally, it will construct a URL based on an array of options, or deconstruct a URL into an array of options.
 * You can also define custom slugs or routes to be used for internal routing mechanisms.
 *
 * @package Titon\Route
 * @events
 *      route.preMatch(Router $router, $url)
 *      route.postMatch(Router $router, Route $route)
 */
class Router {
    use Cacheable, Configurable, Emittable, FactoryAware;

    /**
     * Base folder structure if the application was placed within a directory.
     *
     * @type string
     */
    protected static $_base = '/';

    /**
     * The matched route object.
     *
     * @type \Titon\Route\Route
     */
    protected $_current;

    /**
     * Default configuration.
     *
     * @type array
     */
    protected $_config = [
        'defaults' => ['module' => 'main', 'controller' => 'index', 'action' => 'index'],
        'prefixes' => ['locale', 'module'],
        'resourceMap' => [
            'list' => 'index',
            'create' => 'create',
            'read' => 'read',
            'update' => 'update',
            'delete' => 'delete'
        ]
    ];

    /**
     * List of filters to trigger for specific routes during a match.
     *
     * @type array
     */
    protected $_filters = [];

    /**
     * List of currently open groups (and their options) in the stack.
     *
     * @type array
     */
    protected $_groups = [];

    /**
     * The class to use for route matching.
     *
     * @type \Titon\Route\Matcher
     */
    protected $_matcher;

    /**
     * Manually defined aesthetic routes that re-route internally.
     *
     * @type \Titon\Route\Route[]
     */
    protected $_routes = [];

    /**
     * The current URL broken up into multiple segments: protocol, host, route, query, base
     *
     * @type array
     */
    protected $_segments = [];

    /**
     * Parses the current URL into multiple segments.
     *
     * @param array $config
     */
    public function __construct(array $config = []) {
        $this->applyConfig($config);

        // Determine if app is within a base folder
        $base = dirname(str_replace($_SERVER['DOCUMENT_ROOT'], '', $_SERVER['SCRIPT_FILENAME']));

        // Store the base to prepend to built routes
        if ($base && $base !== '.') {
            static::$_base = rtrim(str_replace('\\', '/', $base), '/') ?: '/';
        }

        // Store the current URL and query as router segments
        $this->_segments = Hash::merge(parse_url($_SERVER['REQUEST_URI']), [
            'scheme' => (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on') ? 'https' : 'http',
            'query' => $_GET,
            'host' => $_SERVER['HTTP_HOST'],
            'port' => $_SERVER['SERVER_PORT']
        ]);

        // Set a default matcher
        $this->setMatcher(new LoopMatcher());
    }

    /**
     * Maps the default routing objects and attempts to match.
     */
    public function initialize() {
        $this->_current = $this->match($this->getSegment('path'));
    }

    /**
     * Return all routes.
     *
     * @return \Titon\Route\Route[]
     */
    public function all() {
        return $this->getRoutes();
    }

    /**
     * Return the base URL if the app was not placed in the root directory.
     *
     * @return string
     */
    public static function base() {
        return static::$_base;
    }

    /**
     * Takes a route in array format and processes it down into a single string that represents an internal URL path.
     *
     * @param string|array $url
     * @return string
     * @throws \Titon\Route\Exception\InvalidRouteException
     */
    public function build($url = []) {
        return $this->cache([__METHOD__, $url], function() use ($url) {

            // Convert strings to array structure
            if (is_string($url)) {
                // Route by key syntax
                if (isset($this->_routes[$url])) {
                    $url = ['route' => $url];

                // Convert @ route to array
                } else if (strpos($url, '@') !== false) {
                    $url = $this->parse($url);

                // Literal URL, return immediately
                } else {
                    return $url;
                }
            }

            // Convert @ routes first as the logic should be different than route keys
            if (isset($url['route']) && strpos($url['route'], '@') !== false) {
                $url = array_merge($url, $this->parse($url['route']));
                unset($url['route']);
            }

            // Build it!
            $url = $this->defaults($url, $this->getConfig('defaults'));
            $base = $this->base();

            if ($url === $this->defaults([], $this->getConfig('defaults'))) {
                return $base;
            }

            $routes = $this->all();
            $path = [];
            $args = [];
            $query = [];
            $ext = '';
            $fragment = '';
            $prefixes = $this->getConfig('prefixes');

            // Extract all the values from the URL array
            foreach ($url as $key => $value) {
                if ($key === 'args') {
                    $args = (array) $value + $args;

                } else if ($key === 'query') {
                    $query = (array) $value + $query;

                } else if ($key === '#') {
                    $fragment = $value;

                } else if (is_numeric($key)) {
                    $args[] = $value;

                } else {
                    continue;
                }

                unset($url[$key]);
            }

            if ($base !== '/' && $base !== '') {
                $path[] = trim($base, '/');
            }

            // Reverse routing through the key
            if (isset($url['route'])) {
                if (empty($routes[$url['route']])) {
                    throw new InvalidRouteException(sprintf('Route with key %s does not exist', $url['route']));
                }

                $route = $routes[$url['route']];
                $routePath = str_replace([']', ')', '>'], '}', str_replace(['[', '(', '<'], '{', ltrim($route->getPath(), '/')));

                foreach ($route->getTokens() as $token) {
                    $tokenKey = $token['token'];

                    // Build the key found in the compiled path: {token} or {token?}
                    $tokenGroup = $tokenKey;

                    if ($token['optional']) {
                        $tokenGroup .= '?';
                    }

                    // Replace the token with a value
                    if (isset($url[$tokenKey])) {
                        $routePath = String::insert($routePath, [$tokenGroup => $url[$tokenKey]], ['escape' => false]);

                    } else if ($token['optional']) {
                        $routePath = str_replace(sprintf('/{%s}', $tokenGroup), '', $routePath);

                    } else {
                        throw new InvalidRouteException(sprintf('Missing %s token for %s route', $tokenKey, $url['route']));
                    }

                    unset($url[$tokenKey]);
                }

                $path[] = $routePath;

            // Module, controller, action
            } else {
                foreach ($prefixes as $prefix) {
                    if (!empty($url[$prefix])) {
                        $path[] = $url[$prefix];
                    }
                }

                // Hide controller and action if they are both set to index
                // But only if there are no arguments or extension
                $hasArgs = !empty($args);
                $hasExt = !empty($url['ext']);

                if ($url['controller'] !== 'index' || $url['action'] !== 'index' || $hasArgs || $hasExt) {
                    $path[] = $url['controller'];

                    if ($url['action'] !== 'index' || $hasArgs || $hasExt) {
                        $path[] = $url['action'];
                    }
                }

                if (!empty($url['ext'])) {
                    $ext = $url['ext'];
                }
            }

            foreach (array_merge($prefixes, ['controller', 'action', 'ext', 'route']) as $unset) {
                unset($url[$unset]);
            }

            // Append remaining to the query
            $query = array_merge($query, $url);

            // Action arguments
            if ($args) {
                foreach ($args as $arg) {
                    $path[] = urlencode($arg);
                }
            }

            $path = '/' . implode('/', $path);

            if ($ext) {
                $path .= '.' . $ext;
            }

            if ($query) {
                $path .= '?' . http_build_query($query, '', '&', PHP_QUERY_RFC1738);
            }

            if ($fragment) {
                if (is_array($fragment)) {
                    $path .= '#' . http_build_query($fragment, '', '&', PHP_QUERY_RFC1738);
                } else {
                    $path .= '#' . urlencode($fragment);
                }
            }

            return $path;
        });
    }

    /**
     * Return the current matched route object.
     *
     * @return \Titon\Route\Route
     */
    public function current() {
        return $this->_current;
    }

    /**
     * Maps the default routes and determines the controller and module.
     * Can be merged with a dynamic route to map missing segments.
     *
     * @uses Titon\Utility\Inflector
     *
     * @param array $data
     * @param array $defaults
     * @return array
     */
    public static function defaults(array $data = [], array $defaults = []) {
        $defaults = $defaults + [
            'module' => 'main',
            'controller' => 'index',
            'action' => 'index',
            'ext' => '',
            'query' => [],
            'args' => []
        ];

        $data = array_filter($data) + $defaults;
        $data['module'] = Inflector::route($data['module']);
        $data['controller'] = Inflector::route($data['controller']);
        $data['action'] = Inflector::route($data['action']);

        if ($locale = Config::get('titon.locale.current')) {
            $data['locale'] = Inflector::route($locale);
        }

        return $data;
    }

    /**
     * Map a route that only responds to a DELETE request.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return $this
     */
    public function delete($key, Route $route) {
        return $this->map($key, $route->setConfig('method', 'delete'));
    }

    /**
     * Map a filter to a callable function.
     * Bound filters are executed when a route is matched.
     *
     * @param string $key
     * @param callable $callback
     * @return $this
     * @throws \Titon\Route\Exception\InvalidFilterException
     */
    public function filter($key, $callback) {
        if ($callback instanceof Filter) {
            $this->_filters[$key] = [$callback, 'filter'];

        } else if (is_callable($callback)) {
            $this->_filters[$key] = $callback;

        } else {
            throw new InvalidFilterException(sprintf('Uncallable filter for %s', $key));
        }

        return $this;
    }

    /**
     * Map a route that only responds to a GET request.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return $this
     */
    public function get($key, Route $route) {
        return $this->map($key, $route->setConfig('method', ['get', 'head']));
    }

    /**
     * Return a filter by key.
     *
     * @param string $key
     * @return callable
     * @throws \Titon\Route\Exception\MissingFilterException
     */
    public function getFilter($key) {
        if (isset($this->_filters[$key])) {
            return $this->_filters[$key];
        }

        throw new MissingFilterException(sprintf('Filter %s does not exist', $key));
    }

    /**
     * Return all filters.
     *
     * @return callable[]
     */
    public function getFilters() {
        return $this->_filters;
    }

    /**
     * Return the matcher object.
     *
     * @return \Titon\Route\Matcher
     */
    public function getMatcher() {
        return $this->_matcher;
    }

    /**
     * Return a route by key.
     *
     * @param string $key
     * @return \Titon\Route\Route
     * @throws \Titon\Route\Exception\MissingRouteException
     */
    public function getRoute($key) {
        if (isset($this->_routes[$key])) {
            return $this->_routes[$key];
        }

        throw new MissingRouteException(sprintf('Route %s does not exist', $key));
    }

    /**
     * Return all routes.
     *
     * @return \Titon\Route\Route[]
     */
    public function getRoutes() {
        return $this->_routes;
    }

    /**
     * Return a segment by key.
     *
     * @param string $key
     * @return string
     * @throws \Titon\Route\Exception\MissingSegmentException
     */
    public function getSegment($key) {
        if (isset($this->_segments[$key])) {
            return $this->_segments[$key];
        }

        throw new MissingSegmentException(sprintf('Routing segment %s does not exist', $key));
    }

    /**
     * Return all segments.
     *
     * @return array
     */
    public function getSegments() {
        return $this->_segments;
    }

    /**
     * Group multiple route mappings into a single collection and apply options to all of them.
     * Can apply path prefixes, suffixes, route config, before filters, and after filters.
     *
     * @param array $options
     * @param \Closure $callback
     * @return $this
     */
    public function group(array $options, Closure $callback) {
        $this->_groups[] = $options + [
            'prefix' => null,
            'suffix' => null,
            'secure' => null,
            'patterns' => [],
            'pass' => [],
            'filters' => []
        ];

        call_user_func($callback, $this);

        array_pop($this->_groups);

        return $this;
    }

    /**
     * Add a custom defined route object that matches to an internal destination.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return $this
     */
    public function map($key, Route $route) {
        $this->_routes[$key] = $route;

        // Apply group options
        foreach ($this->_groups as $group) {
            if ($group['prefix'] !== null) {
                $route->prepend($group['prefix']);
            }

            if ($group['suffix'] !== null) {
                $route->append($group['suffix']);
            }

            if ($group['secure'] !== null) {
                $route->setConfig('secure', $group['secure']);
            }

            if ($group['patterns']) {
                $route->setConfig('patterns', array_merge($route->getPatterns(), (array) $group['patterns']));
            }

            if ($group['pass']) {
                $route->setConfig('pass', array_merge((array) $group['pass'], $route->getPassed()));
            }

            if ($group['filters']) {
                $route->setConfig('filters', array_merge($route->getFilters(), (array) $group['filters']));
            }
        }

        // Compile the regex pattern
        $route->compile();

        return $this;
    }

    /**
     * Attempt to match an internal route.
     *
     * @param string $url
     * @return \Titon\Route\Route
     * @throws \Titon\Route\Exception\NoMatchException
     */
    public function match($url) {
        $this->emit('route.preMatch', [$this, $url]);

        $match = $this->getMatcher()->match($url, $this->all());

        if (!$match) {
            throw new NoMatchException(sprintf('No route has been matched for %s', $url));
        }

        // Trigger filters
        foreach ($match->getFilters() as $filter) {
            call_user_func_array($this->getFilter($filter), [$this, $match]);
        }

        $this->emit('route.postMatch', [$this, $match]);

        return $match;
    }

    /**
     * Parse a URL and apply default routes. Attempt to deconstruct @ URLs.
     *
     * @param string|array $url
     * @return string|array
     * @throws \Titon\Route\Exception\InvalidRouteException
     */
    public static function parse($url) {
        if (is_string($url)) {
            if (preg_match('/^(?:(\w+)\\\)?(\w+)(?:\@(\w+)(?:\.(\w+))?)?$/i', $url, $matches)) {
                $url = array_map(function($value) {
                    return Inflector::underscore($value);
                }, [
                    'module' => isset($matches[1]) ? $matches[1] : null,
                    'controller' => $matches[2],
                    'action' => isset($matches[3]) ? $matches[3] : 'index',
                    'ext' => isset($matches[4]) ? $matches[4] : null
                ]);
            } else {
                throw new InvalidRouteException(sprintf('Invalid @ routing format for %s', $url));
            }
        }

        return static::defaults($url);
    }

    /**
     * Map a route that only responds to a POST request.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return $this
     */
    public function post($key, Route $route) {
        return $this->map($key, $route->setConfig('method', 'post'));
    }

    /**
     * Map a route that only responds to a DELETE request.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return $this
     */
    public function put($key, Route $route) {
        return $this->map($key, $route->setConfig('method', 'put'));
    }

    /**
     * Map multiple routes that point to REST styled resources.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @param array $map
     * @return $this
     */
    public function resource($key, Route $route, array $map = []) {
        $map = $map + $this->getConfig('resourceMap');
        $class = get_class($route);
        $path = $route->getPath();
        $config = $route->allConfig();
        $params = $route->getParams();
        $methods = [
            'list' => 'get',
            'create' => 'post',
            'read' => 'get',
            'update' => ['put', 'post'],
            'delete' => ['delete', 'post']
        ];

        foreach ($map as $type => $action) {
            $newPath = $path;
            $params['action'] = $action;
            $config['method'] = $methods[$type];

            if (in_array($type, ['read', 'update', 'delete'])) {
                $newPath .= '/(id)';
                $config['pass'] = ['id'];
            }

            $this->map($key . '.' . $type, new $class($newPath, $params, $config));
        }

        return $this;
    }

    /**
     * Set the matcher.
     *
     * @param \Titon\Route\Matcher $matcher
     * @return $this
     */
    public function setMatcher(Matcher $matcher) {
        $this->_matcher = $matcher;

        return $this;
    }

    /**
     * Return the current URL.
     *
     * @return string
     */
    public function url() {
        $segments = $this->getSegments();
        $base = $this->base();

        $url = $segments['scheme'] . '://' . $segments['host'];

        if ($base !== '/') {
            $url .= $base;
        }

        $url .= $segments['path'];

        if (!empty($segments['query'])) {
            $url .= '?' . http_build_query($segments['query'], '', '&', PHP_QUERY_RFC1738);
        }

        return $url;
    }

}
