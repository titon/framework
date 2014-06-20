<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

use Titon\Utility\Inflector;
use Titon\Utility\String as Str;
use Titon\Utility\Sanitize;

/**
 * The String type allows for the modification and manipulation of a string as if it was an object.
 * One can also modify the string using a series of chained method calls that sequentially alter the initial value.
 *
 * @package Titon\Type
 */
class String extends Type {

    /**
     * Append a string to the end of this string.
     *
     * @param string $value
     * @return $this
     */
    public function append($value) {
        return new static($this->value() . (string) $value);
    }

    /**
     * Upper case the first letter of the first word.
     *
     * @return $this
     */
    public function capitalize() {
        return new static(ucfirst($this->value()));
    }

    /**
     * Return the character at the specified index, if not found returns null.
     *
     * @uses Titon\Utility\String
     *
     * @param int $index
     * @return string
     */
    public function charAt($index) {
        return Str::charAt($this->value(), $index);
    }

    /**
     * Removes all extraneous whitespace from a string and trims it.
     *
     * @return $this
     */
    public function clean() {
        return new static(trim(preg_replace('/\s{2,}+/', ' ', $this->value())));
    }

    /**
     * Compares to strings alphabetically. Returns 0 if they are equal,
     * negative if passed value is greater, or positive if current value is greater.
     *
     * @uses Titon\Utility\String
     *
     * @param string $value
     * @param int $length
     * @return int
     */
    public function compare($value, $length = 0) {
        return Str::compare($this->value(), $value, $length);
    }

    /**
     * Concatenate two strings and return a new string object.
     *
     * @param string $string
     * @param bool $append
     * @return $this
     */
    public function concat($string, $append = true) {
        if ($append) {
            return $this->append($string);
        }

        return $this->prepend($string);
    }

    /**
     * Check to see if a string exists within this string.
     *
     * @uses Titon\Utility\String
     *
     * @param string $needle
     * @param bool $strict
     * @param int $offset
     * @return bool
     */
    public function contains($needle, $strict = true, $offset = 0) {
        return Str::contains($this->value(), $needle, $strict, $offset);
    }

    /**
     * Checks to see if the string ends with a specific value.
     *
     * @uses Titon\Utility\String
     *
     * @param string $value
     * @return bool
     */
    public function endsWith($value) {
        return Str::endsWith($this->value(), $value);
    }

    /**
     * Checks to see if both values are equal.
     *
     * @param string $value
     * @return bool
     */
    public function equals($value) {
        return ($this->value() === $value);
    }

    /**
     * Escape the string.
     *
     * @uses Titon\Utility\Sanitize
     *
     * @param int $flags
     * @return $this
     */
    public function escape($flags = ENT_QUOTES) {
        return new static(Sanitize::escape($this->value(), ['flags' => $flags]));
    }

    /**
     * Extracts a portion of a string (substring).
     *
     * @uses Titon\Utility\String
     *
     * @param int $offset
     * @param int $length
     * @return string
     */
    public function extract($offset, $length = null) {
        return new static(Str::extract($this->value(), $offset, $length));
    }

    /**
     * Grab the index of the first matched character.
     *
     * @uses Titon\Utility\String
     *
     * @param string $needle
     * @param bool $strict
     * @param int $offset
     * @return int
     */
    public function indexOf($needle, $strict = true, $offset = 0) {
        return Str::indexOf($this->value(), $needle, $strict, $offset);
    }

    /**
     * Checks to see if the value is empty.
     *
     * @return bool
     */
    public function isBlank() {
        return ($this->value() === '');
    }

    /**
     * Checks to see if the trimmed value is empty.
     *
     * @return bool
     */
    public function isEmpty() {
        return (trim($this->value()) === '');
    }

    /**
     * Checks to see if the trimmed value is not empty.
     *
     * @return bool
     */
    public function isNotBlank() {
        return !$this->isBlank();
    }

    /**
     * Checks to see if the value is not empty.
     *
     * @return bool
     */
    public function isNotEmpty() {
        return !$this->isEmpty();
    }

    /**
     * Grab the index of the last matched character.
     *
     * @uses Titon\Utility\String
     *
     * @param string $needle
     * @param bool $strict
     * @param int $offset
     * @return int
     */
    public function lastIndexOf($needle, $strict = true, $offset = 0) {
        return Str::lastIndexOf($this->value(), $needle, $strict, $offset);
    }

    /**
     * Return the string length.
     *
     * @return int
     */
    public function length() {
        return mb_strlen($this->value());
    }

    /**
     * Perform a regex pattern match.
     *
     * @param string $pattern
     * @param bool $return
     * @param int $flags
     * @return int|array
     */
    public function matches($pattern, $return = false, $flags = 0) {
        $regex = preg_match($pattern, $this->value(), $matches, $flags);

        return $return ? $matches : $regex;
    }

    /**
     * Pad the string with a defined character for a specific length.
     *
     * @param int $length
     * @param string $value
     * @param int $type
     * @return $this
     */
    public function pad($length, $value = ' ', $type = STR_PAD_BOTH) {
        return new static(str_pad($this->value(), $length, $value, $type));
    }

    /**
     * Pad the string on the left.
     *
     * @param int $length
     * @param string $value
     * @return $this
     */
    public function padLeft($length, $value = ' ') {
        return $this->pad($length, $value, STR_PAD_LEFT);
    }

    /**
     * Pad the string on the right.
     *
     * @param int $length
     * @param string $value
     * @return $this
     */
    public function padRight($length, $value = ' ') {
        return $this->pad($length, $value, STR_PAD_RIGHT);
    }

    /**
     * Prepend a string to the beginning of this string.
     *
     * @param string $value
     * @return $this
     */
    public function prepend($value) {
        return new static((string) $value . $this->value());
    }

    /**
     * Replace specific values with a new value.
     *
     * @param string|array $search
     * @param string|array $replace
     * @param bool $strict
     * @return $this
     */
    public function replace($search, $replace, $strict = true) {
        if ($strict) {
            return new static(str_replace($search, $replace, $this->value()));
        }

        return new static(str_ireplace($search, $replace, $this->value()));
    }

    /**
     * Reverse the string.
     *
     * @return $this
     */
    public function reverse() {
        return new static(strrev($this->value()));
    }

    /**
     * Shuffle the string.
     *
     * @return $this
     */
    public function shuffle() {
        return new static(str_shuffle($this->value()));
    }

    /**
     * Checks to see if the string starts with a specific value.
     *
     * @uses Titon\Utility\String
     *
     * @param string $value
     * @return bool
     */
    public function startsWith($value) {
        return Str::startsWith($this->value(), $value);
    }

    /**
     * Strips the string of its tags and anything in between them.
     *
     * @return $this
     */
    public function strip() {
        return new static(strip_tags($this->value()));
    }

    /**
     * Split the string.
     *
     * @param string $delimiter
     * @param int $length
     * @return array
     */
    public function split($delimiter = null, $length = null) {
        if ($delimiter !== null) {
            if ($length !== null) {
                return explode((string) $delimiter, $this->value(), $length);
            } else {
                return explode((string) $delimiter, $this->value());
            }
        }

        if (!$length) {
            $length = 1;
        }

        return str_split($this->value(), (int) $length);
    }

    /**
     * Converts the string to a camel case form.
     *
     * @uses Titon\Utility\Inflector
     *
     * @return $this
     */
    public function toCamelCase() {
        return new static(Inflector::camelCase($this->value()));
    }

    /**
     * Lower case the string.
     *
     * @return $this
     */
    public function toLowerCase() {
        return new static(mb_strtolower($this->value()));
    }

    /**
     * Upper case the string.
     *
     * @return $this
     */
    public function toUpperCase() {
        return new static(mb_strtoupper($this->value()));
    }

    /**
     * Upper case the first letter of every word.
     *
     * @return $this
     */
    public function toUpperWords() {
        return new static(mb_convert_case($this->value(), MB_CASE_TITLE));
    }

    /**
     * Trim the string.
     *
     * @param string $char
     * @return $this
     */
    public function trim($char = null) {
        if ($char) {
            return new static(trim($this->value(), $char));
        }

        return new static(trim($this->value()));
    }

    /**
     * Trim the string on the left.
     *
     * @param string $char
     * @return $this
     */
    public function trimLeft($char = null) {
        if ($char) {
            return new static(ltrim($this->value(), $char));
        }

        return new static(ltrim($this->value()));
    }

    /**
     * Trim the string on the right.
     *
     * @param string $char
     * @return $this
     */
    public function trimRight($char = null) {
        if ($char) {
            return new static(rtrim($this->value(), $char));
        }

        return new static(rtrim($this->value()));
    }

    /**
     * Lower case the first letter of the first word.
     *
     * @return $this
     */
    public function uncapitalize() {
        return new static(lcfirst($this->value()));
    }

    /**
     * Count how many words exist within the string.
     *
     * @param string $inherit
     * @return int
     */
    public function wordCount($inherit = '') {
        return str_word_count($this->value(), 0, $inherit);
    }

    /**
     * {@inheritdoc}
     */
    public function write($value) {
        return parent::write((string) $value);
    }

}