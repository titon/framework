<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;
use \DateTime;

/**
 * Format provides utility methods for converting raw data to specific visual formats.
 *
 * @package Titon\Utility
 */
class Format {
    use Macroable;

    /**
     * Format a date string to an Atom feed format.
     *
     * @uses Titon\Utility\Time
     *
     * @param string|int $time
     * @return string
     */
    public static function atom(mixed $time): string {
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
    public static function date(mixed $time, string $format = '%Y-%m-%d'): string {
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
    public static function datetime(mixed $time, string $format = '%Y-%m-%d %H:%M:%S'): string {
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
     * @param string|int $value
     * @param string $format
     * @return string
     */
    public static function format(mixed $value, string $format): string {
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
    public static function http(mixed $time): string {
        return gmdate('D, d M Y H:i:s T', Time::toUnix($time));
    }

    /**
     * Format a phone number. A phone number can support multiple variations,
     * depending on how many numbers are present.
     *
     * @param int $value
     * @param string|array $format
     * @return string
     */
    public static function phone(mixed $value, mixed $format): string {
        $value = preg_replace('/[^0-9]+/', '', $value);

        if (is_traversable($format)) {
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
     * @param Map<string, mixed> $options
     * @return string
     */
    public static function relativeTime(mixed $time, Map<string, mixed> $options = Map {}): string {
        $options = Traverse::merge(Map {
            'seconds' => Vector {'%ss', '%s second', '%s seconds'},
            'minutes' => Vector {'%sm', '%s minute', '%s minutes'},
            'hours' => Vector {'%sh', '%s hour', '%s hours'},
            'days' => Vector {'%sd', '%s day', '%s days'},
            'weeks' => Vector {'%sw', '%s week', '%s weeks'},
            'months' => Vector {'%sm', '%s month', '%s months'},
            'years' => Vector {'%sy', '%s year', '%s years'},
            'now' => 'just now',
            'in' => 'in %s',
            'ago' => '%s ago',
            'separator' => ', ',
            'verbose' => true,
            'depth' => 2,
            'time' => time()
        }, $options);

        $diff = Time::difference($options['time'], Time::toUnix($time));
        $output = [];

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
    public static function rss(mixed $time): string {
        return date(DateTime::RSS, Time::toUnix($time));
    }

    /**
     * Format a social security number.
     *
     * @param string|int $value
     * @param string $format
     * @return string
     */
    public static function ssn(mixed $value, string $format): string {
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
    public static function time(mixed $time, string $format = '%H:%M:%S'): string {
        return strftime($format, Time::toUnix($time));
    }

}