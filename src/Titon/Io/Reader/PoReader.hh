<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Io\ResourceMap;
use Titon\Io\Exception\ReadErrorException;

/**
 * A file reader that parses gettext PO files.
 *
 * @package Titon\Io\Reader
 */
class PoReader extends AbstractReader {

    /**
     * {@inheritdoc}
     */
    public function getResourceExt(): string {
        return 'po';
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function readResource(): ResourceMap {
        if ($this->exists()) {
            return $this->_parse();
        }

        throw new ReadErrorException(sprintf('PoReader failed to parse %s', $this->getPath()));
    }

    /**
     * Parse out the po contents.
     *
     * @return \Titon\Io\ResourceMap
     */
    protected function _parse(): ResourceMap {
        $lines = file($this->getPath(), FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
        $data = Map {};
        $key = '';
        $value = '';
        $plurals = Vector {};
        $isPlural = false;

        foreach ($lines as $line) {
            // Comment or empty line
            if ($line[0] === '#' || !$line) {
                continue;

            // Multiline value
            } else if ($line[0] === '"') {
                $value .= "\n" . static::dequote($line);

            // Key
            } else if (mb_strpos($line, 'msgid') === 0) {

                // Save the previous value
                if ($key !== '') {
                    if ($plurals) {
                        $data[$key] = $plurals;
                    } else {
                        $data[$key] = $value;
                    }

                    $value = '';
                    $plurals = Vector {};
                }

                $key = static::dequote($line);

            // Message
            } else if (mb_strpos($line, 'msgstr') === 0) {

                // msgstr[n]
                if ($line[6] === '[') {
                    $plurals[] = static::dequote($line);

                // msgstr
                } else {
                    $value = static::dequote($line);
                }
            }
        }

        // Grab the last value
        if ($key !== '') {
            if ($plurals) {
                $data[$key] = $plurals;
            } else {
                $data[$key] = $value;
            }
        }

        return $data;
    }

    /**
     * Remove the quotes from a message string.
     *
     * @param string $string
     * @return string
     */
    public static function dequote(string $string): string {
        return mb_substr(mb_substr($string, mb_strpos($string, '"')), 1, -1);
    }

}