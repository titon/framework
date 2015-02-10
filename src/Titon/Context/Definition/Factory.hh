<?hh // strict
/**
 * @author Alex Phillips <exonintrendo@gmail.com>
 * Date: 2/9/15
 * Time: 5:08 PM
 */

namespace Titon\Context\Definition;

use Closure;
use Titon\Context\Depository;

class Factory
{
    public function create(string $alias, mixed $concrete, Depository $depository)
    {
        if ($concrete instanceof Closure) {
            return new CallableDefinition($alias, $concrete, $depository);
        }

        if (is_string($concrete) && class_exists($concrete)) {
            return new ClassDefinition($alias, $concrete, $depository);
        }

        return $concrete;
    }
}