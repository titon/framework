<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

use Titon\Utility\Inflector;
use Titon\Utility\Sanitize;
use Titon\Utility\Str;
use \Serializable;

/**
 * The StringBuffer allows for immutable modification and manipulation of a string as if it was an object.
 * One can also modify the string using a series of chained method calls that sequentially alter the initial value.
 *
 * @package Titon\Type
 */
class StringBuffer implements Serializable {

    /**
     * Raw string value.
     *
     * @var string
     */
    protected string $value = '';

    /**
     * Set the value.
     *
     * @param string $value
     */
    final public function __construct(string $value = '') {
        $this->write($value);
    }

    /**
     * Define magic to string.
     *
     * @return string
     */
    public function __toString(): string {
        return $this->toString();
    }

    /**
     * Append a string to the end of this string.
     *
     * @param string $value
     * @return \Titon\Type\StringBuffer
     */
    public function append(string $value): StringBuffer {
        return new static($this->value() . $value);
    }

    /**
     * Upper case the first letter of the first word.
     *
     * @return \Titon\Type\StringBuffer
     */
    public function capitalize(): StringBuffer {
        return new static(ucfirst($this->value()));
    }

    /**
     * Return the character at the specified index, if not found returns null.
     *
     * @uses Titon\Utility\Str
     *
     * @param int $index
     * @return string
     */
    public function charAt(int $index): ?string {
        return Str::charAt($this->value(), $index);
    }

    /**
     * Removes all extraneous whitespace from a string and trims it.
     *
     * @return \Titon\Type\StringBuffer
     */
    public function clean(): StringBuffer {
        return new static(trim(preg_replace('/\s{2,}+/', ' ', $this->value())));
    }

    /**
     * Compares to strings alphabetically. Returns 0 if they are equal,
     * negative if passed value is greater, or positive if current value is greater.
     *
     * @uses Titon\Utility\Str
     *
     * @param string $value
     * @param int $length
     * @return int
     */
    public function compare(string $value, int $length = 0): int {
        return Str::compare($this->value(), $value, $length);
    }

    /**
     * Concatenate two strings and return a new string object.
     *
     * @param string $string
     * @param bool $append
     * @return \Titon\Type\StringBuffer
     */
    public function concat(string $string, bool $append = true): StringBuffer {
        if ($append) {
            return $this->append($string);
        }

        return $this->prepend($string);
    }

    /**
     * Check to see if a string exists within this string.
     *
     * @uses Titon\Utility\Str
     *
     * @param string $needle
     * @param bool $strict
     * @param int $offset
     * @return bool
     */
    public function contains(string $needle, bool $strict = true, int $offset = 0): bool {
        return Str::contains($this->value(), $needle, $strict, $offset);
    }

    /**
     * Checks to see if the string ends with a specific value.
     *
     * @uses Titon\Utility\Str
     *
     * @param string $value
     * @return bool
     */
    public function endsWith(string $value): bool {
        return Str::endsWith($this->value(), $value);
    }

    /**
     * Checks to see if both values are equal.
     *
     * @param string $value
     * @return bool
     */
    public function equals(string $value): bool {
        return ($this->value() === $value);
    }

    /**
     * Escape the string.
     *
     * @uses Titon\Utility\Sanitize
     *
     * @param int $flags
     * @return \Titon\Type\StringBuffer
     */
    public function escape(int $flags = ENT_QUOTES): StringBuffer {
        return new static(Sanitize::escape($this->value(), Map {'flags' => $flags}));
    }

    /**
     * Extracts a portion of a string (substring).
     *
     * @uses Titon\Utility\Str
     *
     * @param int $offset
     * @param int $length
     * @return string
     */
    public function extract(int $offset, int $length = 0): StringBuffer {
        return new static(Str::extract($this->value(), $offset, $length));
    }

    /**
     * Grab the index of the first matched character.
     *
     * @uses Titon\Utility\Str
     *
     * @param string $needle
     * @param bool $strict
     * @param int $offset
     * @return int
     */
    public function indexOf(string $needle, bool $strict = true, int $offset = 0): int {
        return Str::indexOf($this->value(), $needle, $strict, $offset);
    }

    /**
     * Checks to see if the value is empty.
     *
     * @return bool
     */
    public function isEmpty(): bool {
        return ($this->value() === '');
    }

    /**
     * Grab the index of the last matched character.
     *
     * @uses Titon\Utility\Str
     *
     * @param string $needle
     * @param bool $strict
     * @param int $offset
     * @return int
     */
    public function lastIndexOf(string $needle, bool $strict = true, int $offset = 0): int {
        return Str::lastIndexOf($this->value(), $needle, $strict, $offset);
    }

    /**
     * Return the string length.
     *
     * @return int
     */
    public function length(): int {
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
    public function matches(string $pattern, bool $return = false, int $flags = 0): mixed {
        $matches = [];
        $regex = preg_match($pattern, $this->value(), $matches, $flags);

        return $return ? $matches : $regex;
    }

    /**
     * Pad the string with a defined character for a specific length.
     *
     * @param int $length
     * @param string $value
     * @param int $type
     * @return \Titon\Type\StringBuffer
     */
    public function pad(int $length, string $value = ' ', int $type = STR_PAD_BOTH): StringBuffer {
        return new static(str_pad($this->value(), $length, $value, $type));
    }

    /**
     * Pad the string on the left.
     *
     * @param int $length
     * @param string $value
     * @return \Titon\Type\StringBuffer
     */
    public function padLeft(int $length, string $value = ' '): StringBuffer {
        return $this->pad($length, $value, STR_PAD_LEFT);
    }

    /**
     * Pad the string on the right.
     *
     * @param int $length
     * @param string $value
     * @return \Titon\Type\StringBuffer
     */
    public function padRight(int $length, string $value = ' '): StringBuffer {
        return $this->pad($length, $value, STR_PAD_RIGHT);
    }

    /**
     * Prepend a string to the beginning of this string.
     *
     * @param string $value
     * @return \Titon\Type\StringBuffer
     */
    public function prepend(string $value): StringBuffer {
        return new static($value . $this->value());
    }

    /**
     * Replace specific values with a new value.
     *
     * @param string|array $search
     * @param string|array $replace
     * @param bool $strict
     * @return \Titon\Type\StringBuffer
     */
    public function replace(mixed $search, mixed $replace, bool $strict = true): StringBuffer {
        if ($strict) {
            return new static(str_replace($search, $replace, $this->value()));
        }

        return new static(str_ireplace($search, $replace, $this->value()));
    }

    /**
     * Reverse the string.
     *
     * @return \Titon\Type\StringBuffer
     */
    public function reverse(): StringBuffer {
        return new static(strrev($this->value()));
    }

    /**
     * Return the value serialized.
     *
     * @return string
     */
    public function serialize(): string {
        return serialize($this->value());
    }

    /**
     * Shuffle the string.
     *
     * @return \Titon\Type\StringBuffer
     */
    public function shuffle(): StringBuffer {
        return new static(str_shuffle($this->value()));
    }

    /**
     * Checks to see if the string starts with a specific value.
     *
     * @uses Titon\Utility\Str
     *
     * @param string $value
     * @return bool
     */
    public function startsWith(string $value): bool {
        return Str::startsWith($this->value(), $value);
    }

    /**
     * Strips the string of its tags and anything in between them.
     *
     * @return \Titon\Type\StringBuffer
     */
    public function strip(): StringBuffer {
        return new static(strip_tags($this->value()));
    }

    /**
     * Split the string.
     *
     * @param string $delimiter
     * @param int $length
     * @return Vector<string>
     */
    public function split(string $delimiter = '', int $length = 0): Vector<string> {
        if ($delimiter !== '') {
            if ($length !== 0) {
                $chars = explode($delimiter, $this->value(), $length);
            } else {
                $chars = explode($delimiter, $this->value());
            }
        } else {
            $chars = str_split($this->value(), $length ?: 1);
        }

        return new Vector($chars);
    }

    /**
     * Converts the string to a camel case form.
     *
     * @uses Titon\Utility\Inflector
     *
     * @return \Titon\Type\StringBuffer
     */
    public function toCamelCase(): StringBuffer {
        return new static(Inflector::camelCase($this->value()));
    }

    /**
     * Lower case the string.
     *
     * @return \Titon\Type\StringBuffer
     */
    public function toLowerCase(): StringBuffer {
        return new static(mb_strtolower($this->value()));
    }

    /**
     * Return the raw string.
     *
     * @return string
     */
    public function toString(): string {
        return $this->value();
    }

    /**
     * Upper case the string.
     *
     * @return \Titon\Type\StringBuffer
     */
    public function toUpperCase(): StringBuffer {
        return new static(mb_strtoupper($this->value()));
    }

    /**
     * Upper case the first letter of every word.
     *
     * @return \Titon\Type\StringBuffer
     */
    public function toUpperWords(): StringBuffer {
        return new static(mb_convert_case($this->value(), MB_CASE_TITLE));
    }

    /**
     * Trim the string.
     *
     * @param string $char
     * @return \Titon\Type\StringBuffer
     */
    public function trim(string $char = ''): StringBuffer {
        if ($char !== '') {
            return new static(trim($this->value(), $char));
        }

        return new static(trim($this->value()));
    }

    /**
     * Trim the string on the left.
     *
     * @param string $char
     * @return \Titon\Type\StringBuffer
     */
    public function trimLeft(string $char = ''): StringBuffer {
        if ($char !== '') {
            return new static(ltrim($this->value(), $char));
        }

        return new static(ltrim($this->value()));
    }

    /**
     * Trim the string on the right.
     *
     * @param string $char
     * @return \Titon\Type\StringBuffer
     */
    public function trimRight(string $char = ''): StringBuffer {
        if ($char !== '') {
            return new static(rtrim($this->value(), $char));
        }

        return new static(rtrim($this->value()));
    }

    /**
     * Lower case the first letter of the first word.
     *
     * @return \Titon\Type\StringBuffer
     */
    public function uncapitalize(): StringBuffer {
        return new static(lcfirst($this->value()));
    }

    /**
     * Set the value after unserialization.
     *
     * @param string $value
     */
    public function unserialize(/* HH_FIXME[4032]: no type hint */ $value): void {
        $this->write(unserialize($value));
    }

    /**
     * Return the current value.
     *
     * @return string
     */
    public function value(): string {
        return $this->value;
    }

    /**
     * Count how many words exist within the string.
     *
     * @param string $inherit
     * @return int
     */
    public function wordCount(string $inherit = ''): int {
        return str_word_count($this->value(), 0, $inherit);
    }

    /**
     * Set or overwrite the value.
     *
     * @param string $value
     * @return $this
     */
    public function write(string $value): this {
        $this->value = $value;

        return $this;
    }

}
