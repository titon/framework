<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl\Utility;

use Titon\Common\Registry;
use Titon\Intl\Intl;

/**
 * Enhance the parent Number class by providing localized currency and number rule support.
 *
 * @package Titon\Intl\Utility
 */
class Number extends \Titon\Utility\Number {

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Common\Registry
     */
    public static function currency($number, array $options = []) {
        $g11n = G11n::registry();

        if ($g11n->isEnabled()) {
            $options = array_merge(
                $g11n->current()->getFormatPatterns('number'),
                $g11n->current()->getFormatPatterns('currency'),
                $options
            );
        }

        return parent::currency($number, $options);
    }

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Common\Registry
     */
    public static function percentage($number, $options = []) {
        if (is_numeric($options)) {
            $options = ['places' => $options];
        }

        $g11n = G11n::registry();

        if ($g11n->isEnabled()) {
            $options = array_merge(
                $g11n->current()->getFormatPatterns('number'),
                $options
            );
        }

        return parent::percentage($number, $options);
    }

}
