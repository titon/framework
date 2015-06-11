<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl\Bag;

use Titon\Common\Bag\AbstractBag;

/**
 * A bag for locale specific validation patterns.
 *
 * @package Titon\Intl\Bag
 */
class ValidationBag extends AbstractBag<string, mixed> {

    /**
     * Return the currency pattern.
     *
     * @return string
     */
    public function getCurrency(): string {
        return (string) $this->get('currency', '');
    }

    /**
     * Return the phone pattern.
     *
     * @return string
     */
    public function getPhone(): string {
        return (string) $this->get('phone', '');
    }

    /**
     * Return the postal code pattern.
     *
     * @return string
     */
    public function getPostalCode(): string {
        return (string) $this->get('postal_code', '');
    }

    /**
     * Return the SSN pattern.
     *
     * @return string
     */
    public function getSSN(): string {
        return (string) $this->get('ssn', '');
    }

}
