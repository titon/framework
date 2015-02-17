<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\Definition;

use Closure;
use Titon\Context\Depository;
use Titon\Context\Definition;
use Titon\Context\Exception\ItemNotDefinable;

class DefinitionFactory {
    /**
     * Factory method to generate the necessary definition given the passed in
     * parameters
     *
     * @param string     $key           The key (class name or alias) for the
     *                                  definition
     * @param mixed      $concrete      The concrete (class, class name, or callable)
     *                                  for the definition to reference
     * @param Depository $depository    The depository object the definition is
     *                                  contained in
     *
     * @return Definition   The definition object for fluent method chaining
     */
    public static function factory(string $key, mixed $concrete, Depository $depository): Definition {
        if ($concrete instanceof Closure) {
            return new ClosureDefinition($key, $concrete, $depository);
        }

        if (is_string($concrete) && class_exists($concrete)) {
            return new ClassDefinition($key, $concrete, $depository);
        }

        if (is_string($concrete) && strpos($concrete, '::') !== false) {
            $concrete = explode('::', $concrete);
        }

        if (is_callable($concrete)) {
            $class = null;
            $function = $concrete;
            if (is_array($concrete)) {
                list($class, $function) = $concrete;
            }

            return new CallableDefinition($key, $class, $function, $depository);
        }

        throw new ItemNotDefinable("Cannot create a definition from " . print_r($concrete));
    }
}