<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common\Bag;

use Titon\Utility\Col;

/**
 * Bag for interacting with configuration settings.
 *
 * @package Titon\Common\Bag
 */
class ConfigBag<Tk, Tv> extends AbstractBag<Tk, Tv> {

    /**
     * Default configuration.
     *
     * @type Map<Tk, Tv>
     */
    protected Map<Tk, Tv> $_defaults = Map {};

    /**
     * Apply defaults and merge the custom configuration in.
     *
     * @uses Titon\Utility\Col
     *
     * @param Map<Tk, Tv> $config
     * @param Map<Tk, Tv> $defaults
     */
    public function __construct(Map<Tk, Tv> $config = Map {}, Map<Tk, Tv> $defaults = Map {}) {
        parent::__construct(Col::merge($defaults, $config));

        $this->_defaults = $defaults;
    }

    /**
     * Return the default configuration.
     *
     * @return Map<Tk, Tv>
     */
    public function getDefaults(): Map<Tk, Tv> {
        return $this->_defaults;
    }

}