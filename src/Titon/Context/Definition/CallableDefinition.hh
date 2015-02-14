<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\Definition;

use ReflectionClass;
use Titon\Context\Depository;

class CallableDefinition extends Definition implements DefinitionInterface
{
    protected $class;
    protected $function;

    public function __construct(string $key, mixed $callable, Depository $depository)
    {
        parent::__construct($key, $depository);

        if (is_array($callable)) {
            list($this->class, $this->function) = $callable;
            if (!method_exists($this->class, $this->function)) {
                // throw exception
            }

        }
        else {
            $this->function = $callable;
            if (!function_exists($this->function)) {
                // throw exception
            }
        }
    }

    public function create(...$arguments)
    {
        if (!is_null($this->class)) {
            $class = $this->depository->make($this->class);
            return call_user_func_array(array($class, $this->function), $arguments);
        }

        return call_user_func_array($this->function, $arguments);
    }
}