<?hh // strict
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
use Titon\Utility\Col;
use Titon\Utility\Str;
use \Closure;

newtype FilterCallback = (function(Router, Route): void);
newtype RouteGroup = (function(Router): void);

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
    protected static string $_base = '/';

    /**
     * The matched route object.
     *
     * @type \Titon\Route\Route
     */
    protected ?Route $_current;

    /**
     * Default configuration.
     *
     * @type Map<string, mixed>
     */
    protected Map<string, mixed> $_config = Map {
        'defaults' => Map {
            'module' => 'main',
            'controller' => 'index',
            'action' => 'index'
        },
        'prefixes' => Vector {'locale', 'module'},
        'resourceMap' => Map {
            'list' => 'index',
            'create' => 'create',
            'read' => 'read',
            'update' => 'update',
            'delete' => 'delete'
        }
    };

    /**
     * List of filters to trigger for specific routes during a match.
     *
     * @type Map<string, FilterCallback>
     */
    protected Map<string, FilterCallback> $_filters = Map {};

    /**
     * List of currently open groups (and their options) in the stack.
     *
     * @type Vector<Map<string, mixed>>
     */
    protected Vector<Map<string, mixed>> $_groups = Vector {};

    /**
     * The class to use for route matching.
     *
     * @type \Titon\Route\Matcher
     */
    protected Matcher $_matcher;

    /**
     * Manually defined aesthetic routes that re-route internally.
     *
     * @type Map<string, Route>
     */
    protected Map<string, Route> $_routes = Map {};

    /**
     * The current URL broken up into multiple segments: protocol, host, route, query, base
     *
     * @type Map<string, mixed>
     */
    protected Map<string, mixed> $_segments = Map {};

    /**
     * Parses the current URL into multiple segments.
     *
     * @param Map<string, mixed> $config
     */
    public function __construct(Map<string, mixed> $config = Map {}) {
        $this->setMatcher(new LoopMatcher());

        $this->__initConfigurable($config);
        $this->__initRouter();
    }

    /**
     * Maps the default routing objects and attempts to match.
     */
    public function initialize(): void {
        $this->_current = $this->match((string) $this->getSegment('path'));
    }

    /**
     * Return all routes.
     *
     * @return Map<string, Route>
     */
    public function all(): Map<string, Route> {
        return $this->getRoutes();
    }

    /**
     * Return the base URL if the app was not placed in the root directory.
     *
     * @return string
     */
    public static function base(): string {
        return static::$_base;
    }

    /**
     * Takes a route in array format and processes it down into a single string that represents an internal URL path.
     *
     * @param string|array|Map $url
     * @return string
     * @throws \Titon\Route\Exception\InvalidRouteException
     */
    public function build(mixed $url = []): string {
        $self = $this;

        return $this->cache([__METHOD__, $url], function() use ($url, $self) {
            $routes = $self->all();

            // Convert strings to array structure
            if (is_string($url)) {
                // Route by key syntax
                if ($routes->contains($url)) {
                    $url = ['route' => $url];

                // Convert @ route to array
                } else if (strpos($url, '@') !== false) {
                    $url = $self->parse($url);

                // Literal URL, return immediately
                } else {
                    return $url;
                }
            }

            // Convert to map
            if ($url instanceof Traversable) {
                $url = new Map($url);
            }

            invariant($url instanceof Map, 'URL must be a map');

            // Convert @ routes first as the logic should be different than route keys
            if ($url->contains('route') && strpos($url['route'], '@') !== false) {
                $url = Col::merge($self->parse($url['route']), $url);
                $url->remove('rout');
            }

            // Build it!
            $defaults = $self->getConfig('defaults');
            $url = $self->defaults($url, $defaults);
            $base = $self->base();

            if ($url == $self->defaults(Map {}, $defaults)) {
                return $base;
            }

            $path = Vector {};
            $args = Vector {};
            $query = Map {};
            $ext = '';
            $fragment = '';

            // Extract all the values from the URL array
            foreach ($url as $key => $value) {
                if ($key === 'args') {
                    $args->addAll(new Vector($value));

                } else if ($key === 'query') {
                    $query->setAll(new Map($value));

                } else if ($key === '#') {
                    $fragment = $value;

                } else if (is_numeric($key)) {
                    $args[] = $value;
                }
            }

            if ($base !== '/' && $base !== '') {
                $path[] = trim($base, '/');
            }

            // Reverse routing through the key
            if ($url->contains('route')) {
                if (!$routes->contains($url['route'])) {
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
                    if ($url->contains($tokenKey)) {
                        $routePath = Str::insert($routePath, Map {$tokenGroup => $url[$tokenKey]}, Map {'escape' => false});

                    } else if ($token['optional']) {
                        $routePath = str_replace(sprintf('/{%s}', $tokenGroup), '', $routePath);

                    } else {
                        throw new InvalidRouteException(sprintf('Missing %s token for %s route', $tokenKey, $url['route']));
                    }
                }

                $path[] = $routePath;

            // Module, controller, action
            } else {
                foreach ($self->getConfig('prefixes') as $prefix) {
                    if ($url->contains($prefix)) {
                        $path[] = $url[$prefix];
                    }
                }

                // Hide controller and action if they are both set to index
                // But only if there are no arguments or extension
                $hasArgs = (bool) ($args);
                $hasExt = (bool) ($url['ext']);

                if ($url['controller'] !== 'index' || $url['action'] !== 'index' || $hasArgs || $hasExt) {
                    $path[] = $url['controller'];

                    if ($url['action'] !== 'index' || $hasArgs || $hasExt) {
                        $path[] = $url['action'];
                    }
                }

                if ($url->contains('ext')) {
                    $ext = $url['ext'];
                }
            }

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
                if ($fragment instanceof Traversable) {
                    $path .= '#' . http_build_query((array) $fragment, '', '&', PHP_QUERY_RFC1738);
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
    public function current(): ?Route {
        return $this->_current;
    }

    /**
     * Maps the default routes and determines the controller and module.
     * Can be merged with a dynamic route to map missing segments.
     *
     * @uses Titon\Utility\Inflector
     *
     * @param Map<string, mixed> $data
     * @param Map<string, mixed> $defaults
     * @return Map<string, mixed>
     */
    public static function defaults(Map<string, mixed> $data = Map {}, Map<string, mixed> $defaults = Map {}): Map<string, mixed> {
        $data = Col::merge(Map {
            'module' => 'main',
            'controller' => 'index',
            'action' => 'index',
            'ext' => '',
            'query' => Map {},
            'args' => Vector {}
        }, $defaults, $data);

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
    public function delete(string $key, Route $route): this {
        return $this->map($key, $route->addMethod('delete'));
    }

    /**
     * Map a filter object to be triggered when a route is matched.
     *
     * @param string $key
     * @param \Titon\Route\Filter $callback
     * @return $this
     */
    public function filter(string $key, Filter $callback): this {
        $this->_filters[$key] = inst_meth($callback, 'filter');

        return $this;
    }

    /**
     * Map a filter callback to be triggered when a route is matched.
     *
     * @param string $key
     * @param FilterCallback $callback
     * @return $this
     */
    public function filterCallback(string $key, FilterCallback $callback): this {
        $this->_filters[$key] = $callback;

        return $this;
    }

    /**
     * Map a route that only responds to a GET request.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return $this
     */
    public function get(string $key, Route $route): this {
        return $this->map($key, $route->setMethods(Vector {'get', 'head'}));
    }

    /**
     * Return a filter by key.
     *
     * @param string $key
     * @return FilterCallback
     * @throws \Titon\Route\Exception\MissingFilterException
     */
    public function getFilter(string $key): FilterCallback {
        if ($this->_filters->contains($key)) {
            return $this->_filters[$key];
        }

        throw new MissingFilterException(sprintf('Filter %s does not exist', $key));
    }

    /**
     * Return all filters.
     *
     * @return Map<string, FilterCallback>
     */
    public function getFilters(): Map<string, FilterCallback> {
        return $this->_filters;
    }

    /**
     * Return the matcher object.
     *
     * @return \Titon\Route\Matcher
     */
    public function getMatcher(): Matcher {
        return $this->_matcher;
    }

    /**
     * Return a route by key.
     *
     * @param string $key
     * @return \Titon\Route\Route
     * @throws \Titon\Route\Exception\MissingRouteException
     */
    public function getRoute(string $key): Route {
        if ($this->_routes->contains($key)) {
            return $this->_routes[$key];
        }

        throw new MissingRouteException(sprintf('Route %s does not exist', $key));
    }

    /**
     * Return all routes.
     *
     * @return Map<string, Route>
     */
    public function getRoutes(): Map<string, Route> {
        return $this->_routes;
    }

    /**
     * Return a segment by key.
     *
     * @param string $key
     * @return mixed
     * @throws \Titon\Route\Exception\MissingSegmentException
     */
    public function getSegment(string $key): mixed {
        if ($this->_segments->contains($key)) {
            return $this->_segments[$key];
        }

        throw new MissingSegmentException(sprintf('Routing segment %s does not exist', $key));
    }

    /**
     * Return all segments.
     *
     * @return Map<string, mixed>
     */
    public function getSegments(): Map<string, mixed> {
        return $this->_segments;
    }

    /**
     * Group multiple route mappings into a single collection and apply options to all of them.
     * Can apply path prefixes, suffixes, route config, before filters, and after filters.
     *
     * @param Map<string, mixed> $options
     * @param RouteGroup $callback
     * @return $this
     */
    public function group(Map<string, mixed> $options, RouteGroup $callback) {
        $this->_groups[] = Col::merge(Map {
            'prefix' => '',
            'suffix' => '',
            'secure' => false,
            'patterns' => Map {},
            'pass' => Vector {},
            'filters' => Vector {}
        }, $options);

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
    public function map($key, Route $route): this {
        $this->_routes[$key] = $route;

        // Apply group options
        foreach ($this->_groups as $group) {
            if ($group['prefix'] !== '') {
                $route->prepend($group['prefix']);
            }

            if ($group['suffix'] !== '') {
                $route->append($group['suffix']);
            }

            $route->setSecure($group['secure']);

            if ($group['patterns']) {
                $route->setPatterns($route->getPatterns()->setAll($group['patterns']));
            }

            if ($group['pass']) {
                // todo - Find a better way to apply uniqueness to a vector
                $route->pass(new Vector(array_unique($route->getPassed()->addAll($group['pass'])->toArray())));
            }

            if ($group['filters']) {
                $route->setFilters($route->getFilters()->addAll($group['filters']));
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
    public function match(string $url): Route {
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
     * @return Map<string, mixed>
     * @throws \Titon\Route\Exception\InvalidRouteException
     */
    public static function parse(mixed $url): Map<string, mixed> {
        if (is_string($url)) {
            $matches = [];

            if (preg_match('/^(?:(\w+)\\\)?(\w+)(?:\@(\w+)(?:\.(\w+))?)?$/i', $url, $matches)) {
                $url = array_filter(array_map(function($value) {
                    return Inflector::underscore($value);
                }, [
                    'module' => array_key_exists(1, $matches) ? $matches[1] : '',
                    'controller' => $matches[2],
                    'action' => array_key_exists(3, $matches) ? $matches[3] : '',
                    'ext' => array_key_exists(4, $matches) ? $matches[4] : ''
                ]));
            } else {
                throw new InvalidRouteException(sprintf('Invalid @ routing format for %s', $url));
            }
        }

        if (is_array($url)) {
            $url = new Map($url);
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
    public function post(string $key, Route $route): this {
        return $this->map($key, $route->addMethod('post'));
    }

    /**
     * Map a route that only responds to a DELETE request.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return $this
     */
    public function put(string $key, Route $route): this {
        return $this->map($key, $route->addMethod('put'));
    }

    /**
     * Map multiple routes that point to REST styled resources.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @param Map<string, string> $map
     * @return $this
     */
    public function resource(string $key, Route $route, Map<string, string> $map = Map {}) {
        $map = Col::merge($this->getConfig('resourceMap'), $map);
        $class = get_class($route);
        $path = $route->getPath();
        $params = $route->getParams();
        $methods = Map {
            'list' => Vector {'get'},
            'create' => Vector {'post'},
            'read' => Vector {'get'},
            'update' => Vector {'put', 'post'},
            'delete' => Vector {'delete', 'post'}
        };

        foreach ($map as $type => $action) {
            $newPath = $path;
            $params['action'] = $action;
            $pass = $route->getPassed()->toVector();

            if (in_array($type, Vector {'read', 'update', 'delete'})) {
                $newPath .= '/(id)';
                $pass[] = 'id';
            }

            $newRoute = new $class($newPath, $params);
            $newRoute->setStatic($route->getStatic());
            $newRoute->setSecure($route->getSecure());
            $newRoute->setFilters($route->getFilters());
            $newRoute->setPatterns($route->getPatterns());
            $newRoute->setMethods($methods[$type]);
            $newRoute->pass($pass);

            $this->map($key . '.' . $type, $newRoute);
        }

        return $this;
    }

    /**
     * Set the matcher.
     *
     * @param \Titon\Route\Matcher $matcher
     * @return $this
     */
    public function setMatcher(Matcher $matcher): this {
        $this->_matcher = $matcher;

        return $this;
    }

    /**
     * Return the current URL.
     *
     * @return string
     */
    public function url(): string {
        $segments = $this->getSegments();
        $base = $this->base();

        $url = $segments['scheme'] . '://' . $segments['host'];

        if ($base !== '/') {
            $url .= $base;
        }

        $url .= $segments['path'];

        if ($segments['query']) {
            $url .= '?' . http_build_query($segments['query'], '', '&', PHP_QUERY_RFC1738);
        }

        return $url;
    }

    /**
     * Private initializer method.
     */
    private function __initRouter(): void {
        // Determine if app is within a base folder
        $base = dirname(str_replace($_SERVER['DOCUMENT_ROOT'], '', $_SERVER['SCRIPT_FILENAME']));

        // Store the base to prepend to built routes
        if ($base && $base !== '.') {
            static::$_base = rtrim(str_replace('\\', '/', $base), '/') ?: '/';
        }

        // Store the current URL and query as router segments
        $this->_segments = Col::merge(new Map(parse_url($_SERVER['REQUEST_URI'])), Map {
            'scheme' => (array_key_exists('HTTPS', $_SERVER) && $_SERVER['HTTPS'] === 'on') ? 'https' : 'http',
            'query' => new Map($_GET),
            'host' => $_SERVER['HTTP_HOST'],
            'port' => $_SERVER['SERVER_PORT']
        });
    }

}
