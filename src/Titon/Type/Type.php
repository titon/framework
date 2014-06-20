<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

use \Serializable;

/**
 * Acts as the base for all generic types.
 *
 * @package Titon\Type
 */
class Type implements Serializable {

    /**
     * Raw value.
     *
     * @type mixed
     */
    protected $_value;

    /**
     * Set the value.
     *
     * @param mixed $value
     */
    public function __construct($value = null) {
        $this->write($value);
    }

    /**
     * Define magic to string.
     *
     * @return string
     */
    public function __toString() {
        return $this->toString();
    }

    /**
     * Return the value instead of serializing it.
     *
     * @return string
     */
    public function serialize() {
        return serialize($this->value());
    }

    /**
     * Define basic to string.
     *
     * @return string
     */
    public function toString() {
        return (string) $this->value();
    }

    /**
     * Set the value after unserialization.
     *
     * @param string $value
     */
    public function unserialize($value) {
        $this->__construct(unserialize($value));
    }

    /**
     * Return the current modified value.
     *
     * @return mixed
     */
    public function value() {
        return $this->_value;
    }

    /**
     * Overwrite the current value.
     *
     * @param mixed $value
     * @return $this
     */
    public function write($value) {
        $this->_value = $value;

        return $this;
    }

}