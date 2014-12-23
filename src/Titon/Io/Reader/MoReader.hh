<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Io\Exception\ReadErrorException;

/**
 * A file reader that parses gettext MO files.
 *
 * @package Titon\Io\Reader
 */
class MoReader extends AbstractReader {

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function read<Tk, Tv>(): Map<Tk, Tv> {
        if ($this->exists()) {
            return ($data = $this->_unpack()) ? $data : Map {};
        }

        throw new ReadErrorException(sprintf('MoReader failed to parse %s', $this->path()));
    }

    /**
     * Unpack the mo file contents and extract the hash tables.
     *
     * @return \Titon\Io\ResourceMap
     */
    protected function _unpack(): ?ResourceMap {
        $file = fopen($this->path(), 'rb');
        $header = fread($file, 28);

        if (strlen($header) != 28) {
            return null;
        }

        // Determine endian
        $endian = unpack('Nendian', substr($header, 0, 4));

        if ($endian['endian'] == (int) hexdec('950412de')) {
            $endian = 'N';

        } else if ($endian['endian'] == (int) hexdec('de120495')) {
            $endian = 'V';

        } else {
            return null;
        }

        // Extract header
        $header = unpack("{$endian}Hrevision/{$endian}Hcount/{$endian}HposOriginals/{$endian}HposTranslations/{$endian}HsizeHash/{$endian}HposHash", substr($header, 4));

        if (!is_array($header)) {
            return null;
        }

        // Support revision 0 of MO format specs, only
        if ($header['Hrevision'] != 0) {
            return null;
        }

        // 1) Read index tables on originals and translations

        // Seek data blocks
        fseek($file, $header['HposOriginals'], SEEK_SET);

        // Read originals indices
        $HsizeOriginals = $header['HposTranslations'] - $header['HposOriginals'];

        if ($HsizeOriginals != ($header['Hcount'] * 8)) {
            return null;
        }

        $originals = fread($file, $HsizeOriginals);

        if (strlen($originals) != $HsizeOriginals) {
            return null;
        }

        // Read translations indices
        $HsizeTranslations = $header['HposHash'] - $header['HposTranslations'];

        if ($HsizeTranslations != ($header['Hcount'] * 8)) {
            return null;
        }

        $translations = fread($file, $HsizeTranslations);

        if (strlen($translations) != $HsizeTranslations) {
            return null;
        }

        // Transform raw data into a set of indices
        $originals = str_split($originals, 8);
        $translations = str_split($translations, 8);

        // 2) Read set of strings to separate string

        // Skip hash table
        $HposStrings = $header['HposHash'] + $header['HsizeHash'] * 4;

        fseek($file, $HposStrings, SEEK_SET);

        // Read strings expected in rest of file
        $strings = '';

        while (!feof($file)) {
            $strings .= fread($file, 4096);
        }

        fclose($file);

        // 3) Collect hash records

        $hash = Map {};

        for ($i = 0; $i < $header['Hcount']; $i++) {

            // Parse index records on original and related translation
            $o = unpack("{$endian}length/{$endian}pos", $originals[$i]);
            $t = unpack("{$endian}length/{$endian}pos", $translations[$i]);

            if (!$o || !$t) {
                return null;
            }

            // Adjust offset due to reading strings to separate space before
            $o['pos'] -= $HposStrings;
            $t['pos'] -= $HposStrings;

            // Extract original and translations
            $original = substr($strings, $o['pos'], $o['length']);
            $translation = substr($strings, $t['pos'], $t['length']);

            if ($original !== '') {
                $sep = strpos($original, "\04");

                if ($sep !== false) {
                    $context = substr($original, 0, $sep);
                    $original = substr($original, $sep + 1);
                } else {
                    $context = null;
                }

                $original = explode("\00", $original);
                $originalCount = count($original);
                $translation = explode("\00", $translation);

                $singularFrom = array_shift($original);
                $singularTo = array_shift($translation);

                if ($originalCount && ($originalCount == count($translation))) {
                    $plurals = array_combine($original, $translation);
                } else {
                    $plurals = [];
                }

                $key = is_null($context) ? $singularFrom : "$context\04$singularFrom";

                $hash[$key] = new Vector($plurals ?: $singularTo);
            }
        }

        return $hash;
    }

}