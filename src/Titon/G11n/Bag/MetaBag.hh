<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n\Bag;

use Titon\Common\Bag\AbstractBag;

/**
 * A bag for locale specific meta data.
 *
 * @package Titon\G11n\Bag
 */
class MetaBag extends AbstractBag<string, mixed> {

    /**
     * Return the locale code.
     *
     * @return string
     */
    public function getCode(): string {
        return (string) $this->get('code', '');
    }

    /**
     * Return the ISO2 code.
     *
     * @return string
     */
    public function getISO2Code(): string {
        return (string) $this->get('iso2', '');
    }

    /**
     * Return the ISO3 code.
     *
     * @return string
     */
    public function getISO3Code(): string {
        return (string) $this->get('iso3', '');
    }

    /**
     * Return the timezone.
     *
     * @return string
     */
    public function getTimezone(): string {
        return (string) $this->get('timezone', '');
    }

    /**
     * Return the parent locale code.
     *
     * @return string
     */
    public function getParentCode(): string {
        return (string) $this->get('parent', '');
    }

    /**
     * Return the plural rule.
     *
     * @return string
     */
    public function getPluralRule(): int {
        return (int) $this->get('plural', '');
    }

    /**
     * Return the name of the locale.
     *
     * @return string
     */
    public function getTitle(): string {
        return (string) $this->get('title', '');
    }

}