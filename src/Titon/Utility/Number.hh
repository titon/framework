<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\OptionMap;
use Titon\Common\Macroable;

/**
 * The Number utility allows for the twiddling and calculation of numbers and floats.
 * Provides helper methods to ease in the evaluation of numbers within context.
 *
 * @package Titon\Utility
 */
class Number {
    use Macroable;

    /**
     * Bases.
     */
    const int BINARY = 2;
    const int OCTAL = 8;
    const int DECIMAL = 10;
    const int HEX = 16;

    /**
     * Convert a readable string notated form of bytes (1KB) to the numerical equivalent (1024).
     * Supports all the different format variations: k, kb, ki, kib, etc.
     *
     * @param string $number
     * @return int
     */
    public static function bytesFrom(string $number): int {
        if (!$number) {
            return 0;

        } else if (is_numeric($number)) {
            return (int) $number;
        }

        $number = trim((string) $number);
        $sizes = [
            'k|kb|ki|kib' => 10,
            'm|mb|mi|mib' => 20,
            'g|gb|gi|gib' => 30,
            't|tb|ti|tib' => 40,
            'p|pb|pi|pib' => 50,
            'e|eb|ei|eib' => 60,
            'z|zb|zi|zib' => 70,
            'y|yb|yi|yib' => 80,
            'b' => 0
        ];

        foreach ($sizes as $format => $pow) {
            $matches = Vector {}; // Type checker needs it defined

            if (preg_match('/^([0-9\.]+)(' . $format . ')$/i', $number, $matches)) {
                return (int) (((float) $matches[1]) * pow(2, $pow));
            }
        }

        return 0;
    }

    /**
     * Convert a numerical value to the readable string notated equivalent.
     * The size must be a string to support large integers and floats.
     *
     * @param int|float $size
     * @return string
     */
    public static function bytesTo(num $size): string {
        $sizes = ['YB', 'ZB', 'EB', 'PB', 'TB', 'GB', 'MB', 'KB', 'B'];
        $total = count($sizes);

        while ($total-- && $size >= 1024) {
            $size /= 1024;
        }

        return $size . $sizes[$total];
    }

    /**
     * Convert a number from one base to another.
     *
     * @param int|float $no
     * @param int $fromBase
     * @param int $toBase
     * @return string
     */
    public static function convert(mixed $no, int $fromBase, int $toBase): string {
        if ($fromBase === $toBase) {
            return (string) $no;
        }

        return base_convert($no, $fromBase, $toBase);
    }

    /**
     * Convert a number to it's currency equivalent, respecting locale.
     * Allow for overrides through an options array.
     *
     * @param int|float $number
     * @param \Titon\Common\OptionMap $options {
     *      @type string $thousands Character used for thousands place
     *      @type string $decimals  Character used for decimal
     *      @type int $places       Decimal (cent) limit
     *      @type string $code      Currency code with replaceable hash
     *      @type string $dollar    Dollar sign with replaceable hash
     *      @type string $cents     Cent sign with replaceable hash
     *      @type string $use       Whether to use dollar or code for formatting
     *      @type string $negative  Negative sign with replaceable hash
     * }
     * @return string
     */
    public static function currency(num $number, OptionMap $options = Map {}): string {
        $options = (Map {
            'thousands' => ',',
            'decimals' => '.',
            'places' => 2,
            'code' => 'USD #',
            'dollar' => '$#',
            'cents' => '#&cent;',
            'use' => 'dollar',
            'negative' => '(#)'
        })->setAll($options);

        $amount = number_format(static::precision(abs($number), (int) $options['places']), $options['places'], $options['decimals'], $options['thousands']);

        // Cents
        if (($number < 1 && $number > -1) && $options['cents']) {
            $amount = str_replace('#', $amount, $options['cents']);

        // Dollars
        } else {
            if ($options['use'] === 'dollar') {
                $amount = str_replace('#', $amount, $options['dollar']);
            } else {
                $amount = str_replace('#', $amount, $options['code']);
            }
        }

        // Negative
        if ($number < 0 && $options['negative']) {
            $amount = str_replace('#', $amount, $options['negative']);
        }

        return $amount;
    }

    /**
     * Return true if the number is within the min and max.
     *
     * @param int|float $number
     * @param int|float $min
     * @param int|float $max
     * @return bool
     */
    public static function in(num $number, num $min, num $max): bool {
        return ($number >= $min && $number <= $max);
    }

    /**
     * Is the current value even?
     *
     * @param int $number
     * @return bool
     */
    public static function isEven(int $number): bool {
        return ($number % 2 === 0);
    }

    /**
     * Is the current value negative; less than zero.
     *
     * @param int|float $number
     * @return bool
     */
    public static function isNegative(num $number): bool {
        return ($number < 0);
    }

    /**
     * Is the current value odd?
     *
     * @param int $number
     * @return bool
     */
    public static function isOdd(int $number): bool {
        return !static::isEven($number);
    }

    /**
     * Is the current value positive; greater than or equal to zero.
     *
     * @param int|float $number
     * @param bool $zero
     * @return bool
     */
    public static function isPositive(num $number, bool $zero = true): bool {
        return ($zero ? ($number >= 0) : ($number > 0));
    }

    /**
     * Limits the number between two bounds.
     *
     * @param int|float $number
     * @param int|float $min
     * @param int|float $max
     * @return int|float
     */
    public static function limit(num $number, num $min, num $max): num {
        return static::max(static::min($number, $min), $max);
    }

    /**
     * Increase the number to the minimum if below threshold.
     *
     * @param int|float $number
     * @param int|float $min
     * @return int|float
     */
    public static function min(num $number, num $min): num {
        return ($number < $min) ? $min : $number;
    }

    /**
     * Decrease the number to the maximum if above threshold.
     *
     * @param int|float $number
     * @param int|float $max
     * @return int|float
     */
    public static function max(num $number, num $max): num {
        return ($number > $max) ? $max : $number;
    }

    /**
     * Return true if the number is outside the min and max.
     *
     * @param int|float $number
     * @param int|float $min
     * @param int|float $max
     * @return bool
     */
    public static function out(num $number, num $min, num $max): bool {
        return ($number < $min || $number > $max);
    }

    /**
     * Convert a number to a percentage string with decimal and comma separations.
     *
     * @param int|float $number
     * @param \Titon\Common\OptionMap $options {
     *      @type string $thousands Character used for thousands place
     *      @type string $decimals  Character used for decimal
     *      @type int $places       Decimal (cent) limit
     * }
     * @return string
     */
    public static function percentage(num $number, OptionMap $options = Map {}): string {
        $options = (Map {
            'thousands' => ',',
            'decimals' => '.',
            'places' => 2
        })->setAll($options);

        return number_format(static::precision($number, (int) $options['places']), $options['places'], $options['decimals'], $options['thousands']) . '%';
    }

    /**
     * Formats a number with a level of precision (even if it had none).
     *
     * @param int|float $number
     * @param int $precision
     * @return float
     */
    public static function precision(num $number, int $precision = 2): float {
        $float = $number;

        // Hack's type checker doesn't allow variable sprintf() arguments, only literal strings.
        // So we have to use this approach. Let's hope no one needs an outlandish precision.
        switch ($precision) {
            case 1: $float = sprintf("%01.1f", $number); break;
            default:
            case 2: $float = sprintf("%01.2f", $number); break;
            case 3: $float = sprintf("%01.3f", $number); break;
            case 4: $float = sprintf("%01.4f", $number); break;
            case 5: $float = sprintf("%01.5f", $number); break;
        }

        return (float) $float;
    }

    /**
     * Returns -1 if the value is negative, 0 if the value equals 0, or 1 if the value is positive.
     *
     * @param int|float $number
     * @return int
     */
    public static function signum(num $number): int {
        if ($number < 0) {
            return -1;

        } else if ($number == 0) {
            return 0;

        } else {
            return 1;
        }
    }

    /**
     * Returns as an unsigned integer in base 2 (binary).
     *
     * @param int|float $number
     * @param int $base
     * @return string
     */
    public static function toBinary(mixed $number, int $base = self::DECIMAL): string {
        return static::convert($number, $base, self::BINARY);
    }

    /**
     * Returns as an unsigned integer in base 10 (decimal).
     *
     * @param int|float $number
     * @param int $base
     * @return string
     */
    public static function toDecimal(mixed $number, int $base = self::DECIMAL): string {
        return static::convert($number, $base, self::DECIMAL);
    }

    /**
     * Returns as an unsigned integer in base 16 (hexadecimal).
     *
     * @param int|float $number
     * @param int $base
     * @return string
     */
    public static function toHex(mixed $number, int $base = self::DECIMAL): string {
        return static::convert($number, $base, self::HEX);
    }

    /**
     * Returns as an unsigned integer in base 8 (octal).
     *
     * @param int|float $number
     * @param int $base
     * @return string
     */
    public static function toOctal(mixed $number, int $base = self::DECIMAL): string {
        return static::convert($number, $base, self::OCTAL);
    }

}