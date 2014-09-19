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
     * The configuration bag that inherits the settings.
     *
     * @type \Titon\Common\Bag\ConfigBag
     */
    protected ?ConfigBag<string, mixed> $_configBag;

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
    public function getConfigBag(): ConfigBag<string, mixed> {
        if (!$this->_configBag) {
            $this->_configBag = $this->__initConfigurable();
        }

        return $this->_configBag;
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

    /**
     * Merge the custom configuration with the defaults and inherit from parent classes.
     *
     * @uses Titon\Utility\Col
     *
     * @param Map<string, mixed> $config
     * @return ConfigBag<string, mixed>
     */
    private function __initConfigurable(Map<string, mixed> $config = Map {}): ConfigBag<string, mixed> {
        $parent = $this;
        $defaults = property_exists($this, '_config') ? $this->_config : Map {};

        // Inherit config from parents
        while ($parent = get_parent_class($parent)) {
            $props = get_class_vars($parent);

            if (array_key_exists('_config', $props)) {
                $defaults = Col::merge($props['_config'], $defaults);
            }
        }

        return $this->_configBag = new ConfigBag($config, $defaults);
    }

}