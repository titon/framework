<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common\Mixin;

use Titon\Common\Bag\ConfigBag;
use Titon\Utility\Hash;

/**
 * Provides a configuration layer within classes.
 *
 * @package Titon\Common\Mixin
 */
trait Configurable {

    /**
     * The configuration object.
     *
     * @type \Titon\Common\Bag\ConfigBag
     */
    private $__config;

    /**
     * Add multiple configurations.
     *
     * @param array $data
     * @return $this
     */
    public function addConfig(array $data) {
        $this->getConfigBag()->add($data);

        return $this;
    }

    /**
     * Return all current configurations.
     *
     * @return array
     */
    public function allConfig() {
        return $this->getConfigBag()->all();
    }

    /**
     * Merge the custom configuration with the defaults and inherit from parent classes.
     *
     * @uses Titon\Utility\Hash
     *
     * @param array $config
     * @return $this
     */
    public function applyConfig(array $config = []) {
        $parent = $this;
        $defaults = isset($this->_config) ? $this->_config : [];

        // Inherit config from parents
        while ($parent = get_parent_class($parent)) {
            $props = get_class_vars($parent);

            if (isset($props['_config'])) {
                $defaults = Hash::merge($props['_config'], $defaults);
            }
        }

        $this->__config = new ConfigBag($config, $defaults);

        return $this;
    }

    /**
     * Get a configuration by key.
     *
     * @param string $key
     * @param mixed $default
     * @return mixed
     */
    public function getConfig($key, $default = null) {
        return $this->getConfigBag()->get($key, $default);
    }

    /**
     * Return the configuration bag.
     *
     * @return \Titon\Common\Bag\ConfigBag
     */
    public function getConfigBag() {
        return $this->__config;
    }

    /**
     * Check if a configuration exists.
     *
     * @param string $key
     * @return bool
     */
    public function hasConfig($key) {
        return $this->getConfigBag()->has($key);
    }

    /**
     * Remove a configuration by key.
     *
     * @param string $key
     * @return $this
     */
    public function removeConfig($key) {
        $this->getConfigBag()->remove($key);

        return $this;
    }

    /**
     * Set a configuration value.
     *
     * @param string $key
     * @param mixed $value
     * @return $this
     */
    public function setConfig($key, $value) {
        $this->getConfigBag()->set($key, $value);

        return $this;
    }

}