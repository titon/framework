<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use Titon\Common\Bag\ConfigBag;
use Titon\Utility\Col;

/**
 * Provides a configuration layer within classes.
 *
 * @package Titon\Common
 */
trait Configurable {

    /**
     * The configuration object.
     *
     * @type \Titon\Common\Bag\ConfigBag
     */
    private ConfigBag $__config;

    /**
     * Add multiple configurations.
     *
     * @param Map<string, mixed> $data
     * @return $this
     */
    public function addConfig(Map<string, mixed> $data): this {
        $this->getConfigBag()->add($data);

        return $this;
    }

    /**
     * Return all current configurations.
     *
     * @return Map<string, mixed>
     */
    public function allConfig(): Map<string, mixed> {
        return $this->getConfigBag()->all();
    }

    /**
     * Merge the custom configuration with the defaults and inherit from parent classes.
     *
     * @uses Titon\Utility\Col
     *
     * @param Map<string, mixed> $config
     * @return $this
     */
    public function applyConfig(Map<string, mixed> $config = Map {}): this {
        $parent = $this;
        $defaults = isset($this->_config) ? $this->_config : Map {};

        // Inherit config from parents
        while ($parent = get_parent_class($parent)) {
            $props = get_class_vars($parent);

            if (isset($props['_config'])) {
                $defaults = Col::merge($props['_config'], $defaults);
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
    public function getConfig(string $key, mixed $default = null): mixed {
        return $this->getConfigBag()->get($key, $default);
    }

    /**
     * Return the configuration bag.
     *
     * @return \Titon\Common\Bag\ConfigBag
     */
    public function getConfigBag(): ConfigBag {
        return $this->__config;
    }

    /**
     * Check if a configuration exists.
     *
     * @param string $key
     * @return bool
     */
    public function hasConfig(string $key): bool {
        return $this->getConfigBag()->has($key);
    }

    /**
     * Remove a configuration by key.
     *
     * @param string $key
     * @return $this
     */
    public function removeConfig(string $key): this {
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
    public function setConfig(string $key, mixed $value): this {
        $this->getConfigBag()->set($key, $value);

        return $this;
    }

}