<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl\Utility;

/**
 * Enhance the parent Inflector class by providing localized inflection rule support.
 *
 * @package Titon\Intl\Utility
 */
class Inflector extends \Titon\Utility\Inflector {

    /**
     * {@inheritdoc}
     */
    public static function ordinal(string $number): string {
        $translator = translator();

        if (!$translator->isEnabled()) {
            return $number;
        }

        $suffixes = $translator->current()?->getInflectionRules()?->getOrdinalSuffixes();

        if ($suffixes === null) {
            return $number;
        }

        $number = (int) $number;

        // 11-13
        if (in_array(($number % 100), range(11, 13)) && $suffixes->contains(-1)) {
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
        $translator = translator();

        if (!$translator->isEnabled()) {
            return $string;
        }

        $rules = $translator->current()?->getInflectionRules();

        if ($rules === null) {
            return $string;
        }

        $string = mb_strtolower($string);
        $uninflected = $rules->getUninflectedWords();
        $irregular = $rules->getIrregularWords();
        $plurals = $rules->getPluralPatterns();

        if ($uninflected->contains($string)) {
            return $string;

        } else if ($irregular->contains($string)) {
            return $irregular[$string];

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
        $translator = translator();

        if (!$translator->isEnabled()) {
            return $string;
        }

        $rules = $translator->current()?->getInflectionRules();

        if ($rules === null) {
            return $string;
        }

        $string = mb_strtolower($string);
        $uninflected = $rules->getUninflectedWords();
        $irregular = $rules->getIrregularWords();
        $singulars = $rules->getSingularPatterns();

        if ($uninflected->contains($string)) {
            return $string;

        } else if ($irregular->contains($string)) {
            return $irregular[$string];

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
        $translator = translator();

        if (!$translator->isEnabled()) {
            return $string;
        }

        $transliterations = $translator->current()?->getInflectionRules()?->getTransliterations();

        if ($transliterations === null) {
            return $string;
        }

        // Replace with ASCII characters
        $string = str_replace($transliterations->keys(), $transliterations->values(), $string);

        // Remove any left over non 7bit ASCII
        return preg_replace('/[^\x09\x0A\x0D\x20-\x7E]/', '', $string);
    }

}
