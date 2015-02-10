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
 * Defines type aliases that are used by the route package.
 */

namespace Titon\Route {
    use Titon\Route\Group as RouteGroup;

    type Action = shape('class' => string, 'action' => string);
    type FilterCallback = (function(Router, Route): void);
    type FilterMap = Map<string, FilterCallback>;
    type GroupCallback = (function(Router, RouteGroup): void);
    type GroupList = Vector<RouteGroup>;
    type ParamMap = Map<string, mixed>;
    type QueryMap = Map<string, mixed>;
    type ResourceMap = Map<string, string>;
    type RouteMap = Map<string, Route>;
    type SegmentMap = Map<string, mixed>;
    type Token = shape('token' => string, 'optional' => bool);
    type TokenList = Vector<Token>;
}

namespace Titon\Route\Mixin {
    use Titon\Route\Route;

    type ConditionCallback = (function(Route): bool);
    type ConditionList = Vector<ConditionCallback>;
    type FilterList = Vector<string>;
    type MethodList = Vector<string>;
    type PatternMap = Map<string, string>;
}

/**
 * --------------------------------------------------------------
 *  Annotations
 * --------------------------------------------------------------
 *
 * Registers all annotations declared in the route packages.
 */

namespace {
    use Titon\Annotation\Registry;

    if (class_exists('Titon\Annotation\Annotation')) {
        Registry::map('Route', 'Titon\Route\Annotation\Route');
    }
}

/**
 * --------------------------------------------------------------
 *  Helper Functions
 * --------------------------------------------------------------
 *
 * Defines global helper functions for common use cases.
 */

namespace {
    use Titon\Route\ParamMap;
    use Titon\Route\QueryMap;
    use Titon\Route\Router;
    use Titon\Route\UrlBuilder;

    /**
     * @see Titon\Route\UrlBuilder::build()
     */
    function link_to(string $key, ParamMap $params = Map {}, QueryMap $query = Map {}): string {
        $router = Router::registry();

        invariant($router instanceof Router, 'Must be a Router');

        return UrlBuilder::registry($router)->build($key, $params, $query);
    }

    /**
     * @see Titon\Route\UrlBuilder::url()
     */
    function url(): string {
        $router = Router::registry();

        invariant($router instanceof Router, 'Must be a Router');

        return UrlBuilder::registry($router)->url();
    }
}
