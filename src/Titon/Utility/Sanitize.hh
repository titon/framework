<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;

/**
 * Makes dirty values clean! Sanitize will process an input and return a safe output depending on the scope of the cleaner.
 *
 * @package Titon\Utility
 */
class Sanitize {
    use Macroable;

    /**
     * Sanitize an email by removing all characters except letters, digits and !#$%&'*+-/=?^_`{|}~@.[].
     *
     * @param string $value
     * @param mixed $options
     * @return string
     */
    public static function email(string $value, mixed $options = []): string {
        return (string) filter_var($value, FILTER_SANITIZE_EMAIL, $options);
    }

    /**
     * Escape a string using the apps encoding.
     *
     * @param string $value
     * @param Map<string, mixed> $options {
     *      @type string $encoding  Character encoding set; defaults to UTF-8
     *      @type int $flags        Encoding flags; defaults to ENT_QUOTES
     *      @type bool $double      Will double escape existing entities
     * }
     * @return string
     */
    public static function escape(string $value, Map<string, mixed> $options = Map {}): string {
        $options = Col::merge(Map {
            'encoding' => 'UTF-8',
            'flags' => ENT_QUOTES,
            'double' => false
        }, $options);

        return htmlentities($value, $options['flags'], $options['encoding'], $options['double']);
    }

    /**
     * Sanitize a float by removing all characters except digits, +- and .,eE.
     *
     * @param string $value
     * @return float
     */
    public static function float(string $value): float {
        return (float) filter_var($value, FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION | FILTER_FLAG_ALLOW_THOUSAND | FILTER_FLAG_ALLOW_SCIENTIFIC);
    }

    /**
     * Sanitize a string by removing xor escaping HTML characters and entities.
     *
     * @param string $value
     * @param Map<string, mixed> $options {
     *      @type bool $strip       Will remove HTML tags
     *      @type string $whitelist List of tags to not strip
     * }
     * @return string
     */
    public static function html(string $value, Map<string, mixed> $options = Map {}): string {
        $options = Col::merge(Map {
            'strip' => true,
            'whitelist' => ''
        }, $options);

        if ($options['strip']) {
            $value = strip_tags($value, (string) $options['whitelist']);
        }

        return static::escape($value, $options);
    }

    /**
     * Sanitize an integer by removing all characters except digits, plus and minus sign.
     *
     * @param string $value
     * @param mixed $options
     * @return int
     */
    public static function integer(string $value, mixed $options = []): int {
        return (int) filter_var($value, FILTER_SANITIZE_NUMBER_INT, $options);
    }

    /**
     * Sanitize a string by removing excess CRLF characters.
     *
     * @param string $value
     * @param Map<string, mixed> $options {
     *      @type bool $cr      Will remove carriage returns \r
     *      @type bool $lf      Will remove line feeds \n
     *      @type bool $crlf    Will remove CRLF \r\n
     *      @type bool $trim    Will remove whitespace and newlines around the edges
     *      @type int $limit    The start limit to remove extraneous characters
     * }
     * @return string
     */
    public static function newlines(string $value, Map<string, mixed> $options = Map {}): string {
        $options = Col::merge(Map {
            'cr' => true,
            'lf' => true,
            'crlf' => true,
            'limit' => 2,
            'trim' => true
        }, $options);

        if ($options['limit']) {
            $pattern = '/(?:%s){' . (string) $options['limit'] . ',}/u';
            $replace = null;

        } else {
            $pattern = '/(?:%s)+/u';
            $replace = '';
        }

        if ($options['crlf']) {
            $value = preg_replace(sprintf($pattern, '\r\n'), is_null($replace) ? "\r\n" : $replace, $value);
        }

        if ($options['cr']) {
            $value = preg_replace(sprintf($pattern, '\r'), is_null($replace) ? "\r" : $replace, $value);
        }

        if ($options['lf']) {
            $value = preg_replace(sprintf($pattern, '\n'), is_null($replace) ? "\n" : $replace, $value);
        }

        if ($options['trim']) {
            $value = trim($value);
        }

        return $value;
    }

    /**
     * Sanitize a URL by removing all characters except letters, digits and $-_.+!*'(),{}|\\^~[]`<>#%";/?:@&=.
     *
     * @param string $value
     * @param mixed $options
     * @return string
     */
    public static function url(string $value, mixed $options = []): string {
        return (string) filter_var($value, FILTER_SANITIZE_URL, $options);
    }

    /**
     * Sanitize a string by removing excess whitespace and tab characters.
     *
     * @param string $value
     * @param Map<string, mixed> $options {
     *      @type bool $space   Will remove white space
     *      @type bool $tab     Will remove tabs
     *      @type bool $strip   Will remove non-standard white space character
     *      @type bool $trim    Will remove whitespace and newlines around the edges
     *      @type int $limit    The start limit to remove extraneous characters
     * }
     * @return string
     */
    public static function whitespace(string $value, Map<string, mixed> $options = Map {}): string {
        $options = Col::merge(Map {
            'space' => true,
            'tab' => true,
            'limit' => 2,
            'strip' => true,
            'trim' => true
        }, $options);

        if ($options['limit']) {
            $pattern = '/%s{' . $options['limit'] . ',}/u';
            $replace = null;

        } else {
            $pattern = '/%s+/u';
            $replace = '';
        }

        if ($options['tab']) {
            $value = preg_replace(sprintf($pattern, '\t'), is_null($replace) ? "\t" : $replace, $value);
        }

        if ($options['space']) {
            $value = preg_replace(sprintf($pattern, ' '), is_null($replace) ? ' ' : $replace, $value); // \s replaces other whitespace characters
        }

        if ($options['strip']) {
            $value = str_replace(chr(0xCA), ' ', $value);
        }

        if ($options['trim']) {
            $value = trim($value);
        }

        return $value;
    }

    /**
     * Sanitize a string by removing any XSS attack vectors.
     * Will bubble up to html() and escape().
     *
     * @param string $value
     * @param Map<string, mixed> $options {
     *      @type bool $strip   Remove HTML tags
     * }
     * @return string
     */
    public static function xss(string $value, Map<string, mixed> $options = Map {}): string {
        $options = Col::merge(Map {'strip' => true}, $options);
        $value = str_replace("\0", '', $value);

        if (!$options['strip']) {

            // Remove any attribute starting with on or xmlns
            $value = preg_replace('/\s?(?:on[a-z]+|xmlns)\s?=\s?"(?:.*?)"/isu', '', $value);

            // Remove namespaced elements
            $value = preg_replace('/<\/?\w+:\w+(?:.*?)>/isu', '', $value);

            // Remove really unwanted tags
            do {
                $old = $value;
                $value = preg_replace('/<\/?(?:applet|base|bgsound|embed|frame|frameset|iframe|layer|link|meta|object|script|style|title|xml|audio|video)(?:.*?)>/isu', '', $value);
            } while ($old !== $value);
        }

        return static::html($value, $options);
    }

}