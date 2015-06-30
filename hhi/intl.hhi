<?hh // decl /* -*- php -*- */
/**
 * Copyright (c) 2014, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the 'hack' directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 */

const string INTL_ICU_VERSION = '4.8.1.1';
const string INTL_ICU_DATA_VERSION = '4.8.1';
const int INTL_MAX_LOCALE_LEN = 80;
const int ULOC_ACTUAL_LOCALE = 0;
const int ULOC_VALID_LOCALE = 1;
const int GRAPHEME_EXTR_COUNT = 0;
const int GRAPHEME_EXTR_MAXBYTES = 1;
const int GRAPHEME_EXTR_MAXCHARS = 2;

function collator_get_sort_key(Collator $object, $str);
function numfmt_create($locale, $style, $pattern = null);
function numfmt_format(NumberFormatter $fmt, $value, $type = null);
function numfmt_parse(NumberFormatter $fmt, $value, $type = null, &$position = null);
function numfmt_format_currency(NumberFormatter $fmt, $value, $currency);
function numfmt_parse_currency(NumberFormatter $fmt, $value, &$currency, &$position = null);
function numfmt_set_attribute(NumberFormatter $fmt, $attr, $value);
function numfmt_get_attribute(NumberFormatter $fmt, $attr);
function numfmt_set_text_attribute(NumberFormatter $fmt, $attr, $value);
function numfmt_get_text_attribute(NumberFormatter $fmt, $attr);
function numfmt_set_symbol(NumberFormatter $fmt, $attr, $value);
function numfmt_get_symbol(NumberFormatter $fmt, $attr);
function numfmt_set_pattern(NumberFormatter $fmt, $pattern);
function numfmt_get_pattern(NumberFormatter $fmt, $nf);
function numfmt_get_locale(NumberFormatter $fmt, $type = null);
function numfmt_get_error_code(NumberFormatter $fmt, $nf);
function numfmt_get_error_message(NumberFormatter $fmt, $nf);
function normalizer_normalize($input, $form = Normalizer::FORM_C);
function normalizer_is_normalized($input, $form = Normalizer::FORM_C);
function locale_get_default();
function locale_set_default($name);
function locale_get_primary_language($locale);
function locale_get_script($locale);
function locale_get_region($locale);
function locale_get_keywords($locale);
function locale_get_display_script($locale, $in_locale = null);
function locale_get_display_region($locale, $in_locale = null);
function locale_get_display_name($locale, $in_locale = null);
function locale_get_display_language($locale, $in_locale = null);
function locale_get_display_variant($locale, $in_locale = null);
function locale_compose(array $subtags);
function locale_parse($locale);
function locale_get_all_variants($locale);
function locale_filter_matches($langtag, $locale, $canonicalize = false);
function locale_canonicalize($arg1);
function locale_lookup(array $langtag, $locale, $canonicalize = false, $default = null);
function locale_accept_from_http($header);
function msgfmt_create($locale, $pattern);
function msgfmt_format(MessageFormatter $fmt, array $args);
function msgfmt_format_message(MessageFormatter $fmt, $locale, $pattern, array $args);
function msgfmt_parse(MessageFormatter $fmt, $value);
function msgfmt_parse_message(MessageFormatter $fmt, $locale, $pattern, $source);
function msgfmt_set_pattern(MessageFormatter $fmt, $pattern);
function msgfmt_get_pattern(MessageFormatter $fmt, $mf);
function msgfmt_get_locale(MessageFormatter $fmt, $mf);
function msgfmt_get_error_code(MessageFormatter $fmt, $nf);
function msgfmt_get_error_message(MessageFormatter $fmt, $coll);
function datefmt_create($locale, $datetype, $timetype, $timezone = null, $calendar = null, $pattern = null);
function datefmt_get_datetype(MessageFormatter $mf);
function datefmt_get_timetype(MessageFormatter $mf);
function datefmt_get_calendar(MessageFormatter $mf);
function datefmt_set_calendar(MessageFormatter $mf, $which);
function datefmt_get_locale(MessageFormatter $mf, $which = null);
function datefmt_get_timezone_id(MessageFormatter $mf);
function datefmt_get_calendar_object();
function datefmt_get_timezone();
function datefmt_set_timezone_id(MessageFormatter $mf, $zone);
function datefmt_set_timezone($zone);
function datefmt_get_pattern(MessageFormatter $mf);
function datefmt_set_pattern(MessageFormatter $mf, $pattern);
function datefmt_is_lenient(MessageFormatter $mf);
function datefmt_set_lenient(MessageFormatter $mf, $lenient);
function datefmt_format(MessageFormatter $mf, $value);
function datefmt_format_object($object, $format = null, $locale = null);
function datefmt_parse(MessageFormatter $mf, $value, &$position = null);
function datefmt_localtime(MessageFormatter $mf, $value, &$position = null);
function datefmt_get_error_code(MessageFormatter $mf);
function datefmt_get_error_message(MessageFormatter $mf, $coll);
function grapheme_strlen($input);
function grapheme_strpos($haystack, $needle, $offset = 0);
function grapheme_stripos($haystack, $needle, $offset = 0);
function grapheme_strrpos($haystack, $needle, $offset = 0);
function grapheme_strripos($haystack, $needle, $offset = 0);
function grapheme_substr($string, $start, $length = null);
function grapheme_strstr($haystack, $needle, $before_needle = false);
function grapheme_stristr($haystack, $needle, $before_needle = false);
function grapheme_extract($haystack, $size, $extract_type = null, $start = 0, &$next = null);
function intlcal_create_instance($timeZone = null, $locale = null);
function intlcal_get_keyword_values_for_locale($key, $locale, $commonlyUsed);
function intlcal_get_now();
function intlcal_get_available_locales();
function intl_get($calendar, $field);
function intlcal_get_time($calendar);
function intlcal_set_time($date);
function intlcal_add($calendar, $field, $amount);
function intlcal_set_time_zone($calendar, $timeZone);
function intlcal_after(IntlCalendar $calendarObject, IntlCalendar $calendar);
function intlcal_before(IntlCalendar $calendarObject, IntlCalendar $calendar);
function intlcal_set($calendar, $year, $month, $dayOfMonth = null, $hour = null, $minute = null, $second = null);
function intlcal_roll($calendar, $field, $amountOrUpOrDown);
function intlcal_clear($calendar, $field = null);
function intlcal_field_difference($calendar, $when, $field);
function intlcal_get_actual_maximum($calendar, $field);
function intlcal_get_actual_minimum($calendar, $field);
function intlcal_get_day_of_week_type($calendar, $dayOfWeek);
function intlcal_get_first_day_of_week($calendar);
function intlcal_greates_minimum($calendar, $field);
function intlcal_get($calendar, $index);
function intlcal_get_least_maximum($calendar, $field);
function intlcal_get_greatest_minimum($calendar, $field);
function intlcal_get_locale($calendar, $localeType);
function intcal_get_maximum($calendar, $field);
function intlcal_get_minimal_days_in_first_week($calendar);
function intlcal_get_minimum($calendar, $field);
function intlcal_get_time_zone($calendar);
function intlcal_get_type($calendar);
function intlcal_get_weekend_transition($calendar, $dayOfWeek);
function intlcal_in_daylight_time($calendar);
function intlcal_is_lenient($calendar);
function intlcal_is_set($calendar, $field);
function intlcal_get_maximum($calendar, $field);
function intlcal_is_equivalent_to(IntlCalendar $calendarObject, IntlCalendar $calendar);
function intlcal_is_weekend($calendar, $date = null);
function intlcal_set_first_day_of_week($calendar, $dayOfWeek);
function intlcal_set_lenient($calendar, $isLenient);
function intlcal_get_repeated_wall_time_option($calendar);
function intlcal_equals($calendarObject, $calendar);
function intlcal_get_skipped_wall_time_option($calendar);
function intlcal_set_repeated_wall_time_option($calendar, $wallTimeOption);
function intlcal_set_skipped_wall_time_option($calendar, $wallTimeOption);
function intlcal_from_date_time($dateTime);
function intlcal_to_date_time($calendar);
function intlcal_get_error_code($calendar);
function intlcal_get_error_message($calendar);
function intltz_count_equivalent_ids($zoneId);
function intlz_create_default();
function intltz_create_enumeration($countryOrRawOffset);
function intltz_create_time_zone($zoneId);
function intltz_from_date_time_zone($zoneId);
function intltz_get_canonical_id($zoneId, &$isSystemID);
function intltz_get_display_name($obj, $isDaylight, $style, $locale);
function intltz_get_dst_savings($obj);
function intltz_get_equivalent_id($zoneId, $index);
function intltz_get_error_code($obj);
function intltz_get_error_message($obj);
function intltz_getGMT();
function intltz_get_id($obj);
function intltz_get_offset($obj, $date, $local, &$rawOffset, &$dstOffset);
function intltz_get_raw_offset($obj);
function intltz_get_tz_data_version($obj);
function intltz_has_same_rules($obj, $otherTimeZone);
function intltz_to_date_time_zone($obj);
function intltz_use_daylight_time($obj);
function intlgregcal_create_instance($timeZone = null, $locale = null);
function intlgregcal_set_gregorian_change($obj, $change);
function intlgregcal_get_gregorian_change($obj);
function intlgregcal_is_leap_year($year);
function resourcebundle_create($locale, $bundlename, $fallback = null);
function resourcebundle_get(ResourceBundle $r, $index);
function resourcebundle_count(ResourceBundle $r, $bundle);
function resourcebundle_locales(ResourceBundle $r, $bundlename);
function resourcebundle_get_error_code(ResourceBundle $bundle);
function resourcebundle_get_error_message(ResourceBundle $bundle);
function transliterator_create($id, $direction = null);
function transliterator_create_from_rules($rules, $direction = null);
function transliterator_list_ids();
function transliterator_create_inverse(Transliterator $orig_trans);
function transliterator_transliterate($transliterator, $subject, $start = null, $end = null);
function transliterator_get_error_code(Transliterator $trans);
function transliterator_get_error_message(Transliterator $trans);

class Locale {
    const ACTUAL_LOCALE = 0;
    const VALID_LOCALE = 1;
    const DEFAULT_LOCALE = null;
    const LANG_TAG = 'language';
    const EXTLANG_TAG = 'extlang';
    const SCRIPT_TAG = 'script';
    const REGION_TAG = 'region';
    const VARIANT_TAG = 'variant';
    const GRANDFATHERED_LANG_TAG = 'grandfathered';
    const PRIVATE_TAG = 'private';
    public function __construct();
    public static function getDefault();
    public static function setDefault($locale);
    public static function getPrimaryLanguage($locale);
    public static function getScript($locale);
    public static function getRegion($locale);
    public static function getKeywords($locale);
    public static function getDisplayScript($locale, $in_locale = null);
    public static function getDisplayRegion($locale, $in_locale = null);
    public static function getDisplayName($locale, $in_locale = null);
    public static function getDisplayLanguage($locale, $in_locale = null);
    public static function getDisplayVariant($locale, $in_locale = null);
    public static function composeLocale(array $subtags);
    public static function parseLocale($locale);
    public static function getAllVariants($locale);
    public static function filterMatches($langtag, $locale, $canonicalize = false);
    public static function lookup(array $langtag, $locale, $canonicalize = false, $default = null);
    public static function canonicalize($locale);
    public static function acceptFromHttp($header);
}

class MessageFormatter {
    public function __construct($locale, $pattern);
    public static function create($locale, $pattern);
    public function format(array $args);
    public static function formatMessage($locale, $pattern, array $args);
    public function parse($value);
    public static function parseMessage($locale, $pattern, $source);
    public function setPattern($pattern);
    public function getPattern();
    public function getLocale();
    public function getErrorCode();
    public function getErrorMessage();
}

class IntlDateFormatter {
    const FULL = 0;
    const LONG = 1;
    const MEDIUM = 2;
    const SHORT = 3;
    const NONE = -1;
    const GREGORIAN = 1;
    const TRADITIONAL = 0;
    public function __construct($locale, $datetype, $timetype, $timezone, $calendar, $pattern);
    public static function create($locale, $datetype, $timetype, $timezone = null, $calendar = null, $pattern = null);
    public function getDateType();
    public function getTimeType();
    public function getCalendar();
    public function setCalendar($which);
    public function getTimeZoneId();
    public function getCalendarObject();
    public function getTimeZone();
    public function setTimeZoneId($zone);
    public function setTimezone($zone);
    public function setPattern($pattern);
    public function getPattern();
    public function getLocale($which = null);
    public function setLenient($lenient);
    public function isLenient();
    public function format($value);
    public function formatObject($object, $format = null, $locale = null);
    public function parse($value, &$position = null);
    public function localtime($value, &$position = null);
    public function getErrorCode();
    public function getErrorMessage();
}

class ResourceBundle implements Traversable {
    public function __construct($locale, $bundlename, $fallback);
    public static function create($locale, $bundlename, $fallback = null);
    public function get($index);
    public function count();
    public static function getLocales($bundlename);
    public function getErrorCode();
    public function getErrorMessage();
}

class Transliterator {
    const FORWARD = 0;
    const REVERSE = 1;
    public $id;
    final private function __construct();
    public static function create($id, $direction = null);
    public static function createFromRules($rules, $direction = null);
    public function createInverse();
    public static function listIDs();
    public function transliterate($subject, $start = null, $end = null);
    public function getErrorCode();
    public function getErrorMessage();
}

class IntlGregorianCalendar {
    public static function createInstance($timeZone = null, $locale = null);
    public function setGregorianChange($change);
    public function getGregorianChange();
    public function isLeapYear($year);
}

class IntlCalendar {
    const FIELD_ERA = 0;
    const FIELD_YEAR = 1;
    const FIELD_MONTH = 2;
    const FIELD_WEEK_OF_YEAR = 3;
    const FIELD_WEEK_OF_MONTH = 4;
    const FIELD_DATE = 5;
    const FIELD_DAY_OF_YEAR = 6;
    const FIELD_DAY_OF_WEEK = 7;
    const FIELD_DAY_OF_WEEK_IN_MONTH = 8;
    const FIELD_AM_PM = 9;
    const FIELD_HOUR = 10;
    const FIELD_HOUR_OF_DAY = 11;
    const FIELD_MINUTE = 12;
    const FIELD_SECOND = 13;
    const FIELD_MILLISECOND = 14;
    const FIELD_ZONE_OFFSET = 15;
    const FIELD_DST_OFFSET = 16;
    const FIELD_YEAR_WOY = 17;
    const FIELD_DOW_LOCAL = 18;
    const FIELD_EXTENDED_YEAR = 19;
    const FIELD_JULIAN_DAY = 20;
    const FIELD_MILLISECONDS_IN_DAY = 21;
    const FIELD_IS_LEAP_MONTH = 22;
    const FIELD_FIELD_COUNT = 23;
    const FIELD_DAY_OF_MONTH = 5;
    const DOW_SUNDAY = 1;
    const DOW_MONDAY = 2;
    const DOW_TUESDAY = 3;
    const DOW_WEDNESDAY = 4;
    const DOW_THURSDAY = 5;
    const DOW_FRIDAY = 6;
    const DOW_SATURDAY = 7;
    const DOW_TYPE_WEEKDAY = 0;
    const DOW_TYPE_WEEKEND = 1;
    const DOW_TYPE_WEEKEND_OFFSET = 2;
    const DOW_TYPE_WEEKEND_CEASE = 3;
    const WALLTIME_FIRST = 1;
    const WALLTIME_LAST = 0;
    const WALLTIME_NEXT_VALID = 2;
    private function __construct();
    public function add($field, $amount);
    public function after(IntlCalendar $calendar);
    public function before(IntlCalendar $calendar);
    public function clear($field = null);
    public static function createInstance($timeZone = null, $locale = null);
    public function equals($calendar);
    public function fieldDifference($when, $field);
    public static function fromDateTime($dateTime);
    public function get($field);
    public function getActualMaximum($field);
    public function getActualMinimum($field);
    public static function getAvailableLocales();
    public function getDayOfWeekType($dayOfWeek);
    public function getErrorCode();
    public function getErrorMessage();
    public function getFirstDayOfWeek();
    public function getGreatestMinimum($field);
    public static function getKeywordValuesForLocale($key, $locale, $commonlyUsed);
    public function getLeastMaximum($field);
    public function getLocale($localeType);
    public function getMaximum($field);
    public function getMinimalDaysInFirstWeek();
    public function getMinimum($field);
    public static function getNow();
    public function getRepeatedWallTimeOption();
    public function getSkippedWallTimeOption();
    public function getTime();
    public function getTimeZone();
    public function getType();
    public function getWeekendTransition($dayOfWeek);
    public function inDaylightTime();
    public function isEquivalentTo(IntlCalendar $calendar);
    public function isLenient();
    public function isWeekend($date = null);
    public function roll($field, $amountOrUpOrDown);
    public function set($year, $month, $dayOfMonth = null, $hour = null, $minute = null, $second = null);
    public function setFirstDayOfWeek($dayOfWeek);
    public function setLenient($isLenient);
    public function setRepeatedWallTimeOption($wallTimeOption);
    public function setSkippedWallTimeOption($wallTimeOption);
    public function setTime($date);
    public function setTimeZone($timeZone);
    public function toDateTime();
}

class IntlIterator implements Iterator {
}

class IntlTimeZone {
    const DISPLAY_SHORT = 1;
    const DISPLAY_LONG = 2;
    public static function countEquivalentIDs($zoneId);
    public function createDefault();
    public static function createEnumeration($countryOrRawOffset);
    public static function createTimeZone($zoneId);
    public static function fromDateTimeZone($zoneId);
    public static function getCanonicalID($zoneId, &$isSystemID);
    public function getDisplayName($isDaylight, $style, $locale);
    public function getDSTSavings();
    public static function getEquivalentID($zoneId, $index);
    public function getErrorCode();
    public function getErrorMessage();
    public static function getGMT();
    public function getID();
    public function getOffset($date, $local, &$rawOffset, &$dstOffset);
    public function getRawOffset();
    public static function getTZDataVersion();
    public function hasSameRules(IntlTimeZone $otherTimeZone);
    public function toDateTimeZone();
    public function useDaylightTime();
}

class IntlBreakIterator implements Traversable {
    const DONE = -1;
    const WORD_NONE = 0;
    const WORD_NONE_LIMIT = 100;
    const WORD_NUMBER = 100;
    const WORD_NUMBER_LIMIT = 200;
    const WORD_LETTER = 200;
    const WORD_LETTER_LIMIT = 300;
    const WORD_KANA = 300;
    const WORD_KANA_LIMIT = 400;
    const WORD_IDEO = 400;
    const WORD_IDEO_LIMIT = 500;
    const LINE_SOFT = 0;
    const LINE_SOFT_LIMIT = 100;
    const LINE_HARD = 100;
    const LINE_HARD_LIMIT = 200;
    const SENTENCE_TERM = 0;
    const SENTENCE_TERM_LIMIT = 100;
    const SENTENCE_SEP = 100;
    const SENTENCE_SEP_LIMIT = 200;
    private function __construct();
    public static function createCharacterInstance($locale);
    public static function createCodePointInstance();
    public static function createLineInstance($locale);
    public static function createSentenceInstance($locale);
    public static function createTitleInstance($locale);
    public static function createWordInstance($locale);
    public function current();
    public function first();
    public function following($offset);
    public function getErrorCode();
    public function getErrorMessage();
    public function getLocale($locale_type);
    public function getPartsIterator($key_type);
    public function getText();
    public function isBoundary($offset);
    public function last();
    public function next($offset);
    public function preceding($offset);
    public function previous();
    public function setText($text);
}

class IntlRuleBasedBreakIterator extends IntlBreakIterator implements Traversable {
    public function __construct($rules, $areCompiled);
    public static function createCharacterInstance($locale);
    public static function createCodePointInstance();
    public static function createLineInstance($locale);
    public static function createSentenceInstance($locale);
    public static function createTitleInstance($locale);
    public static function createWordInstance($locale);
    public function getBinaryRules();
    public function getRules();
    public function getRuleStatus();
    public function getRuleStatusVec();
}

class IntlCodePointBreakIterator extends IntlBreakIterator implements Traversable {
    public function getLastCodePoint();
}
