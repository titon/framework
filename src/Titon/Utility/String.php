<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

/**
 * Specific methods that deal with string manipulation, truncation, formation, etc.
 *
 * @package Titon\Utility
 */
class String extends Macro {

    /**
     * Generator types.
     */
    const ALPHA = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    const ALPHA_LOWER = 'abcdefghijklmnopqrstuvwxyz';
    const ALPHA_UPPER = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
    const NUMERIC = '0123456789';
    const NUMERIC_NOZERO = '123456789';
    const NUMERIC_EVEN = '02468';
    const NUMERIC_ODD = '13579';
    const ALNUM = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
    const HEX = '0123456789abcdef';

    /**
     * Return the character at the specified index, if not found returns null.
     *
     * @param string $string
     * @param int $index
     * @return string
     */
    public static function charAt($string, $index) {
        return isset($string[$index]) ? $string[$index] : null;
    }

    /**
     * Compares to strings alphabetically. Returns 0 if they are equal, negative if passed value is greater, or positive if current value is greater.
     *
     * @param string $string
     * @param string $value
     * @param int $length
     * @return int
     */
    public static function compare($string, $value, $length = 0) {
        $string = (string) $string;
        $value = (string) $value;

        if ($length > 0) {
            return strncasecmp($string, $value, $length);
        }

        return strcasecmp($string, $value);
    }

    /**
     * Check to see if a string exists within this string.
     *
     * @param string $string
     * @param string $needle
     * @param bool $strict
     * @param int $offset
     * @return bool
     */
    public static function contains($string, $needle, $strict = true, $offset = 0) {
        return (static::indexOf($string, $needle, $strict, $offset) !== false);
    }

    /**
     * Checks to see if the string ends with a specific value.
     *
     * @param string $string
     * @param string $needle
     * @param bool $strict
     * @return bool
     */
    public static function endsWith($string, $needle, $strict = true) {
        $end = static::extract($string, -mb_strlen($needle));

        if ($strict) {
            return ($end === $needle);
        }

        return (mb_strtolower($end) === mb_strtolower($needle));
    }

    /**
     * Extracts a portion of a string.
     *
     * @param string $string
     * @param int $offset
     * @param int $length
     * @return string
     */
    public static function extract($string, $offset, $length = null) {
        if ($length) {
            return mb_substr($string, $offset, $length);
        }

        return mb_substr($string, $offset);
    }

    /**
     * Generates a string of random characters.
     *
     * @param int $length
     * @param string $seed
     * @return string
     */
    public static function generate($length, $seed = self::ALNUM) {
        $return = '';
        $seed = (string) $seed;
        $totalChars = mb_strlen($seed) - 1;

        for ($i = 0; $i < $length; ++$i) {
            $return .= $seed[rand(0, $totalChars)];
        }

        return $return;
    }

    /**
     * Grab the index of the first matched character.
     *
     * @param string $string
     * @param string $needle
     * @param bool $strict
     * @param int $offset
     * @return int
     */
    public static function indexOf($string, $needle, $strict = true, $offset = 0) {
        if ($strict) {
            return mb_strpos($string, $needle, $offset);
        }

        return mb_stripos($string, $needle, $offset);
    }

    /**
     * Insert values into a string defined by an array of key tokens.
     *
     * @uses Titon\Utility\Sanitize
     *
     * @param string $string
     * @param array $data
     * @param array $options {
     *      @type string $before    Opening variable delimiter
     *      @type string $after     Closing variable delimiter
     *      @type bool $escape      Escape the string
     * }
     * @return string
     */
    public static function insert($string, array $data, array $options = array()) {
        $options = $options + array(
            'before' => '{',
            'after' => '}',
            'escape' => true
        );

        foreach ($data as $key => $value) {
            $string = str_replace($options['before'] . $key . $options['after'], $value, $string);
        }

        if ($options['escape']) {
            $string = Sanitize::escape($string);
        }

        return $string;
    }

    /**
     * Grab the index of the last matched character.
     *
     * @param string $string
     * @param string $needle
     * @param bool $strict
     * @param int $offset
     * @return int
     */
    public static function lastIndexOf($string, $needle, $strict = true, $offset = 0) {
        if ($strict) {
            return mb_strrpos($string, $needle, $offset);
        }

        return mb_strripos($string, $needle, $offset);
    }

    /**
     * Creates a comma separated list with the last item having an ampersand prefixing it.
     *
     * @param array $items
     * @param string $glue
     * @param string $sep
     * @return string
     */
    public static function listing($items, $glue = ' &amp; ', $sep = ', ') {
        if (is_array($items)) {
            $lastItem = array_pop($items);

            if (count($items) === 0) {
                return $lastItem;
            }

            $items = implode($sep, $items);
            $items = $items . $glue . $lastItem;
        }

        return $items;
    }

    /**
     * If a string is too long, shorten it in the middle while also respecting whitespace and preserving words.
     *
     * @param string $string
     * @param int $limit
     * @param string $glue
     * @return string
     */
    public static function shorten($string, $limit = 25, $glue = ' &hellip; ') {
        if (mb_strlen($string) > $limit) {
            $width = round($limit / 2);

            // prefix
            $pre = mb_substr($string, 0, $width);

            if (mb_substr($pre, -1) !== ' ' && ($i = static::lastIndexOf($pre, ' '))) {
                $pre = mb_substr($pre, 0, $i);
            }

            // suffix
            $suf = mb_substr($string, -$width);

            if (mb_substr($suf, 0, 1) !== ' ' && ($i = static::indexOf($suf, ' '))) {
                $suf = mb_substr($suf, $i);
            }

            return trim($pre) . $glue . trim($suf);
        }

        return $string;
    }

    /**
     * Checks to see if the string starts with a specific value.
     *
     * @param string $string
     * @param string $needle
     * @param bool $strict
     * @return bool
     */
    public static function startsWith($string, $needle, $strict = true) {
        $start = static::extract($string, 0, mb_strlen($needle));

        if ($strict) {
            return ($start === $needle);
        }

        return (mb_strtolower($start) === mb_strtolower($needle));
    }

    /**
     * Truncates a string to a certain length. Will preserve HTML tags and words if the flags are true.
     *
     * @param string $string
     * @param int $limit
     * @param array $options {
     *      @type bool $html        True to preserve HTML tags
     *      @type bool $word        True to preserve trailing words
     *      @type string $suffix    Will be appended to the end of the output
     *      @type string $prefix    Will be appended to the beginning of the out output
     *      @type string $open      The opening tag (defaults to < HTML)
     *      @type string $close     The closing tag (defaults to > HTML)
     * }
     * @return string
     */
    public static function truncate($string, $limit = 25, array $options = array()) {
        $options = $options + array(
            'html' => true,
            'word' => true,
            'suffix' => '&hellip;',
            'prefix' => '',
            'open' => '<',
            'close' => '>'
        );

        // If we should preserve HTML
        if ($options['open'] !== '<' || $options['close'] !== '>') {
            $options['html'] = false;
        }

        if (!$options['html']) {
            $string = strip_tags($string);
        }

        // If string is shorten than limit
        $length = mb_strlen($string);

        if ($length <= $limit || !$limit) {
            return $string;
        }

        // Generate tokens
        $open = $options['open'];
        $close = $options['close'];
        $tokens = array();
        $token = '';
        $i = 0;

        while ($i < $length) {
            $char = $string[$i];

            if ($char === $open || $char === '&') {
                $tokens[] = $token;
                $token = $char;

            } else if ($char === $close || $char === ';') {
                $tokens[] = $token . $char;
                $token = '';

            } else {
                $token .= $char;
            }

            $i++;
        }

        $tokens[] = $token;

        // Determine output
        $current = 0;
        $inHtml = false;
        $htmlPattern = '/\\' . $open . '\/?(?:.*?)\\' . $close . '/iSu';
        $entityPattern = '/&[a-z0-9]{2,8};|&#[0-9]{1,7};/iSu';
        $output = '';

        foreach ($tokens as $token) {
            // Increase limit by 1 for tokens
            if (preg_match($entityPattern, $token) && $current < $limit) {
                $current++;
                $output .= $token;

            // Increase limit by 0 for HTML tags but check for tag boundaries
            } else if (preg_match($htmlPattern, $token)) {
                $inHtml = (mb_substr($token, 0, 2) !== $open . '/');
                $output .= $token;

            // Regular string
            } else {
                $length = mb_strlen($token);

                if ($current >= $limit) {
                    // Do nothing, we reached the limit

                } else if (($current + $length) >= $limit) {
                    $allowed = ($limit - $current);
                    $output .= mb_substr($token, 0, $allowed);
                    $current += $allowed;

                } else {
                    $output .= $token;
                    $current += $length;
                }
            }

            // We done?
            if ($current >= $limit && !$inHtml) {
                break;
            }
        }

        // If we should preserve words
        if ($options['word']) {
            $lastChar = mb_substr($output, -1);

            if ($lastChar !== ' ' && $lastChar !== $close && $lastChar !== ';') {
                $output = mb_substr($string, 0, static::lastIndexOf($output, ' '));
            }
        }

        return $options['prefix'] . trim($output) . $options['suffix'];
    }

    /**
     * Creates UUID version 4: random number generation based.
     *
     * @return string
     */
    public static function uuid() {
        return sprintf('%s-%s-%s%s-%s%s-%s',
            static::generate(8, String::HEX), // 1
            static::generate(4, String::HEX), // 2
            4, // 3
            static::generate(3, String::HEX), // 3
            static::generate(1, '89AB'), // 4
            static::generate(3, String::HEX), // 4
            static::generate(12, String::HEX)); // 5
    }

}