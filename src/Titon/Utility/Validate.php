<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Utility\Exception\InvalidArgumentException;
use Titon\Utility\Exception\InvalidCreditCardException;
use \Closure;

/**
 * Validate provides methods for validating data against specific conditions. This should not be used to validate formatting (excluding a few),
 * but should allow validation of data before the formatting of data.
 *
 * @package Titon\Utility
 */
class Validate extends Macro {

    /**
     * IP version constants.
     */
    const IPV4 = FILTER_FLAG_IPV4;
    const IPV6 = FILTER_FLAG_IPV6;

    /**
     * Credit card constants.
     */
    const AMERICAN_EXPRESS = 'americanExpress';
    const BANKCARD = 'bankcard';
    const DINERS_CLUB = 'diners';
    const DISCOVER = 'discover';
    const ENROUTE = 'enroute';
    const JCB = 'jcb';
    const MAESTRO = 'maestro';
    const MASTERCARD = 'mastercard';
    const SOLO_DEBIT = 'solo';
    const SWITCH_DEBIT = 'switch';
    const VISA = 'visa';
    const VISA_ELECTRON = 'electron';
    const VOYAGER = 'voyager';

    /**
     * Validate input is alphabetical.
     *
     * @param string $input
     * @param array $exceptions
     * @return bool
     */
    public static function alpha($input, $exceptions = array()) {
        return static::custom($input, '/^[\p{L}\s' . static::escape($exceptions) . ']+$/imU');
    }

    /**
     * Validate input is numerical and alphabetical (does not include punctuation).
     *
     * @param string $input
     * @param array $exceptions
     * @return bool
     */
    public static function alphaNumeric($input, $exceptions = array()) {
        return static::custom($input, '/^[\p{L}\p{N}\p{Nd}\s' . static::escape($exceptions) . ']+$/imU');
    }

    /**
     * Validate input string length is between the min and max.
     *
     * @param string $input
     * @param int $min
     * @param int $max
     * @return bool
     */
    public static function between($input, $min, $max) {
        $length = mb_strlen($input);

        return ($length <= $max && $length >= $min);
    }

    /**
     * Validate input is a bool or bool-like flag.
     *
     * @param string $input
     * @return bool
     */
    public static function boolean($input) {
        return in_array($input, array(true, false, 1, 0, '1', '0', 'on', 'off', 'yes', 'no'), true);
    }

    /**
     * Validate input with a custom callback function.
     *
     * @param string $input
     * @param \Closure $callback
     * @return bool
     */
    public static function callback($input, Closure $callback) {
        return (bool) $callback($input);
    }

    /**
     * Compare two numerical values.
     *
     * @param int $input
     * @param int $check
     * @param string $mode
     * @return bool
     * @throws \Titon\Utility\Exception\InvalidArgumentException
     */
    public static function comparison($input, $check, $mode) {
        switch (mb_strtolower($mode)) {
            case 'greater':
            case 'gt':
            case '>':
                return ($input > $check);
            break;
            case 'greaterorequal':
            case 'gte':
            case '>=':
                return ($input >= $check);
            break;
            case 'less':
            case 'lt':
            case '<':
                return ($input < $check);
            break;
            case 'lessorequal':
            case 'lte':
            case '<=':
                return ($input <= $check);
            break;
            case 'equal':
            case 'eq':
            case '==':
            case '=':
                return ($input == $check);
            break;
            case 'notequal':
            case 'neq':
            case 'ne':
            case '!=':
                return ($input != $check);
            break;
            default:
                throw new InvalidArgumentException(sprintf('Unsupported mode %s for %s', $mode, __METHOD__));
            break;
        }
    }

    /**
     * Validate input is a credit card number. If $types is defined, will only validate against those cards, else will validate against all.
     *
     * @param string $input
     * @param string|array $types
     * @return bool
     * @throws \Titon\Utility\Exception\InvalidCreditCardException
     */
    public static function creditCard($input, $types = null) {
        $input = str_replace(array('-', ' '), '', $input);

        if (mb_strlen($input) < 13) {
            return false;
        }

        $cards = array(
            static::AMERICAN_EXPRESS  => '/^3[4|7]\\d{13}$/',
            static::BANKCARD          => '/^56(10\\d\\d|022[1-5])\\d{10}$/',
            static::DISCOVER          => '/^(?:6011|650\\d)\\d{12}$/',
            static::ENROUTE           => '/^2(?:014|149)\\d{11}$/',
            static::JCB               => '/^(3\\d{4}|2100|1800)\\d{11}$/',
            static::MAESTRO           => '/^(?:5020|6\\d{3})\\d{12}$/',
            static::MASTERCARD        => '/^5[1-5]\\d{14}$/',
            static::DINERS_CLUB       => '/^(?:3(0[0-5]|[68]\\d)\\d{11})|(?:5[1-5]\\d{14})$/',
            static::SOLO_DEBIT        => '/^(6334[5-9][0-9]|6767[0-9]{2})\\d{10}(\\d{2,3})?$/',
            static::SWITCH_DEBIT      => '/^(?:49(03(0[2-9]|3[5-9])|11(0[1-2]|7[4-9]|8[1-2])|36[0-9]{2})\\d{10}(\\d{2,3})?)|(?:564182\\d{10}(\\d{2,3})?)|(6(3(33[0-4][0-9])|759[0-9]{2})\\d{10}(\\d{2,3})?)$/',
            static::VISA              => '/^4\\d{12}(\\d{3})?$/',
            static::VISA_ELECTRON     => '/^(?:417500|4917\\d{2}|4913\\d{2})\\d{10}$/',
            static::VOYAGER           => '/^8699[0-9]{11}$/'
        );

        if ($types) {
            $validate = array();

            foreach ((array) $types as $card) {
                if (isset($cards[$card])) {
                    $validate[$card] = $cards[$card];
                } else {
                    throw new InvalidCreditCardException(sprintf('Credit card type %s does not exist', $card));
                }
            }
        } else {
            $validate = $cards;
        }

        foreach ($validate as $pattern) {
            if (preg_match($pattern, $input)) {
                return static::luhn($input);
            }
        }

        return false;
    }

    /**
     * Validate input matches a currency format.
     *
     * @param string $input
     * @param string $format
     * @return bool
     */
    public static function currency($input, $format = '/^\$[0-9,]+(?:\.[0-9]{2})?$/') {
        return static::custom($input, $format);
    }

    /**
     * Validate input against a custom regex pattern.
     *
     * @param string $input
     * @param string $expression
     * @return bool
     */
    public static function custom($input, $expression) {
        return (bool) preg_match($expression, $input);
    }

    /**
     * Validate input is a real date.
     *
     * @uses Titon\Utility\Time
     *
     * @param string $input
     * @return bool
     */
    public static function date($input) {
        $input = Time::toUnix($input);

        if (!$input) {
            return false;
        }

        list($m, $d, $y) = explode('/', date('m/d/Y', $input));

        return checkdate($m, $d, $y);
    }

    /**
     * Validate input is a decimal.
     *
     * @param string $input
     * @param int $places
     * @return bool
     */
    public static function decimal($input, $places = 2) {
        if (!$places) {
            $regex = '/^[-+]?[0-9]*\.{1}[0-9]+(?:[eE][-+]?[0-9]+)?$/';
        } else {
            $regex = '/^[-+]?[0-9]*\.{1}[0-9]{' . $places . '}$/';
        }

        return static::custom($input, $regex);
    }

    /**
     * Validate an images dimensions.
     *
     * @param array $input
     * @param string $type
     * @param int $size
     * @return bool
     */
    public static function dimensions($input, $type, $size) {
        if (static::file($input)) {
            $input = $input['tmp_name'];
        }

        if (!file_exists($input)) {
            return false;
        }

        if ($file = getimagesize($input)) {
            $width = $file[0];
            $height = $file[1];

            switch ($type) {
                case 'width':       return ($width == $size);
                case 'height':      return ($height == $size);
                case 'maxWidth':    return ($width <= $size);
                case 'maxHeight':   return ($height <= $size);
                case 'minWidth':    return ($width >= $size);
                case 'minHeight':   return ($height >= $size);
            }
        }

        return false;
    }

    /**
     * Validate input is an email. If $dns is true, will check DNS records as well.
     *
     * @param string $input
     * @param bool $dns
     * @return bool
     */
    public static function email($input, $dns = true) {
        $result = (bool) filter_var($input, FILTER_VALIDATE_EMAIL);

        if (!$result) {
            return false;
        }

        if ($dns) {
            $host = trim(mb_strstr(filter_var($input, FILTER_SANITIZE_EMAIL), '@'), '@');

            if (function_exists('checkdnsrr') && checkdnsrr($host, 'MX')) {
                return true;
            }

            return is_array(gethostbynamel($host));
        }

        return $result;
    }

    /**
     * Validate two values are equal.
     *
     * @param string $input
     * @param string $check
     * @return bool
     */
    public static function equal($input, $check) {
        return ($input == $check);
    }

    /**
     * Validate two types match exactly.
     *
     * @param string $input
     * @param string $check
     * @return bool
     */
    public static function exact($input, $check) {
        return ($input === $check);
    }

    /**
     * Escapes characters that would break the regex.
     *
     * @param array|string $characters
     * @return string
     */
    public static function escape($characters) {
        if (is_array($characters)) {
            $characters = implode('', $characters);
        }

        return preg_quote($characters, '/');
    }

    /**
     * Validate input has an extension and is in the whitelist.
     *
     * @uses Titon\Utility\Loader
     *
     * @param string $input
     * @param string|array $extensions
     * @return bool
     */
    public static function ext($input, $extensions = array('gif', 'jpeg', 'png', 'jpg')) {
        if (is_array($input) && isset($input['name'])) {
            $input = $input['name'];
        }

        return in_array(Path::ext($input), (array) $extensions, true);
    }

    /**
     * Validate input is a file upload by checking for tmp_name and verifying error.
     *
     * @param string $input
     * @return bool
     */
    public static function file($input) {
        return (is_array($input) && !empty($input['tmp_name']) && $input['error'] == 0);
    }

    /**
     * Validate an images height is exact.
     *
     * @param array $input
     * @param int $size
     * @return bool
     */
    public static function height($input, $size = 0) {
        return static::dimensions($input, 'height', $size);
    }

    /**
     * Validate input is in the list.
     *
     * @param string $input
     * @param array $list
     * @return bool
     */
    public static function inList($input, array $list) {
        return in_array($input, $list, true);
    }

    /**
     * Validate input is within the min and max range.
     *
     * @param string $input
     * @param int $min
     * @param int $max
     * @return bool
     */
    public static function inRange($input, $min, $max) {
        return ($input <= $max && $input >= $min);
    }

    /**
     * Validate input is an IP address. Optional $mode can be passed to flag as IP v4 or v6.
     *
     * @param string $input
     * @param int $flags
     * @return bool
     */
    public static function ip($input, $flags = 0) {
        return (bool) filter_var($input, FILTER_VALIDATE_IP, $flags);
    }

    /**
     * Luhn algorithm.
     *
     * @param string $input
     * @return bool
     * @link http://en.wikipedia.org/wiki/Luhn_algorithm
     */
    public static function luhn($input) {
        if ($input == 0) {
            return false;
        }

        $sum = 0;
        $length = mb_strlen($input);

        for ($position = 1 - ($length % 2); $position < $length; $position += 2) {
            $sum += $input[$position];
        }

        for ($position = ($length % 2); $position < $length; $position += 2) {
            $number = $input[$position] * 2;
            $sum += ($number < 10) ? $number : $number - 9;
        }

        return ($sum % 10 == 0);
    }

    /**
     * Validate a files mime type is in the whitelist.
     *
     * @param string $input
     * @param string|array $mimes
     * @return bool
     */
    public static function mimeType($input, $mimes) {
        if (static::file($input)) {
            $input = $input['tmp_name'];
        }

        if (!file_exists($input)) {
            return false;
        }

        $file = finfo_open(FILEINFO_MIME_TYPE);
        $type = finfo_file($file, $input);
        finfo_close($file);

        return in_array($type, (array) $mimes);
    }

    /**
     * Validate an images file size is above the minimum.
     *
     * @param array $input
     * @param int $min
     * @return bool
     */
    public static function minFilesize($input, $min) {
        if (static::file($input)) {
            $size = $input['size'];

        } else if (file_exists($input)) {
            $size = filesize($input);

        } else {
            return false;
        }

        return ($size >= Number::bytesFrom($min));
    }

    /**
     * Validate an images height is above the minimum.
     *
     * @param array $input
     * @param int $min
     * @return bool
     */
    public static function minHeight($input, $min) {
        return static::dimensions($input, 'minHeight', $min);
    }

    /**
     * Validate input length has a minimum amount of characters.
     *
     * @param string $input
     * @param int $min
     * @return bool
     */
    public static function minLength($input, $min) {
        return (mb_strlen($input) >= $min);
    }

    /**
     * Validate an images width is above the minimum.
     *
     * @param array $input
     * @param int $min
     * @return bool
     */
    public static function minWidth($input, $min) {
        return static::dimensions($input, 'minWidth', $min);
    }

    /**
     * Validate an images file size is below the maximum.
     *
     * @uses Titon\Utility\Number
     *
     * @param array $input
     * @param int $max
     * @return bool
     */
    public static function maxFilesize($input, $max) {
        if (static::file($input)) {
            $size = $input['size'];

        } else if (file_exists($input)) {
            $size = filesize($input);

        } else {
            return false;
        }

        return ($size <= Number::bytesFrom($max));
    }

    /**
     * Validate an images height is below the maximum.
     *
     * @param array $input
     * @param int $max
     * @return bool
     */
    public static function maxHeight($input, $max) {
        return static::dimensions($input, 'maxHeight', $max);
    }

    /**
     * Validate input length has a maximum amount of characters.
     *
     * @param string $input
     * @param int $max
     * @return bool
     */
    public static function maxLength($input, $max) {
        return (mb_strlen($input) <= $max);
    }

    /**
     * Validate an images width is below the maximum.
     *
     * @param array $input
     * @param int $max
     * @return bool
     */
    public static function maxWidth($input, $max) {
        return static::dimensions($input, 'maxWidth', $max);
    }

    /**
     * Validate input is not empty; zero's are not flagged as empty.
     *
     * @param string $input
     * @return bool
     */
    public static function notEmpty($input) {
        return (!empty($input) || $input === 0 || $input === '0');
    }

    /**
     * Validate input is numeric.
     *
     * @param string $input
     * @return bool
     */
    public static function numeric($input) {
        return is_numeric($input);
    }

    /**
     * Validate input matches a phone number format.
     *
     * @param string $input
     * @param string $format
     * @return bool
     */
    public static function phone($input, $format = '/^(?:\+?1)?\s?(?:\([0-9]{3}\))?\s?[0-9]{3}-[0-9]{4}$/') {
        return static::custom($input, $format);
    }

    /**
     * Validate input matches a postal/zip code format.
     *
     * @param string $input
     * @param string $format
     * @return bool
     */
    public static function postalCode($input, $format = '/^[0-9]{5}(?:-[0-9]{4})?$/') {
        return static::custom($input, $format);
    }

    /**
     * Validate input matches a social security number (SSN) format.
     *
     * @param string $input
     * @param string $format
     * @return bool
     */
    public static function ssn($input, $format = '/^[0-9]{3}-[0-9]{2}-[0-9]{4}$/') {
        return static::custom($input, $format);
    }

    /**
     * Validate input is a UUID.
     *
     * @param string $input
     * @return bool
     */
    public static function uuid($input) {
        return static::custom($input, '/^[a-f0-9]{8}\-[a-f0-9]{4}\-[a-f0-9]{4}\-[a-f0-9]{4}\-[a-f0-9]{12}$/i');
    }

    /**
     * Validate input is a URL / website address.
     *
     * @param string $input
     * @return bool
     */
    public static function url($input) {
        return (bool) filter_var($input, FILTER_VALIDATE_URL);
    }

    /**
     * Validate an images width is exact.
     *
     * @param array $input
     * @param int $size
     * @return bool
     */
    public static function width($input, $size) {
        return static::dimensions($input, 'width', $size);
    }

}