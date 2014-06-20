<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

/**
 * The Number utility allows for the twiddling and calculation of numbers and floats.
 * Provides helper methods to ease in the evaluation of numbers within context.
 *
 * @package Titon\Utility
 */
class Number extends Macro {

    /**
     * Bases.
     */
    const BINARY = 2;
    const OCTAL = 8;
    const DECIMAL = 10;
    const HEX = 16;

    /**
     * Convert a readable string notated form of bytes (1KB) to the numerical equivalent (1024).
     * Supports all the different format variations: k, kb, ki, kib, etc.
     *
     * @param int $number
     * @return int
     */
    public static function bytesFrom($number) {
        if (!$number) {
            return 0;

        } else if (is_numeric($number)) {
            return $number;
        }

        $number = trim((string) $number);
        $sizes = array(
            'k|kb|ki|kib' => 10,
            'm|mb|mi|mib' => 20,
            'g|gb|gi|gib' => 30,
            't|tb|ti|tib' => 40,
            'p|pb|pi|pib' => 50,
            'e|eb|ei|eib' => 60,
            'z|zb|zi|zib' => 70,
            'y|yb|yi|yib' => 80,
            'b' => 0
        );

        foreach ($sizes as $format => $pow) {
            if (preg_match('/^([0-9\.]+)(' . $format . ')$/i', $number, $matches)) {
                return (((float) $matches[1]) * pow(2, $pow));
            }
        }

        return null;
    }

    /**
     * Convert a numerical value to the readable string notated equivalent.
     *
     * @param int $size
     * @param int $precision
     * @return string
     */
    public static function bytesTo($size, $precision = 0) {
        $sizes = array('YB', 'ZB', 'EB', 'PB', 'TB', 'GB', 'MB', 'KB', 'B');
        $total = count($sizes);

        while ($total-- && $size >= 1024) {
            $size /= 1024;
        }

        return static::precision($size, $precision) . $sizes[$total];
    }

    /**
     * Convert a number from one base to another.
     *
     * @param int $no
     * @param int $fromBase
     * @param int $toBase
     * @return int
     */
    public static function convert($no, $fromBase, $toBase) {
        if ($fromBase == $toBase) {
            return $no;
        }

        return base_convert($no, $fromBase, $toBase);
    }

    /**
     * Convert a number to it's currency equivalent, respecting locale.
     * Allow for overrides through an options array.
     *
     * @param int $number
     * @param array $options {
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
    public static function currency($number, array $options = array()) {
        $defaults = array(
            'thousands' => ',',
            'decimals' => '.',
            'places' => 2,
            'code' => 'USD #',
            'dollar' => '$#',
            'cents' => '#&cent;',
            'use' => 'dollar',
            'negative' => '(#)'
        );

        $options = $options + $defaults;
        $amount = number_format(static::precision(abs($number), $options['places']), $options['places'], $options['decimals'], $options['thousands']);

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
     * @param int $number
     * @param int $min
     * @param int $max
     * @return bool
     */
    public static function in($number, $min, $max) {
        return ($number >= $min && $number <= $max);
    }

    /**
     * Is the current value even?
     *
     * @param int $number
     * @return bool
     */
    public static function isEven($number) {
        return ($number % 2 === 0);
    }

    /**
     * Is the current value negative; less than zero.
     *
     * @param int $number
     * @return bool
     */
    public static function isNegative($number) {
        return ($number < 0);
    }

    /**
     * Is the current value odd?
     *
     * @param int $number
     * @return bool
     */
    public static function isOdd($number) {
        return !static::isEven($number);
    }

    /**
     * Is the current value positive; greater than or equal to zero.
     *
     * @param int $number
     * @param bool $zero
     * @return bool
     */
    public static function isPositive($number, $zero = true) {
        return ($zero ? ($number >= 0) : ($number > 0));
    }

    /**
     * Limits the number between two bounds.
     *
     * @param int $number
     * @param int $min
     * @param int $max
     * @return int
     */
    public static function limit($number, $min, $max) {
        return static::max(static::min($number, $min), $max);
    }

    /**
     * Increase the number to the minimum if below threshold.
     *
     * @param int $number
     * @param int $min
     * @return int
     */
    public static function min($number, $min) {
        if ($number < $min) {
            $number = $min;
        }

        return $number;
    }

    /**
     * Decrease the number to the maximum if above threshold.
     *
     * @param int $number
     * @param int $max
     * @return int
     */
    public static function max($number, $max) {
        if ($number > $max) {
            $number = $max;
        }

        return $number;
    }

    /**
     * Return true if the number is outside the min and max.
     *
     * @param int $number
     * @param int $min
     * @param int $max
     * @return bool
     */
    public static function out($number, $min, $max) {
        return ($number < $min || $number > $max);
    }

    /**
     * Convert a number to a percentage string with decimal and comma separations.
     *
     * @param int $number
     * @param int|array $options {
     *      @type string $thousands Character used for thousands place
     *      @type string $decimals  Character used for decimal
     *      @type int $places       Decimal (cent) limit
     * }
     * @return string
     */
    public static function percentage($number, $options = array()) {
        if (!is_array($options)) {
            $options = array('places' => $options);
        }

        $options = $options + array(
            'thousands' => ',',
            'decimals' => '.',
            'places' => 2
        );

        return number_format(static::precision($number, $options['places']), $options['places'], $options['decimals'], $options['thousands']) . '%';
    }

    /**
     * Formats a number with a level of precision (even if it had none).
     *
     * @param float $number
     * @param int $precision
     * @return float
     */
    public static function precision($number, $precision = 2) {
        return sprintf('%01.' . $precision . 'F', $number);
    }

    /**
     * Returns -1 if the value is negative, 0 if the value equals 0, or 1 if the value is positive.
     *
     * @param int $number
     * @return int
     */
    public static function signum($number) {
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
     * @param int $number
     * @param int $base
     * @return int
     */
    public static function toBinary($number, $base = self::DECIMAL) {
        return static::convert($number, $base, static::BINARY);
    }

    /**
     * Returns as an unsigned integer in base 10 (decimal).
     *
     * @param int $number
     * @param int $base
     * @return int
     */
    public static function toDecimal($number, $base = self::DECIMAL) {
        return static::convert($number, $base, static::DECIMAL);
    }

    /**
     * Returns as an unsigned integer in base 16 (hexadecimal).
     *
     * @param int $number
     * @param int $base
     * @return string
     */
    public static function toHex($number, $base = self::DECIMAL) {
        return static::convert($number, $base, static::HEX);
    }

    /**
     * Returns as an unsigned integer in base 8 (octal).
     *
     * @param int $number
     * @param int $base
     * @return string
     */
    public static function toOctal($number, $base = self::DECIMAL) {
        return static::convert($number, $base, static::OCTAL);
    }

}