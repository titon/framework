<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;
use \DateTime;
use \Indexish;

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
    public static function format(string $value, string $format): string {
        $length = mb_strlen($format);
        $result = '';
        $pos = 0;

        for ($i = 0; $i < $length; $i++) {
            $char = $format[$i];

            if ($char === '#') {
                $raw = mb_substr($value, $pos, 1);
                $result .= ($raw === false || $raw === '') ? '#' : $raw;
                $pos++;

            } else if ($char === '*') {
                $result .= $char;
                $pos++;

            } else {
                $result .= $char;
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
     * @param string $value
     * @param string|Map $format
     * @return string
     */
    public static function phone(string $value, mixed $format): string {
        $value = preg_replace('/[^0-9]+/', '', $value);
        $pattern = $format;

        if ($format instanceof Map) {
            $length = mb_strlen($value);

            if ($length >= 11 && $format->contains(11)) {
                $pattern = $format[11];

            } else if ($length >= 10 && $format->contains(10)) {
                $pattern = $format[10];

            } else if ($format->contains(7)) {
                $pattern = $format[7];

            } else {
                $pattern = str_repeat('#', $length);
            }
        }

        return static::format($value, (string) $pattern);
    }

    /**
     * Format a timestamp as a date relative human readable string; also known as time ago in words.
     *
     * @uses Titon\Utility\Time
     *
     * @param string|int $time
     * @param Map<string, mixed> $options
     * @param Map<string, Map<int, string>> $messages
     * @return string
     */
    public static function relativeTime(mixed $time, Map<string, mixed> $options = Map {}, Map<string, Map<int, string>> $messages = Map {}): string {
        $options = (Map {
            'now' => 'just now',
            'in' => 'in {time}',
            'ago' => '{time} ago',
            'separator' => ', ',
            'verbose' => true,
            'depth' => 2,
            'time' => time()
        })->setAll($options);

        $messages = Col::merge(Map {
            'seconds'   => Map {0 => '{count}s', 1 => '{count} second', 2 => '{count} seconds'},
            'minutes'   => Map {0 => '{count}m', 1 => '{count} minute', 2 => '{count} minutes'},
            'hours'     => Map {0 => '{count}h', 1 => '{count} hour', 2 => '{count} hours'},
            'days'      => Map {0 => '{count}d', 1 => '{count} day', 2 => '{count} days'},
            'weeks'     => Map {0 => '{count}w', 1 => '{count} week', 2 => '{count} weeks'},
            'months'    => Map {0 => '{count}m', 1 => '{count} month', 2 => '{count} months'},
            'years'     => Map {0 => '{count}y', 1 => '{count} year', 2 => '{count} years'}
        }, $messages);

        $diff = Time::difference($options['time'], Time::toUnix($time));

        // Present tense
        if ($diff === 0) {
            return (string) $options['now'];
        }

        // Past or future tense
        $seconds = abs($diff);
        $depth = (int) $options['depth'];
        $output = Vector {};

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

            $count = (int) round($seconds / $div);
            $seconds -= ($count * $div);

            if ($options['verbose']) {
                $index = ($count === 1) ? 1 : 2;
            } else {
                $index = 0;
            }

            $output[] = Str::insert((string) $messages[$key][$index], Map {'count' => $count});
            $depth--;
        }

        return Str::insert((string) $options[($diff > 0) ? 'ago' : 'in'], Map {'time' => implode($options['separator'], $output)});
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
    public static function ssn(string $value, string $format): string {
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