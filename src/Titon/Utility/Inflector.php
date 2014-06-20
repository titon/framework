<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

/**
 * String and grammar inflection. Converts strings to a certain format. Camel cased, singular, plural etc.
 *
 * @package Titon\Utility
 */
class Inflector extends Macro {

    /**
     * Inflect a word to a camel case form with the first letter being capitalized.
     *
     * @param string $string
     * @return string
     */
    public static function camelCase($string) {
        return static::cache(array(__METHOD__, $string), function() use ($string) {
            return str_replace(' ', '', mb_convert_case(str_replace(array('_', '-'), ' ', preg_replace('/[^-_a-z0-9\s]+/i', '', $string)), MB_CASE_TITLE));
        });
    }

    /**
     * Inflect a word to a class name. Singular camel cased form.
     *
     * @param string $string
     * @return string
     */
    public static function className($string) {
        return static::cache(array(__METHOD__, $string), function() use ($string) {
            return Inflector::camelCase(Inflector::singularize($string));
        });
    }

    /**
     * Inflect a word for a filename. Studly cased and capitalized.
     *
     * @param string $string
     * @param string $ext
     * @param bool $capitalize
     * @return string
     */
    public static function fileName($string, $ext = 'php', $capitalize = true) {
        if (mb_strpos($string, '.') !== false) {
            $string = mb_substr($string, 0, mb_strrpos($string, '.'));
        }

        $path = static::camelCase($string);

        if (!$capitalize) {
            $path = lcfirst($path);
        }

        if (mb_substr($path, -(mb_strlen($ext) + 1)) !== '.' . $ext) {
            $path .= '.' . $ext;
        }

        return $path;
    }

    /**
     * Inflect a word by replacing spaces with dashes.
     *
     * @param string $string
     * @return string
     */
    public static function hyphenate($string) {
        return static::cache(array(__METHOD__, $string), function() use ($string) {
            return str_replace(' ', '-', preg_replace('/\s{2,}+/', ' ', $string));
        });
    }

    /**
     * Inflect a word to a human readable string with only the first word capitalized and the rest lowercased.
     *
     * @param string $string
     * @return string
     */
    public static function normalCase($string) {
        return static::cache(array(__METHOD__, $string), function() use ($string) {
            return ucfirst(mb_strtolower(str_replace('_', ' ', $string)));
        });
    }

    /**
     * Inflect a number by appending its ordinal suffix: st, nd, rd, th, etc.
     *
     * @param int $number
     * @return string
     */
    public static function ordinal($number) {
        return $number;
    }

    /**
     * Inflect a string to its pluralized form.
     *
     * @param string $string
     * @return string
     */
    public static function pluralize($string) {
        return $string;
    }

    /**
     * Inflect a word to a routeable format. All non-alphanumeric characters will be removed, and any spaces or underscores will be changed to dashes.
     *
     * @param string $string
     * @return string
     */
    public static function route($string) {
        return static::cache(array(__METHOD__, $string), function() use ($string) {
            return mb_strtolower(Inflector::hyphenate(str_replace('_', '-', preg_replace('/[^-_a-z0-9\s\.]+/i', '', $string))));
        });
    }

    /**
     * Inflect a string to its singular form.
     *
     * @param string $string
     * @return string
     */
    public static function singularize($string) {
        return $string;
    }

    /**
     * Inflect a word to a URL friendly slug. Removes all punctuation, replaces dashes with underscores and spaces with dashes.
     *
     * @param string $string
     * @return string
     */
    public static function slug($string) {
        return static::cache(array(__METHOD__, $string), function() use ($string) {
            // Revert entities
            $string = html_entity_decode($string, ENT_QUOTES, 'UTF-8');

            // Remove non-ascii characters
            $string = preg_replace('/[^-a-z0-9\.\s]+/i', '', Inflector::transliterate($string));

            // Replace dashes and underscores
            $string = str_replace(' ', '-', str_replace('-', '_', $string));

            return mb_strtolower($string);
        });
    }

    /**
     * Alias for underscore.
     *
     * @param string $string
     * @return string
     */
    public static function snakeCase($string) {
        return static::underscore($string);
    }

    /**
     * Inflect a word for a database table name. Formatted as plural and camel case with the first letter lowercase.
     *
     * @param string $string
     * @return string
     */
    public static function tableName($string) {
        return static::cache(array(__METHOD__, $string), function() use ($string) {
            return lcfirst(Inflector::camelCase(Inflector::pluralize($string)));
        });
    }

    /**
     * Inflect a word to a human readable string with all words capitalized.
     *
     * @param string $string
     * @return string
     */
    public static function titleCase($string) {
        return static::cache(array(__METHOD__, $string), function() use ($string) {
            return mb_convert_case(str_replace('_', ' ', $string), MB_CASE_TITLE);
        });
    }

    /**
     * Inflect a word by replacing all non-ASCII characters with there equivalents.
     *
     * @param string $string
     * @return string
     */
    public static function transliterate($string) {
        return $string;
    }

    /**
     * Inflect a word to an underscore form that strips all punctuation and special characters and converts spaces to underscores.
     *
     * @param string $string
     * @return string
     */
    public static function underscore($string) {
        return static::cache(array(__METHOD__, $string), function() use ($string) {
            return trim(mb_strtolower(str_replace('__', '_', preg_replace('/([A-Z]{1})/', '_$1', preg_replace('/[^_a-z0-9]+/i', '', preg_replace('/[\s]+/', '_', $string))))), '_');
        });
    }

    /**
     * Inflect a word to be used as a PHP variable. Strip all but letters, numbers and underscores. Add an underscore if the first letter is numeric.
     *
     * @param string $string
     * @return string
     */
    public static function variable($string) {
        $string = preg_replace('/[^_a-z0-9]+/i', '', $string);

        if (is_numeric(mb_substr($string, 0, 1))) {
            $string = '_' . $string;
        }

        return $string;
    }

}