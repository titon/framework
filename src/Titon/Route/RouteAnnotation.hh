<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

use Titon\Common\Annotator;
use Titon\Utility\Converter;
use Titon\Utility\Path;
use \ReflectionClass;
use \ReflectionMethod;

/**
 * The RouteAttribute provides access to the <<Route>> annotation which will auto-wire the Router via the registry.
 * This annotation is usable on both the class (as a resource route) and each method.
 *
 * @package Titon\Route
 */
trait RouteAnnotation {
    require implements Annotator;

    /**
     * Wire up all possible routes by looking in the class and method attributes.
     * The following format is supported. Both the methods and filters can be a string, or an array of strings.
     *
     *      <<Route($path[, $methods[, $filters]])>>
     */
    private function __wireRouteAnnotations(): void {
        $router = Router::registry();
        $class = static::class;
        $key = strtolower(Path::className($class));

        // Map class resource route
        if ($annotation = $this->getClassAnnotation('Route')) {
            $router->resource($key, new Route($annotation[0], $class . '@action'));
        }

        // Map individual method route
        $reflection = new ReflectionClass($this);

        foreach ($reflection->getMethods(ReflectionMethod::IS_PUBLIC) as $method) {
            $name = $method->getName();

            if ($annotation = $this->getMethodAnnotation($name, 'Route')) {
                $route = new Route($annotation[0], $class . '@' . $name);

                if ($annotation->containsKey(1)) {
                    $route->setMethods(Converter::toVector($annotation[1]));
                }

                if ($annotation->containsKey(2)) {
                    $route->setFilters(Converter::toVector($annotation[2]));
                }

                $router->map($key . '.' . strtolower($name), $route);
            }
        }
    }

}