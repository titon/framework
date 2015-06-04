<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl;

/**
 * The registry manages the instantiation of `Locale` objects to reduce the overhead of resource loading.
 *
 * @package Titon\Intl
 */
class LocaleRegistry {

    /**
     * A mapping of registered locales.
     *
     * @var \Titon\Intl\LocaleMap
     */
    protected static LocaleMap $locales = Map {};

    /**
     * Instantiate or return a `Locale` with the given locale code.
     *
     * @param string $code
     * @return \Titon\Intl\Locale
     */
    public static function factory(string $code): Locale {
        if (static::$locales->contains($code)) {
            return static::$locales[$code];
        }

        return static::set(new Locale($code));
    }

    /**
     * Set a `Locale` manually.
     *
     * @param \Titon\Intl\Locale $locale
     * @return \Titon\Intl\Locale
     */
    public static function set(Locale $locale): Locale {
        static::$locales[$locale->getCode()] = $locale;

        return $locale;
    }

}
