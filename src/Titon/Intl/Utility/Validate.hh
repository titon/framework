<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl\Utility;

use Titon\Intl\Bag\ValidationBag;

/**
 * Enhances the parent `Validate` class by providing localized validation rule support.
 *
 * @package Titon\Intl\Utility
 */
class Validate extends \Titon\Utility\Validate {

    /**
     * {@inheritdoc}
     */
    public static function currency(string $input, string $format = ''): bool {
        $patterns = static::loadValidationPatterns();

        if ($patterns !== null && !$format) {
            $format = $patterns->getCurrency();
        }

        if ($format) {
            return parent::currency($input, $format);
        }

        return parent::currency($input);
    }

    /**
     * Load and return a `ValidationBag` from the currently detected locale.
     * If no locale is found, or the translator is not enabled, return null.
     *
     * @return \Titon\Intl\Bag\ValidationBag
     */
    public static function loadValidationPatterns(): ?ValidationBag {
        $translator = translator_context();

        if (!$translator->isEnabled()) {
            return null;
        }

        return $translator->current()?->getValidationPatterns();
    }

    /**
     * {@inheritdoc}
     */
    public static function phone(string $input, string $format = ''): bool {
        $patterns = static::loadValidationPatterns();

        if ($patterns !== null && !$format) {
            $format = $patterns->getPhone();
        }

        if ($format) {
            return parent::phone($input, $format);
        }

        return parent::phone($input);
    }

    /**
     * {@inheritdoc}
     */
    public static function postalCode(string $input, string $format = ''): bool {
        $patterns = static::loadValidationPatterns();

        if ($patterns !== null && !$format) {
            $format = $patterns->getPostalCode();
        }

        if ($format) {
            return parent::postalCode($input, $format);
        }

        return parent::postalCode($input);
    }

    /**
     * {@inheritdoc}
     */
    public static function ssn(string $input, string $format = ''): bool {
        $patterns = static::loadValidationPatterns();

        if ($patterns !== null && !$format) {
            $format = $patterns->getSSN();
        }

        if ($format) {
            return parent::ssn($input, $format);
        }

        return parent::ssn($input);
    }

}
