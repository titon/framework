<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;
use Titon\Utility\Exception\InvalidArgumentException;
use Titon\Utility\Exception\InvalidCreditCardException;
use \Indexish;

/**
 * Validate provides methods for validating data against specific conditions. This should not be used to validate formatting (excluding a few),
 * but should allow validation of data before the formatting of data.
 *
 * @package Titon\Utility
 */
class Validate {
    use Macroable;

    /**
     * IP version constants.
     */
    const int IPV4 = FILTER_FLAG_IPV4;
    const int IPV6 = FILTER_FLAG_IPV6;

    /**
     * Credit card constants.
     */
    const string AMERICAN_EXPRESS = 'americanExpress';
    const string BANKCARD = 'bankcard';
    const string DINERS_CLUB = 'diners';
    const string DISCOVER = 'discover';
    const string ENROUTE = 'enroute';
    const string JCB = 'jcb';
    const string MAESTRO = 'maestro';
    const string MASTERCARD = 'mastercard';
    const string SOLO_DEBIT = 'solo';
    const string SWITCH_DEBIT = 'switch';
    const string VISA = 'visa';
    const string VISA_ELECTRON = 'electron';
    const string VOYAGER = 'voyager';

    /**
     * Validate input is alphabetical.
     *
     * @param string $input
     * @param string $exceptions
     * @return bool
     */
    public static function alpha(string $input, string $exceptions = ''): bool {
        return static::custom($input, '/^[\p{L}\s' . static::escape($exceptions) . ']+$/imU');
    }

    /**
     * Validate input is numerical and alphabetical (does not include punctuation).
     *
     * @param string $input
     * @param string $exceptions
     * @return bool
     */
    public static function alphaNumeric(string $input, string $exceptions = ''): bool {
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
    public static function between(string $input, int $min, int $max): bool {
        $length = mb_strlen($input);

        return ($length <= $max && $length >= $min);
    }

    /**
     * Validate input is a bool or bool-like flag.
     *
     * @param string $input
     * @return bool
     */
    public static function boolean(mixed $input): bool {
        return in_array($input, [true, false, 1, 0, '1', '0', 'on', 'off', 'yes', 'no'], true);
    }

    /**
     * Validate input with a custom callback function.
     *
     * @param string $input
     * @param (function(string): bool) $callback
     * @return bool
     */
    public static function callback(string $input, (function(string): bool) $callback): bool {
        return $callback($input);
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
    public static function comparison(int $input, int $check, string $mode): bool {
        switch (strtolower($mode)) {
            case 'gt':
            case '>':
                return ($input > $check);
            break;
            case 'gte':
            case '>=':
                return ($input >= $check);
            break;
            case 'lt':
            case '<':
                return ($input < $check);
            break;
            case 'lte':
            case '<=':
                return ($input <= $check);
            break;
            case 'eq':
            case '==':
            case '=':
                return ($input == $check);
            break;
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
     * @param string|Vector<string> $types
     * @return bool
     * @throws \Titon\Utility\Exception\InvalidCreditCardException
     */
    public static function creditCard(string $input, mixed $types = ''): bool {
        $input = str_replace(['-', ' '], '', $input);

        if (mb_strlen($input) < 13) {
            return false;
        }

        $validate = Map {};
        $cards = Map{
            self::AMERICAN_EXPRESS  => '/^3[4|7]\\d{13}$/',
            self::BANKCARD          => '/^56(10\\d\\d|022[1-5])\\d{10}$/',
            self::DISCOVER          => '/^(?:6011|650\\d)\\d{12}$/',
            self::ENROUTE           => '/^2(?:014|149)\\d{11}$/',
            self::JCB               => '/^(3\\d{4}|2100|1800)\\d{11}$/',
            self::MAESTRO           => '/^(?:5020|6\\d{3})\\d{12}$/',
            self::MASTERCARD        => '/^5[1-5]\\d{14}$/',
            self::DINERS_CLUB       => '/^(?:3(0[0-5]|[68]\\d)\\d{11})|(?:5[1-5]\\d{14})$/',
            self::SOLO_DEBIT        => '/^(6334[5-9][0-9]|6767[0-9]{2})\\d{10}(\\d{2,3})?$/',
            self::SWITCH_DEBIT      => '/^(?:49(03(0[2-9]|3[5-9])|11(0[1-2]|7[4-9]|8[1-2])|36[0-9]{2})\\d{10}(\\d{2,3})?)|(?:564182\\d{10}(\\d{2,3})?)|(6(3(33[0-4][0-9])|759[0-9]{2})\\d{10}(\\d{2,3})?)$/',
            self::VISA              => '/^4\\d{12}(\\d{3})?$/',
            self::VISA_ELECTRON     => '/^(?:417500|4917\\d{2}|4913\\d{2})\\d{10}$/',
            self::VOYAGER           => '/^8699[0-9]{11}$/'
        };

        if ($types) {
            if (!$types instanceof Vector) {
                $types = new Vector(explode('|', (string) $types));
            }

            foreach ($types as $card) {
                if ($cards->contains($card)) {
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
    public static function currency(string $input, string $format = '/^\$[0-9,]+(?:\.[0-9]{2})?$/'): bool {
        return static::custom($input, $format);
    }

    /**
     * Validate input against a custom regex pattern.
     *
     * @param string $input
     * @param string $expression
     * @return bool
     */
    public static function custom(string $input, string $expression): bool {
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
    public static function date(mixed $input): bool {
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
    public static function decimal(string $input, int $places = 2): bool {
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
     * @param mixed $input
     * @param string $type
     * @param int $size
     * @return bool
     */
    public static function dimensions(mixed $input, string $type, int $size): bool {
        if ($input instanceof Indexish && static::file($input)) {
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
    public static function email(string $input, bool $dns = true): bool {
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
    public static function equal(mixed $input, mixed $check): bool {
        return ($input == $check);
    }

    /**
     * Validate two types match exactly.
     *
     * @param string $input
     * @param string $check
     * @return bool
     */
    public static function exact(mixed $input, mixed $check): bool {
        return ($input === $check);
    }

    /**
     * Escapes characters that would break the regex.
     *
     * @param string $characters
     * @return string
     */
    public static function escape(string $characters): string {
        return preg_quote($characters, '/');
    }

    /**
     * Validate input has an extension and is in the whitelist.
     *
     * @uses Titon\Utility\Loader
     *
     * @param string $input
     * @param Vector<string> $extensions
     * @return bool
     */
    public static function ext(mixed $input, Vector<string> $extensions = Vector {'gif', 'jpeg', 'png', 'jpg'}): bool {
        if (is_array($input) && array_key_exists('name', $input)) {
            $input = $input['name'];
        }

        return in_array(Path::ext((string) $input), $extensions, true);
    }

    /**
     * Validate input is a file upload by checking for tmp_name and verifying error.
     *
     * @param mixed $input
     * @return bool
     */
    public static function file(mixed $input): bool {
        return (is_array($input) && array_key_exists('tmp_name', $input) && $input['error'] == 0);
    }

    /**
     * Check if a rule exists either as a class method or custom macro.
     *
     * @param string $method
     * @return bool
     */
    public static function hasRule(string $method): bool {
        return (method_exists(static::class, $method) || static::hasMacro($method));
    }

    /**
     * Validate an images height is exact.
     *
     * @param mixed $input
     * @param int $size
     * @return bool
     */
    public static function height(mixed $input, int $size = 0): bool {
        return static::dimensions($input, 'height', $size);
    }

    /**
     * Validate input is in the list.
     *
     * @param string $input
     * @param array $list
     * @return bool
     */
    public static function inList(mixed $input, Vector<mixed> $list): bool {
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
    public static function inRange(int $input, int $min, int $max): bool {
        return ($input <= $max && $input >= $min);
    }

    /**
     * Validate input is an IP address. Optional $mode can be passed to flag as IP v4 or v6.
     *
     * @param string $input
     * @param int $flags
     * @return bool
     */
    public static function ip(string $input, int $flags = 0): bool {
        return (bool) filter_var($input, FILTER_VALIDATE_IP, $flags);
    }

    /**
     * Luhn algorithm.
     *
     * @param string $input
     * @return bool
     * @link http://en.wikipedia.org/wiki/Luhn_algorithm
     */
    public static function luhn(string $input): bool {
        if (!$input) {
            return false;
        }

        $sum = 0;
        $length = mb_strlen($input);

        for ($position = 1 - ($length % 2); $position < $length; $position += 2) {
            $sum += (int) $input[$position];
        }

        for ($position = ($length % 2); $position < $length; $position += 2) {
            $number = (int) $input[$position] * 2;
            $sum += ($number < 10) ? $number : $number - 9;
        }

        return ($sum % 10 == 0);
    }

    /**
     * Validate a files mime type is in the whitelist.
     *
     * @param string $input
     * @param Vector<string> $mimes
     * @return bool
     */
    public static function mimeType(mixed $input, Vector<string> $mimes): bool {
        if ($input instanceof Indexish && static::file($input)) {
            $input = $input['tmp_name'];
        }

        if (!file_exists($input)) {
            return false;
        }

        $file = finfo_open(FILEINFO_MIME_TYPE);
        $type = finfo_file($file, $input);
        finfo_close($file);

        return in_array($type, $mimes);
    }

    /**
     * Validate an images file size is above the minimum.
     *
     * @param mixed $input
     * @param int $min
     * @return bool
     */
    public static function minFilesize(mixed $input, int $min): bool {
        if ($input instanceof Indexish && static::file($input)) {
            $size = $input['size'];

        } else if (file_exists($input)) {
            $size = filesize($input);

        } else {
            return false;
        }

        return ($size >= Number::bytesFrom((string) $min));
    }

    /**
     * Validate an images height is above the minimum.
     *
     * @param mixed $input
     * @param int $min
     * @return bool
     */
    public static function minHeight(mixed $input, int $min): bool {
        return static::dimensions($input, 'minHeight', $min);
    }

    /**
     * Validate input length has a minimum amount of characters.
     *
     * @param string $input
     * @param int $min
     * @return bool
     */
    public static function minLength(mixed $input, int $min): bool {
        return (mb_strlen($input) >= $min);
    }

    /**
     * Validate an images width is above the minimum.
     *
     * @param mixed $input
     * @param int $min
     * @return bool
     */
    public static function minWidth(mixed $input, int $min): bool {
        return static::dimensions($input, 'minWidth', $min);
    }

    /**
     * Validate an images file size is below the maximum.
     *
     * @uses Titon\Utility\Number
     *
     * @param mixed $input
     * @param int $max
     * @return bool
     */
    public static function maxFilesize(mixed $input, int $max): bool {
        if ($input instanceof Indexish && static::file($input)) {
            $size = $input['size'];

        } else if (file_exists($input)) {
            $size = filesize($input);

        } else {
            return false;
        }

        return ($size <= Number::bytesFrom((string) $max));
    }

    /**
     * Validate an images height is below the maximum.
     *
     * @param mixed $input
     * @param int $max
     * @return bool
     */
    public static function maxHeight(mixed $input, int $max): bool {
        return static::dimensions($input, 'maxHeight', $max);
    }

    /**
     * Validate input length has a maximum amount of characters.
     *
     * @param string $input
     * @param int $max
     * @return bool
     */
    public static function maxLength(string $input, int $max): bool {
        return (mb_strlen($input) <= $max);
    }

    /**
     * Validate an images width is below the maximum.
     *
     * @param mixed $input
     * @param int $max
     * @return bool
     */
    public static function maxWidth(mixed $input, int $max): bool {
        return static::dimensions($input, 'maxWidth', $max);
    }

    /**
     * Validate input is not empty; zero's are not flagged as empty.
     *
     * @param string $input
     * @return bool
     */
    public static function notEmpty(mixed $input): bool {
        return ($input || $input === 0 || $input === '0');
    }

    /**
     * Validate input is numeric.
     *
     * @param string $input
     * @return bool
     */
    public static function numeric(mixed $input): bool {
        return is_numeric($input);
    }

    /**
     * Validate input matches a phone number format.
     *
     * @param string $input
     * @param string $format
     * @return bool
     */
    public static function phone(string $input, string $format = '/^(?:\+?1)?\s?(?:\([0-9]{3}\))?\s?[0-9]{3}-[0-9]{4}$/'): bool {
        return static::custom($input, $format);
    }

    /**
     * Validate input matches a postal/zip code format.
     *
     * @param string $input
     * @param string $format
     * @return bool
     */
    public static function postalCode(string $input, string $format = '/^[0-9]{5}(?:-[0-9]{4})?$/'): bool {
        return static::custom($input, $format);
    }

    /**
     * Validate input matches a social security number (SSN) format.
     *
     * @param string $input
     * @param string $format
     * @return bool
     */
    public static function ssn(string $input, string $format = '/^[0-9]{3}-[0-9]{2}-[0-9]{4}$/'): bool {
        return static::custom($input, $format);
    }

    /**
     * Validate input is a UUID.
     *
     * @param string $input
     * @return bool
     */
    public static function uuid(string $input): bool {
        return static::custom($input, '/^[a-f0-9]{8}\-[a-f0-9]{4}\-[a-f0-9]{4}\-[a-f0-9]{4}\-[a-f0-9]{12}$/i');
    }

    /**
     * Validate input is a URL / website address.
     *
     * @param string $input
     * @return bool
     */
    public static function url(string $input): bool {
        return (bool) filter_var($input, FILTER_VALIDATE_URL);
    }

    /**
     * Validate an images width is exact.
     *
     * @param mixed $input
     * @param int $size
     * @return bool
     */
    public static function width(mixed $input, int $size): bool {
        return static::dimensions($input, 'width', $size);
    }

}