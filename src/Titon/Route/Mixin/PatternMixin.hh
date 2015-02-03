<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route\Mixin;

/**
 * Provides functionality for regex patterns.
 *
 * @package Titon\Route\Mixin
 */
trait PatternMixin {

    /**
     * Custom defined regex patterns.
     *
     * @var \Titon\Route\Mixin\PatternMap
     */
    protected PatternMap $patterns = Map {};

    /**
     * Add a regex pattern by token name.
     *
     * @param string $pattern
     * @param string $regex
     * @return $this
     */
    public function addPattern(string $pattern, string $regex): this {
        $this->patterns[$pattern] = $regex;

        return $this;
    }

    /**
     * Add multiple regex patterns.
     *
     * @param \Titon\Route\Mixin\PatternMap $patterns
     * @return $this
     */
    public function addPatterns(PatternMap $patterns): this {
        foreach ($patterns as $pattern => $regex) {
            $this->addPattern($pattern, $regex);
        }

        return $this;
    }

    /**
     * Return all the patterns used for compiling.
     *
     * @return \Titon\Route\Mixin\PatternMap
     */
    public function getPatterns(): PatternMap {
        return $this->patterns;
    }

    /**
     * Set a mapping of regex patterns to parse URLs with.
     *
     * @param \Titon\Route\Mixin\PatternMap $patterns
     * @return $this
     */
    public function setPatterns(PatternMap $patterns): this {
        $this->patterns = $patterns;

        return $this;
    }

}
