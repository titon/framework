<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl\Utility;

use Titon\Intl\Bag\InflectionBag;

/**
 * Enhances the parent `Inflect` class by providing localized inflection rule support.
 *
 * @package Titon\Intl\Utility
 */
class Inflect extends \Titon\Utility\Inflect {

    /**
     * Load and return an `InflectionBag` from the currently detected locale.
     * If no locale is found, or the translator is not enabled, return null.
     *
     * @return \Titon\Intl\Bag\InflectionBag
     */
    public static function loadInflectionRules(): ?InflectionBag {
        $translator = translator_context();

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

        // Strip any characters
        $number = (int) $number;

        foreach ($suffixes as $pattern => $suffix) {
            if (preg_match($pattern, $number)) {
                return str_replace('#', $number, $suffix);
            }
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
