<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl\Utility;

use Titon\Utility\Col;
use Titon\Utility\OptionMap;

/**
 * Enhances the parent `Number` class by providing localized currency and number rule support.
 *
 * @package Titon\Intl\Utility
 */
class Number extends \Titon\Utility\Number {

    /**
     * {@inheritdoc}
     */
    public static function currency(num $number, OptionMap $options = Map {}): string {
        $patterns = Format::loadFormatPatterns();

        if ($patterns !== null) {
            $options = Col::merge($patterns->getNumber(), $patterns->getCurrency(), $options);
        }

        return parent::currency($number, $options);
    }

    /**
     * {@inheritdoc}
     */
    public static function percentage(num $number, OptionMap $options = Map {}): string {
        $patterns = Format::loadFormatPatterns();

        if ($patterns !== null) {
            $options = Col::merge($patterns->getNumber(), $options);
        }

        return parent::percentage($number, $options);
    }

}
