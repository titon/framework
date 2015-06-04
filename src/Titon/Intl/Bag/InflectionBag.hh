<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl\Bag;

use Titon\Common\Bag\AbstractBag;

/**
 * A bag for locale specific inflection rules.
 *
 * @package Titon\Intl\Bag
 */
class InflectionBag extends AbstractBag<string, mixed> {

    /**
     * Return the irregular words map.
     *
     * @return Map<string, string>
     */
    public function getIrregularWords(): Map<string, string> {
        $irregular = $this->get('irregular');

        if ($irregular instanceof Map) {
            return $irregular;
        }

        return Map {};
    }

    /**
     * Return the ordinal suffixes map.
     *
     * @return Map<int, string>
     */
    public function getOrdinalSuffixes(): Map<int, string> {
        $suffixes = $this->get('ordinal');

        if ($suffixes instanceof Map) {
            return $suffixes;
        }

        return Map {};
    }

    /**
     * Return the plural patterns map.
     *
     * @return Map<string, string>
     */
    public function getPluralPatterns(): Map<string, string> {
        $patterns = $this->get('plural');

        if ($patterns instanceof Map) {
            return $patterns;
        }

        return Map {};
    }

    /**
     * Return the singular patterns map.
     *
     * @return Map<string, string>
     */
    public function getSingularPatterns(): Map<string, string> {
        $patterns = $this->get('singular');

        if ($patterns instanceof Map) {
            return $patterns;
        }

        return Map {};
    }

    /**
     * Return the transliterations map.
     *
     * @return Map<string, string>
     */
    public function getTransliterations(): Map<string, string> {
        $transliterations = $this->get('transliteration');

        if ($transliterations instanceof Map) {
            return $transliterations;
        }

        return Map {};
    }

    /**
     * Return the uninflected words list.
     *
     * @return Set<string>
     */
    public function getUninflectedWords(): Set<string> {
        $uninflected = $this->get('uninflected');

        if ($uninflected instanceof Set) {
            return $uninflected;
        }

        return Set {};
    }

}
