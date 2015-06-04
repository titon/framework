<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl\Utility;

use Titon\Common\Registry;
use Titon\Intl\Intl;
use Titon\Intl\Exception\MissingPatternException;

/**
 * Enhance the parent Validate class by providing localized validation rule support.
 *
 * @package Titon\Intl\Utility
 */
class Validate extends \Titon\Utility\Validate {

    /**
     * {@inheritdoc}
     */
    public static function currency($input, $format = null) {
        return parent::currency($input, self::get('currency', $format));
    }

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Common\Registry
     *
     * @throws \Titon\Intl\Exception\MissingPatternException
     */
    public static function get($key, $fallback = null) {
        $pattern = G11n::registry()->current()->getValidationRules($key) ?: $fallback;

        if (!$pattern) {
            throw new MissingPatternException(sprintf('Validation rule %s does not exist', $key));
        }

        return $pattern;
    }

    /**
     * {@inheritdoc}
     */
    public static function phone($input, $format = null) {
        return parent::phone($input, self::get('phone', $format));
    }

    /**
     * {@inheritdoc}
     */
    public static function postalCode($input, $format = null) {
        return parent::postalCode($input, self::get('postalCode', $format));
    }

    /**
     * {@inheritdoc}
     */
    public static function ssn($input, $format = null) {
        return parent::ssn($input, self::get('ssn', $format));
    }

}
