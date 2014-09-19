<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use Titon\Common\Configurable;
use Titon\Common\Reflectable;
use \Serializable;
use \JsonSerializable;

/**
 * Primary class for all framework classes to extend. All child classes will inherit the $_config property,
 * allowing any configuration settings to be automatically passed and set through the constructor.
 *
 * @package Titon\Common
 */
class Base implements Serializable, JsonSerializable {
    use Configurable, Reflectable;

    /**
     * An array of configuration settings for the current class.
     *
     * @type Map<string, mixed>
     */
    protected Map<string, mixed> $_config = Map {'initialize' => true};

    /**
     * Merges the custom configuration with the defaults.
     * Trigger initialize method if setting is true.
     *
     * @param Map<string, mixed> $config
     */
    public function __construct(Map<string, mixed> $config = Map {}) {
        $this->__initConfigurable($config);
        $this->__initBase();
    }

    /**
     * Magic method for toString().
     *
     * @return string
     */
    public function __toString(): string {
        return $this->toString();
    }

    /**
     * Serialize the configuration.
     *
     * @return string
     */
    public function serialize(): string {
        return serialize($this->allConfig());
    }

    /**
     * Reconstruct the class once unserialized.
     *
     * todo - Type hinting here causes fatal errors, dono why.
     * https://github.com/facebook/hhvm/blob/4916e5ad093e71f615e6a6db3d213e1ff62bb0b1/hphp/hack/hhi/interfaces.hhi#L102
     *
     * @param string $serialized
     */
    public function unserialize($serialized): void {
        $this->__initConfigurable(new Map(unserialize($serialized)));
        $this->__initBase();
    }

    /**
     * Return the values for JSON serialization.
     *
     * @return mixed
     */
    public function jsonSerialize(): mixed {
        return $this->allConfig()->toArray();
    }

    /**
     * Primary initialize method that is triggered during instantiation.
     */
    public function initialize(): void {
        return;
    }

    /**
     * A dummy function for no operation.
     */
    public function noop(): void {
        return;
    }

    /**
     * Return the class name when called as a string.
     *
     * @return string
     */
    public function toString(): string {
        return static::class;
    }

    /**
     * Private initializer method.
     */
    private function __initBase(): void {
        if ($this->getConfig('initialize')) {
            $this->initialize();
        }
    }

}