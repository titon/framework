<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Common\Config;
use Titon\Common\DataMap;
use Titon\Io\Reader\PoReader;
use Titon\Utility\Col;

/**
 * A file writer that generates PO files.
 *
 * @package Titon\Io\Writer
 */
class PoWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     */
    public function append(DataMap $data) {
        $reader = new PoReader($this->path());

        if ($contents = $reader->read()) {
            $data = Col::merge($contents, $data);
        }

        return parent::write($data);
    }

    /**
     * {@inheritdoc}
     */
    public function write(DataMap $data) {
        return parent::write($this->_process($data));
    }

    /**
     * Process an array into a PO format taking into account multi-line and plurals.
     *
     * @uses Titon\Common\Config
     *
     * @param \Titon\Common\DataMap $data
     * @return string
     */
    protected function _process(DataMap $data): string {
        $comments = Map {
            'Project-Id-Version' => 'Titon',
            'POT-Creation-Date' => date('Y-m-d H:iO'),
            'PO-Revision-Date' => date('Y-m-d H:iO'),
            'Last-Translator' => '',
            'Language-Team' => '',
            'Language' => Config::get('Titon.locale.current'),
            'MIME-Version' => '1.0',
            'Content-Type' => 'text/plain; charset=' . Config::encoding(),
            'Content-Transfer-Encoding' => '8bit',
            'Plural-Forms' => 'nplurals=2; plural=0;'
        };

        if ($data->contains('_comments')) {
            $comments = Col::merge($comments, $data['_comments']);
            $data->remove('_comments');
        }

        $output = '';

        // Output comments first
        if ($comments) {
            $output .= 'msgid ""' . PHP_EOL;
            $output .= 'msgstr ""' . PHP_EOL;

            foreach ($comments as $key => $value) {
                $output .= sprintf('"%s: %s\n"', $key, $value) . PHP_EOL;
            }
        }

        // Output values
        if ($data) {
            foreach ($data as $key => $value) {
                $output .= $this->_processLine($key, $value);
            }
        }

        return $output;
    }

    /**
     * Process an individual line and support multi-line and plurals.
     *
     * @param string $key
     * @param mixed $value
     * @return string
     */
    protected function _processLine(string $key, mixed $value): string {
        if (is_numeric($key)) {
            $key = $value;
            $value = '';
        }

        $output = PHP_EOL;
        $output .= sprintf('msgid "%s"', $key) . PHP_EOL;

        // Plurals
        if ($value instanceof Vector) {
            $output .= sprintf('msgid_plural "%s"', $key) . PHP_EOL;

            foreach ($value as $i => $v) {
                $output .= sprintf('msgstr[%s] "%s"', $i, $v) . PHP_EOL;
            }

        // Single or multi-line
        } else {
            $value = explode("\n", str_replace("\r", '', $value));

            foreach ($value as $i => $v) {
                if ($i == 0) {
                    $output .= sprintf('msgstr "%s"', $v) . PHP_EOL;
                } else {
                    $output .= sprintf('"%s"', $v) . PHP_EOL;
                }
            }
        }

        return $output;
    }

}