<?hh //strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

/**
 * The Collection type can be used to manage a list of objects and apply class level functionality to them.
 * Since Collection inherits from Map, all inherited methods can be used.
 *
 * @package Titon\Type
 */
class Collection extends Map {

    /**
     * Trigger the defined method on all objects within the collection. Optionally pass an array of arguments.
     * If break is true, exit the loop if any of the objects returns a falsey value.
     *
     * @param string $method
     * @param array $args
     * @return $this
     */
    public function trigger($method, array $args = []) {
        foreach ($this->value() as $i => $object) {
            if (is_object($object) && method_exists($object, $method)) {
                $this->_value[$i] = call_user_func_array([$object, $method], $args);
            }
        }

        return $this;
    }

}