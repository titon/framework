<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route\Annotation;

use Titon\Annotation\Annotation;
use Titon\Route\Mixin\FilterMixin;
use Titon\Route\Mixin\MethodMixin;
use Titon\Route\Mixin\PatternMixin;
use Titon\Utility\Col;

/**
 * The Route annotation provides a low-level way of mapping paths, methods, filters, and patterns
 * to a method or class that can be mapped through `Titon\Route\Router::wire()`.
 *
 * <<Route($key, $path[, $methods[, $filters[, $patterns]])>>
 *
 * @package Titon\Route\Annotation
 */
class Route extends Annotation {
    use FilterMixin, MethodMixin, PatternMixin;

    /**
     * The unique key.
     *
     * @var string
     */
    protected string $key;

    /**
     * The path to match.
     *
     * @var string
     */
    protected string $path;

    /**
     * Store all route settings.
     *
     * @param string $key
     * @param string $path
     * @param mixed  $methods
     * @param mixed  $filters
     * @param mixed  $patterns
     */
    public function __construct(string $key, string $path, mixed $methods = [], mixed $filters = [], mixed $patterns = []) {
        $this->key = $key;
        $this->path = $path;
        $this->setMethods(Col::toVector($methods));
        $this->setFilters(Col::toVector($filters));
        $this->setPatterns(Col::toMap($patterns));
    }

    /**
     * Return the key.
     *
     * @return string
     */
    public function getKey(): string {
        return $this->key;
    }

    /**
     * Return the path.
     *
     * @return string
     */
    public function getPath(): string {
        return $this->path;
    }

}
