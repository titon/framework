<?hh
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common\Bag;

use Titon\Utility\Hash;

/**
 * Bag for interacting with configuration settings.
 *
 * @package Titon\Common\Bag
 */
class ConfigBag extends AbstractBag {

    /**
     * Default configuration.
     *
     * @type array
     */
    protected $_defaults = [];

    /**
     * Apply defaults and merge the custom configuration in.
     *
     * @uses Titon\Utility\Hash
     *
     * @param array $config
     * @param array $defaults
     */
    public function __construct(array $config = [], array $defaults = []) {
        parent::__construct(Hash::merge($defaults, $config));

        $this->_defaults = $defaults;
    }

    /**
     * Return the default configuration.
     *
     * @return array
     */
    public function getDefaults() {
        return $this->_defaults;
    }

}