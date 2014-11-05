<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

use Titon\Cache\Storage;
use Titon\Common\FactoryAware;
use Titon\Event\Emittable;
use Titon\Route\Exception\InvalidRouteException;
use Titon\Route\Exception\MissingFilterException;
use Titon\Route\Exception\MissingSegmentException;
use Titon\Route\Exception\MissingRouteException;
use Titon\Route\Exception\NoMatchException;
use Titon\Route\Matcher\LoopMatcher;
use Titon\Utility\Registry;
use Titon\Utility\State\Get;
use Titon\Utility\State\Server;

type Action = shape('class' => string, 'action' => string);
type FilterCallback = (function(Router, Route): void);
type FilterMap = Map<string, FilterCallback>;
type GroupCallback = (function(Router): void);
type GroupList = Vector<Map<string, mixed>>;
type QueryMap = Map<string, mixed>;
type ResourceMap = Map<string, string>;
type RouteMap = Map<string, Route>;
type SegmentMap = Map<string, mixed>;

/**
 * The Router is tasked with the management of routes and filters, at which some point a route is matched against
 * a URL based on the current environment settings.
 *
 * @package Titon\Route
 * @events
 *      route.matching(Router $router, $url)
 *      route.matched(Router $router, Route $route)
 */
class Router {
    use Emittable, FactoryAware;

    /**
     * Base folder structure if the application was placed within a directory.
     *
     * @type string
     */
    protected string $_base = '/';

    /**
     * Have routes been loaded in from the cache?
     *
     * @type bool
     */
    protected bool $_cached = false;

    /**
     * The matched route object.
     *
     * @type \Titon\Route\Route
     */
    protected ?Route $_current;

    /**
     * List of filters to trigger for specific routes during a match.
     *
     * @type \Titon\Route\FilterMap
     */
    protected FilterMap $_filters = Map {};

    /**
     * List of currently open groups (and their options) in the stack.
     *
     * @type \Titon\Route\GroupList
     */
    protected GroupList $_groups = Vector {};

    /**
     * The class to use for route matching.
     *
     * @type \Titon\Route\Matcher
     */
    protected Matcher $_matcher;

    /**
     * Mapping of CRUD actions to URL path parts for REST resources.
     * These mappings will be used when creating resource() routes.
     *
     * @type \Titon\Route\ResourceMap {
     *      @type string $list      - GET /resource - List resources
     *      @type string $create    - POST /resource - Create resource
     *      @type string $read      - GET /resource/{id} - Read resource
     *      @type string $update    - PUT /resource/{id} - Update resource
     *      @type string $delete    - DELETE /resource/{id} - Delete resource
     * }
     */
    protected ResourceMap $_resourceMap = Map {
        'list' => 'index',
        'create' => 'create',
        'read' => 'read',
        'update' => 'update',
        'delete' => 'delete'
    };

    /**
     * Manually defined aesthetic routes that re-route internally.
     *
     * @type \Titon\Route\RouteMap
     */
    protected RouteMap $_routes = Map {};

    /**
     * The current URL broken up into multiple segments: protocol, host, route, query, base
     *
     * @type \Titon\Route\SegmentMap
     */
    protected SegmentMap $_segments = Map {};

    /**
     * Storage engine instance.
     *
     * @type \Titon\Cache\Storage
     */
    protected ?Storage $_storage;

    /**
     * Parses the current environment settings into multiple segments and properties.
     */
    public function __construct() {
        $this->_matcher = new LoopMatcher();

        // Determine if app is within a base folder
        $base = dirname(str_replace(Server::get('DOCUMENT_ROOT'), '', Server::get('SCRIPT_FILENAME')));

        if ($base && $base !== '.') {
            $this->_base = rtrim(str_replace('\\', '/', $base), '/') ?: '/';
        }

        // Store the current URL and query as router segments
        $this->_segments = (new Map(parse_url(Server::get('REQUEST_URI'))))->setAll(Map {
            'scheme' => (Server::get('HTTPS') === 'on') ? 'https' : 'http',
            'query' => Get::all(),
            'host' => Server::get('HTTP_HOST'),
            'port' => Server::get('SERVER_PORT')
        });

        // Set caching events
        $this->on('route.matching', inst_meth($this, 'loadRoutes'), 1);
        $this->on('route.matched', inst_meth($this, 'cacheRoutes'), 1);
    }

    /**
     * Attempts to match a route based on the current path segment.
     */
    public function initialize(): void {
        $this->match((string) $this->getSegment('path'));
    }

    /**
     * Return the base URL if the app was not placed in the root directory.
     *
     * @return string
     */
    public function base(): string {
        return $this->_base;
    }

    /**
     * Return an action shape into its combined @ formatted representation.
     *
     * @param \Titon\Route\Action $action
     * @return string
     */
    public static function buildAction(Action $action): string {
        return sprintf('%s@%s', $action['class'], $action['action']);
    }

    /**
     * Cache the currently mapped routes.
     * This method is automatically called during the `matched` event.
     *
     * @return $this
     */
    public function cacheRoutes(): this {
        if ($this->isCached()) {
            return $this;
        }

        if (($storage = $this->getStorage()) && ($routes = $this->getRoutes())) {
            // Before caching, make sure all routes are compiled
            foreach ($routes as $route) {
                $route->compile();
            }

            // Compiling before hand should speed up the next request
            $storage->set('routes', serialize($routes), '+1 year');
        }

        return $this;
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
     * Map a route that only responds to a DELETE request.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return \Titon\Route\Route
     */
    public function delete(string $key, Route $route): Route {
        return $this->http($key, Vector {'delete'}, $route);
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
     * @param \Titon\Route\FilterCallback $callback
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
     * @return \Titon\Route\Route
     */
    public function get(string $key, Route $route): Route {
        return $this->http($key, Vector {'get'}, $route);
    }

    /**
     * Return a filter by key.
     *
     * @param string $key
     * @return \Titon\Route\FilterCallback
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
     * @return \Titon\Route\FilterMap
     */
    public function getFilters(): FilterMap {
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
     * Return the CRUD action resource map.
     *
     * @return \Titon\Route\ResourceMap
     */
    public function getResourceMap(): ResourceMap {
        return $this->_resourceMap;
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
     * @return \Titon\Route\RouteMap
     */
    public function getRoutes(): RouteMap {
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
     * @return \Titon\Route\SegmentMap
     */
    public function getSegments(): SegmentMap {
        return $this->_segments;
    }

    /**
     * Get the storage engine.
     *
     * @return \Titon\Cache\Storage
     */
    public function getStorage(): ?Storage {
        return $this->_storage;
    }

    /**
     * Group multiple route mappings into a single collection and apply options to all of them.
     * Can apply path prefixes, suffixes, patterns, filters, methods, conditions, and more.
     *
     * @param Map<string, mixed> $options
     * @param \Titon\Route\GroupCallback $callback
     * @return $this
     */
    public function group(Map<string, mixed> $options, GroupCallback $callback): this {
        $this->_groups[] = (Map {
            'prefix' => '',
            'suffix' => '',
            'secure' => false,
            'patterns' => Map {},
            'filters' => Vector {},
            'methods' => Vector {},
            'conditions' => Vector {}
        })->setAll($options);

        call_user_func($callback, $this);

        $this->_groups->pop();

        return $this;
    }

    /**
     * Map a route that only responds to a HEAD request.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return \Titon\Route\Route
     */
    public function head(string $key, Route $route): Route {
        return $this->http($key, Vector {'head'}, $route);
    }

    /**
     * Map a route that only responds to a defined list of HTTP methods.
     *
     * @param string $key
     * @param Vector<string> $methods
     * @param \Titon\Route\Route $route
     * @return \Titon\Route\Route
     */
    public function http(string $key, Vector<string> $methods, Route $route): Route {
        return $this->map($key, $route->setMethods($methods));
    }

    /**
     * Return true if routes have been loaded from a cache.
     *
     * @return bool
     */
    public function isCached(): bool {
        return $this->_cached;
    }

    /**
     * Load routes from the cache if they exist.
     * This method is automatically called during the `matching` event.
     *
     * @return $this
     */
    public function loadRoutes(): this {
        if ($storage = $this->getStorage()) {
            if ($routes = unserialize($storage->get('routes'))) {
                $this->_routes = $routes;
                $this->_cached = true;
            }
        }

        return $this;
    }

    /**
     * Add a custom defined route object that matches to an internal destination.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return \Titon\Route\Route
     */
    public function map(string $key, Route $route): Route {
        $this->_routes[$key] = $route;

        // Apply group options
        foreach ($this->_groups as $group) {
            $route->setSecure((bool) $group['secure']);

            if ($group['prefix'] !== '') {
                $route->prepend((string) $group['prefix']);
            }

            if ($group['suffix'] !== '') {
                $route->append((string) $group['suffix']);
            }

            if ($patterns = $group['patterns']) {
                invariant($patterns instanceof Map, 'Group patterns must be a map');

                $route->addPatterns($patterns);
            }

            if ($filters = $group['filters']) {
                invariant($filters instanceof Vector, 'Group filters must be a vector');

                $route->addFilters($filters);
            }

            if ($methods = $group['methods']) {
                invariant($methods instanceof Vector, 'Group methods must be a vector');

                $route->addMethods($methods);
            }

            if ($conditions = $group['conditions']) {
                invariant($conditions instanceof Vector, 'Group conditions must be a vector');

                $route->addConditions($conditions);
            }
        }

        return $route;
    }

    /**
     * Attempt to match an internal route.
     *
     * @param string $url
     * @return \Titon\Route\Route
     * @throws \Titon\Route\Exception\NoMatchException
     */
    public function match(string $url): Route {
        $this->emit('route.matching', [$this, $url]);

        // Match a route
        $match = $this->getMatcher()->match($url, $this->getRoutes());

        if (!$match) {
            throw new NoMatchException(sprintf('No route has been matched for %s', $url));
        }

        $this->_current = $match;

        // Trigger filters
        foreach ($match->getFilters() as $filter) {
            call_user_func_array($this->getFilter($filter), [$this, $match]);
        }

        $this->emit('route.matched', [$this, $match]);

        return $match;
    }

    /**
     * Map a route that only responds to an OPTIONS request.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return \Titon\Route\Route
     */
    public function options(string $key, Route $route): Route {
        return $this->http($key, Vector {'options'}, $route);
    }

    /**
     * Parse a URL and apply default routes. Attempt to deconstruct @ URLs.
     *
     * @param string $action
     * @return \Titon\Route\Action
     * @throws \Titon\Route\Exception\InvalidRouteException
     */
    public static function parseAction(string $action): Action {
        $matches = [];

        if (preg_match('/^([\w\\\]+)\@(\w+)?$/i', $action, $matches)) {
            return shape(
                'class' => $matches[1],
                'action' => $matches[2]
            );
        }

        throw new InvalidRouteException(sprintf('Invalid @ action routing format for %s', $action));
    }

    /**
     * Map a route that only responds to a POST request.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return \Titon\Route\Route
     */
    public function post(string $key, Route $route): Route {
        return $this->http($key, Vector {'post'}, $route);
    }

    /**
     * Map a route that only responds to a DELETE request.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return \Titon\Route\Route
     */
    public function put(string $key, Route $route): Route {
        return $this->http($key, Vector {'put'}, $route);
    }

    /**
     * Map multiple routes that point to REST styled resources.
     *
     * @param string $key
     * @param \Titon\Route\Route $route
     * @return $this
     */
    public function resource(string $key, Route $route): this {
        $action = $route->getAction();
        $actionMap = $this->getResourceMap();
        $path = $route->getPath();
        $class = get_class($route);
        $resources = Map {
            'list' => Vector {'get'},
            'create' => Vector {'post'},
            'read' => Vector {'get'},
            'update' => Vector {'put', 'post'},
            'delete' => Vector {'delete', 'post'}
        };

        foreach ($resources as $resource => $methods) {

            // Set the action to trigger
            $newAction = $action;
            $newAction['action'] = $actionMap[$resource];

            // Build the new URL path
            $newPath = rtrim($path, '/');

            if (in_array($resource, Vector {'read', 'update', 'delete'})) {
                $newPath .= '/{id}';
            }

            /** @type \Titon\Route\Route $newRoute */
            $newRoute = Registry::factory($class, Vector {$newPath, static::buildAction($newAction)}, false);
            $newRoute->setStatic($route->getStatic());
            $newRoute->setSecure($route->getSecure());
            $newRoute->setFilters($route->getFilters());
            $newRoute->setPatterns($route->getPatterns());
            $newRoute->setMethods($methods);

            $this->map($key . '.' . $resource, $newRoute);
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
     * Update the resource mapping.
     *
     * @param \Titon\Route\ResourceMap $map
     * @return $this
     */
    public function setResourceMap(ResourceMap $map): this {
        $this->_resourceMap->setAll($map);

        return $this;
    }

    /**
     * Set the storage engine.
     *
     * @param \Titon\Cache\Storage $storage
     * @return $this
     */
    public function setStorage(Storage $storage): this {
        $this->_storage = $storage;

        return $this;
    }

}
