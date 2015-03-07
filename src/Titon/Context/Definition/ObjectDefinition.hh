<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\Definition;

use Titon\Context\Exception\ClassNotInstantiableException;
use Titon\Context\Depository;

/**
 * The object definition is used in instances where an instantiated object is registered.
 *
 * @package Titon\Context\Definition
 */
class ObjectDefinition extends AbstractDefinition {

    /**
     * The instantiated object.
     *
     * @var object
     */
    protected mixed $object;

    /**
     * {@inheritdoc}
     */
    public function __construct(string $key, mixed $object, Depository $depository) {
        parent::__construct($key, $depository);

        $this->object = $object;
    }

    /**
     * {@inheritdoc}
     */
    public function create<T>(/* HH_FIXME[4033]: variadic + strict */ ...$arguments): T {
        throw new ClassNotInstantiableException('Object is already instantiated, use get() instead');
    }

    /**
     * Return the object directly.
     *
     * @return mixed
     */
    public function get(): mixed {
        return $this->object;
    }

}
