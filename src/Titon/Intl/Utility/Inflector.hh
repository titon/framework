<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl\Utility;

use Titon\Intl\Bag\InflectionBag;

/**
 * Enhances the parent `Inflector` class by providing localized inflection rule support.
 *
 * @package Titon\Intl\Utility
 */
class Inflector extends \Titon\Utility\Inflector {

    /**
     * Load and return an `InflectionBag` from the currently detected locale.
     * If no locale is found, or the translator is not enabled, return null.
     *
     * @return \Titon\Intl\Bag\InflectionBag
     */
    public static function loadInflectionRules(): ?InflectionBag {
        $translator = translator();

        if (!$translator->isEnabled()) {
            return null;
        }

        return $translator->current()?->getInflectionRules();
    }

    /**
     * {@inheritdoc}
     */
    public static function ordinal(string $number): string {
        $suffixes = static::loadInflectionRules()?->getOrdinalSuffixes();

        if ($suffixes === null) {
            return $number;
        }

        $number = (int) $number;

        // 11-13
        $teenNumber = $number % 100;

        if (($teenNumber >= 11 && $teenNumber <= 19) && $suffixes->contains(-1)) {
            return str_replace('#', $number, $suffixes[-1]);
        }

        // 1, 2, 3
        $modNumber = $number % 10;

        foreach ($suffixes as $i => $format) {
            if ($modNumber === $i) {
                return str_replace('#', $number, $suffixes[$i]);
            }
        }

        // Fallback
        if ($suffixes->contains(-1)) {
            return str_replace('#', $number, $suffixes[-1]);
        }

        return (string) $number;
    }

    /**
     * {@inheritdoc}
     */
    public static function pluralize(string $string): string {
        $rules = static::loadInflectionRules();

        if ($rules === null) {
            return $string;
        }

        $string = mb_strtolower($string);
        $uninflected = $rules->getUninflectedWords();
        $irregular = $rules->getIrregularWords();
        $plurals = $rules->getPluralPatterns();

        // Does not change
        if ($uninflected->contains($string)) {
            return $string;

        // Is singular, use the plural
        } else if ($irregular->contains($string)) {
            return $irregular[$string];

        // Is plural already
        } else if ($irregular->toVector()->linearSearch($string) >= 0) {
            return $string;

        } else if ($plurals) {
            foreach ($plurals as $pattern => $replacement) {
                if (preg_match($pattern, $string)) {
                    return preg_replace($pattern, $replacement, $string);
                }
            }
        }

        return $string;
    }

    /**
     * {@inheritdoc}
     */
    public static function singularize(string $string): string {
        $rules = static::loadInflectionRules();

        if ($rules === null) {
            return $string;
        }

        $string = mb_strtolower($string);
        $uninflected = $rules->getUninflectedWords();
        $irregular = new Map(array_flip($rules->getIrregularWords()->toArray()));
        $singulars = $rules->getSingularPatterns();

        // Does not change
        if ($uninflected->contains($string)) {
            return $string;

        // Is plural, use the singular
        } else if ($irregular->contains($string)) {
            return $irregular[$string];

        // Is singular already
        } else if ($irregular->toVector()->linearSearch($string) >= 0) {
            return $string;

        } else if ($singulars) {
            foreach ($singulars as $pattern => $replacement) {
                if (preg_match($pattern, $string)) {
                    return preg_replace($pattern, $replacement, $string);
                }
            }
        }

        return $string;
    }

    /**
     * {@inheritdoc}
     */
    public static function transliterate(string $string): string {
        $transliterations = static::loadInflectionRules()?->getTransliterations();

        if ($transliterations === null) {
            return $string;
        }

        // Replace with ASCII characters
        $string = str_replace($transliterations->keys(), $transliterations->values(), $string);

        // Remove any left over non 7bit ASCII
        return preg_replace('/[^\x09\x0A\x0D\x20-\x7E]/', '', $string);
    }

}
