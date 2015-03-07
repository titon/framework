<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use \DateTime;
use \DateTimeZone;

/**
 * Time provides functionality for calculating date and time ranges and making timezones easy to use.
 *
 * @package Titon\Utility
 */
class Time {

    /**
     * Time constants represented as seconds.
     */
    const int YEAR = 31536000;
    const int MONTH = 2592000; // 30 days
    const int WEEK = 604800;
    const int DAY = 86400;
    const int HOUR = 3600;
    const int MINUTE = 60;
    const int SECOND = 1;

    /**
     * Calculate the difference in seconds between 2 dates.
     *
     * @param string|int $time1
     * @param string|int $time2
     * @return int
     */
    public static function difference(mixed $time1, mixed $time2): int {
        return (int) (static::toUnix($time1) - static::toUnix($time2));
    }

    /**
     * Return a DateTime object based on the current time and timezone.
     *
     * @param string|int $time
     * @param string $timezone
     * @return \DateTime
     */
    public static function factory(mixed $time = null, mixed $timezone = null): DateTime {
        $timezone = static::timezone($timezone);

        if ($time instanceof DateTime) {
            return $time->setTimezone($timezone);
        }

        if (is_numeric($time)) {
            $time = '@' . (string) $time;
        }

        $dt = new DateTime($time);
        $dt->setTimezone($timezone);

        return $dt;
    }

    /**
     * Returns true if date passed is today.
     *
     * @param string|int $time
     * @return bool
     */
    public static function isToday(mixed $time): bool {
        return (static::factory($time)->format('Y-m-d') === static::factory()->format('Y-m-d'));
    }

    /**
     * Returns true if date passed is within this week.
     *
     * @param string|int $time
     * @return bool
     */
    public static function isThisWeek(mixed $time): bool {
        return (static::factory($time)->format('W-o') === static::factory()->format('W-o'));
    }

    /**
     * Returns true if date passed is within this month.
     *
     * @param string|int $time
     * @return bool
     */
    public static function isThisMonth(mixed $time): bool {
        return (static::factory($time)->format('m-Y') === static::factory()->format('m-Y'));
    }

    /**
     * Returns true if date passed is within this year.
     *
     * @param string|int $time
     * @return bool
     */
    public static function isThisYear(mixed $time): bool {
        return (static::factory($time)->format('Y') === static::factory()->format('Y'));
    }

    /**
     * Returns true if date passed is tomorrow.
     *
     * @param string|int $time
     * @return bool
     */
    public static function isTomorrow(mixed $time): bool {
        return (static::factory($time)->format('Y-m-d') === static::factory('tomorrow')->format('Y-m-d'));
    }

    /**
     * Returns true if the date passed will be within the next time frame span.
     *
     * @param string|int $time
     * @param string|int $span
     * @return bool
     */
    public static function isWithinNext(mixed $time, mixed $span): bool {
        $span = static::factory($span);
        $time = static::factory($time);
        $now = static::factory();

        return ($time <= $span && $time >= $now);
    }

    /**
     * Return a DateTimeZone object based on the current timezone.
     *
     * @param \DateTimeZone|string $timezone
     * @return \DateTimeZone
     */
    public static function timezone(mixed $timezone = null): DateTimeZone {
        if ($timezone instanceof DateTimeZone) {
            return $timezone;
        }

        if (!$timezone) {
            $timezone = date_default_timezone_get();
        }

        return new DateTimeZone($timezone);
    }

    /**
     * Return a unix timestamp. If the time is a string convert it, else cast to int.
     *
     * @param string|int $time
     * @return int
     */
    public static function toUnix(mixed $time): int {
        if (!$time) {
            return time();

        } else if ($time instanceof DateTime) {
            return (int) $time->format('U');
        }

        return (int) (is_string($time) ? strtotime($time) : $time);
    }

    /**
     * Returns true if date passed was within last week.
     *
     * @param string|int $time
     * @return bool
     */
    public static function wasLastWeek(mixed $time): bool {
        $start = static::factory('last week 00:00:00');

        $end = clone $start;
        $end->modify('next week -1 second');

        $time = static::factory($time);

        return ($time >= $start && $time <= $end);
    }

    /**
     * Returns true if date passed was within last month.
     *
     * @param string|int $time
     * @return bool
     */
    public static function wasLastMonth(mixed $time): bool {
        $start = static::factory('first day of last month 00:00:00');

        $end = clone $start;
        $end->modify('next month -1 second');

        $time = static::factory($time);

        return ($time >= $start && $time <= $end);
    }

    /**
     * Returns true if date passed was within last year.
     *
     * @param string|int $time
     * @return bool
     */
    public static function wasLastYear(mixed $time): bool {
        $start = static::factory('last year January 1st 00:00:00');

        $end = clone $start;
        $end->modify('next year -1 second');

        $time = static::factory($time);

        return ($time >= $start && $time <= $end);
    }

    /**
     * Returns true if date passed was yesterday.
     *
     * @param string|int $time
     * @return bool
     */
    public static function wasYesterday(mixed $time): bool {
        return (static::factory($time)->format('Y-m-d') === static::factory('yesterday')->format('Y-m-d'));
    }

    /**
     * Returns true if the date passed was within the last time frame span.
     *
     * @param string|int $time
     * @param string|int $span
     * @return bool
     */
    public static function wasWithinLast(mixed $time, mixed $span): bool {
        $span = static::factory($span);
        $time = static::factory($time);
        $now = static::factory();

        return ($time >= $span && $time <= $now);
    }

}
