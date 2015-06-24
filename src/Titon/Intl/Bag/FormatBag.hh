<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl\Bag;

use Titon\Common\Bag\AbstractBag;

/**
 * A bag for locale specific format patterns.
 *
 * @package Titon\Intl\Bag
 */
class FormatBag extends AbstractBag<string, mixed> {

    /**
     * Return the currency format map.
     *
     * @return Map<string, string>
     */
    public function getCurrency(): Map<string, string> {
        $currency = $this->get('currency');

        if ($currency instanceof Map) {
            return $currency;
        }

        return Map {};
    }

    /**
     * Return the date format.
     *
     * @return string
     */
    public function getDate(): string {
        return (string) $this->get('date', '');
    }

    /**
     * Return the datetime format.
     *
     * @return string
     */
    public function getDatetime(): string {
        return (string) $this->get('datetime', '');
    }

    /**
     * Return the number format map.
     *
     * @return Map<string, string>
     */
    public function getNumber(): Map<string, string> {
        $number = $this->get('number');

        if ($number instanceof Map) {
            return $number;
        }

        return Map {};
    }

    /**
     * Return the phone format map.
     *
     * @return Map<int, string>
     */
    public function getPhone(): Map<int, string> {
        $phone = $this->get('phone');

        if ($phone instanceof Map) {
            return $phone;
        }

        return Map {};
    }

    /**
     * Return the SSN format.
     *
     * @return string
     */
    public function getSSN(): string {
        return (string) $this->get('ssn', '');
    }

    /**
     * Return the time format.
     *
     * @return string
     */
    public function getTime(): string {
        return (string) $this->get('time', '');
    }
}
