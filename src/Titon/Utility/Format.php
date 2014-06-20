<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use \DateTime;

/**
 * Format provides utility methods for converting raw data to specific visual formats.
 *
 * @package Titon\Utility
 */
class Format extends Macro {

    /**
     * Format a date string to an Atom feed format.
     *
     * @uses Titon\Utility\Time
     *
     * @param string|int $time
     * @return string
     */
    public static function atom($time) {
        return date(DateTime::ATOM, Time::toUnix($time));
    }

    /**
     * Format a date string.
     *
     * @uses Titon\Utility\Time
     *
     * @param string|int $time
     * @param string $format
     * @return string
     */
    public static function date($time, $format = '%Y-%m-%d') {
        return strftime($format, Time::toUnix($time));
    }

    /**
     * Format a datetime string.
     *
     * @uses Titon\Utility\Time
     *
     * @param string|int $time
     * @param string $format
     * @return string
     */
    public static function datetime($time, $format = '%Y-%m-%d %H:%M:%S') {
        return strftime($format, Time::toUnix($time));
    }

    /**
     * Format a value to a certain string sequence. All #'s in the format will be replaced by the character in the same position within the sequence.
     * All *'s will mask the character in the sequence. Large numbers should be passed as strings.
     *
     * @example {
     *      Format::format(1234567890, '(###) ###-####');               (123) 456-7890
     *      Format::format(1234567890123456, '****-****-####-####');    ***-****-9012-3456
     * }
     *
     * @param int|string $value
     * @param string $format
     * @return string
     */
    public static function format($value, $format) {
        $value = (string) $value;
        $length = mb_strlen($format);
        $result = $format;
        $pos = 0;

        for ($i = 0; $i < $length; $i++) {
            $char = $format[$i];

            if (($char === '#' || $char === '*') && isset($value[$pos])) {
                $replace = ($char === '*') ? '*' : $value[$pos];
                $result = substr_replace($result, $replace, $i, 1);
                $pos++;
            }
        }

        return $result;
    }

    /**
     * Format a date string to an HTTP header format.
     *
     * @uses Titon\Utility\Time
     *
     * @param string|int $time
     * @return string
     */
    public static function http($time) {
        return gmdate('D, d M Y H:i:s T', Time::toUnix($time));
    }

    /**
     * Format a phone number. A phone number can support multiple variations,
     * depending on how many numbers are present.
     *
     * @param int $value
     * @param string $format
     * @return string
     */
    public static function phone($value, $format) {
        $value = preg_replace('/[^0-9]+/', '', $value);

        if (is_array($format)) {
            $length = mb_strlen($value);

            if ($length >= 11) {
                $format = $format[11];
            } else if ($length >= 10) {
                $format = $format[10];
            } else {
                $format = $format[7];
            }
        }

        return static::format($value, $format);
    }

    /**
     * Format a timestamp as a date relative human readable string; also known as time ago in words.
     *
     * @uses Titon\Utility\Time
     *
     * @param string|int $time
     * @param array $options
     * @return string
     */
    public static function relativeTime($time, array $options = array()) {
        $defaults = array(
            'seconds' => array('%ss', '%s second', '%s seconds'),
            'minutes' => array('%sm', '%s minute', '%s minutes'),
            'hours' => array('%sh', '%s hour', '%s hours'),
            'days' => array('%sd', '%s day', '%s days'),
            'weeks' => array('%sw', '%s week', '%s weeks'),
            'months' => array('%sm', '%s month', '%s months'),
            'years' => array('%sy', '%s year', '%s years'),
            'now' => 'just now',
            'in' => 'in %s',
            'ago' => '%s ago',
            'separator' => ', ',
            'verbose' => true,
            'depth' => 2,
            'time' => time()
        );

        $options = $options + $defaults;
        $diff = Time::difference($options['time'], Time::toUnix($time));
        $output = array();

        // Present tense
        if ($diff === 0) {
            return $options['now'];
        }

        // Past or future tense
        $seconds = abs($diff);
        $depth = $options['depth'];

        while ($seconds > 0 && $depth > 0) {
            if ($seconds >= Time::YEAR) {
                $key = 'years';
                $div = Time::YEAR;

            } else if ($seconds >= Time::MONTH) {
                $key = 'months';
                $div = Time::MONTH;

            } else if ($seconds >= Time::WEEK) {
                $key = 'weeks';
                $div = Time::WEEK;

            } else if ($seconds >= Time::DAY) {
                $key = 'days';
                $div = Time::DAY;

            } else if ($seconds >= Time::HOUR) {
                $key = 'hours';
                $div = Time::HOUR;

            } else if ($seconds >= Time::MINUTE) {
                $key = 'minutes';
                $div = Time::MINUTE;

            } else {
                $key = 'seconds';
                $div = Time::SECOND;
            }

            $count = round($seconds / $div);
            $seconds -= ($count * $div);

            if ($options['verbose']) {
                $index = ($count == 1) ? 1 : 2;
            } else {
                $index = 0;
            }

            $output[$key] = sprintf($options[$key][$index], $count);
            $depth--;
        }

        $output = implode($options['separator'], $output);

        // Past
        if ($diff > 0) {
            return sprintf($options['ago'], $output);
        }

        return sprintf($options['in'], $output);
    }

    /**
     * Format a date string to an RSS feed format.
     *
     * @uses Titon\Utility\Time
     *
     * @param string|int $time
     * @return string
     */
    public static function rss($time) {
        return date(DateTime::RSS, Time::toUnix($time));
    }

    /**
     * Format a social security number.
     *
     * @param string|int $value
     * @param string $format
     * @return string
     */
    public static function ssn($value, $format) {
        return static::format($value, $format);
    }

    /**
     * Format a time string.
     *
     * @uses Titon\Utility\Time
     *
     * @param string|int $time
     * @param string $format
     * @return string
     */
    public static function time($time, $format = '%H:%M:%S') {
        return strftime($format, Time::toUnix($time));
    }

}