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
    type RouteCallback = (function(...): mixed);
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
    use Titon\Context\Depository;

    /**
     * @see Titon\Route\UrlBuilder::build()
     */
    function link_to(string $key, ParamMap $params = Map {}, QueryMap $query = Map {}): string {
        // UNSAFE
        // Since we're not type checking the response from the container.
        return Depository::getInstance()
            ->make('Titon\Route\UrlBuilder')
            ->build($key, $params, $query);
    }

    /**
     * @see Titon\Route\UrlBuilder::url()
     */
    function url(): string {
        // UNSAFE
        // Since we're not type checking the response from the container.
        return Depository::getInstance()
            ->make('Titon\Route\UrlBuilder')
            ->url();
    }
}
