<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common\Bag;

use Titon\Utility\Traverse;

/**
 * Bag for interacting with configuration settings.
 *
 * @package Titon\Common\Bag
 */
class ConfigBag extends AbstractBag {

    /**
     * Default configuration.
     *
     * @type Map<string, mixed>
     */
    protected Map<string, mixed> $_defaults = Map {};

    /**
     * Apply defaults and merge the custom configuration in.
     *
     * @uses Titon\Utility\Traverse
     *
     * @param Map<string, mixed> $config
     * @param Map<string, mixed> $defaults
     */
    public function __construct(Map<string, mixed> $config = Map {}, Map<string, mixed> $defaults = Map {}) {
        parent::__construct(Traverse::merge($defaults, $config));

        $this->_defaults = $defaults;
    }

    /**
     * Return the default configuration.
     *
     * @return Map<string, mixed>
     */
    public function getDefaults(): Map<string, mixed> {
        return $this->_defaults;
    }

}