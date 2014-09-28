<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Utility\Col;

/**
 * A file writer that generates INI files.
 *
 * @package Titon\Io\Writer
 */
class IniWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Utility\Hash
     */
    public function append($data) {
        if ($contents = $this->read()) {
            $data = Col::merge(parse_ini_string($contents, true), $data);
        }

        return $this->write($data);
    }

    /**
     * {@inheritdoc}
     */
    public function write($data) {
        return parent::write($this->_process($data));
    }

    /**
     * Process a multi-dimensional array into an INI format.
     *
     * @param string|array $data
     * @return string
     */
    protected function _process($data) {
        $sections = [];
        $settings = [];
        $output = '';

        // Parse out the sections and settings
        foreach ((array) $data as $key => $value) {
            if (is_array($value) && !isset($value[0])) {
                $sections[$key] = $value;
            } else {
                $settings[$key] = $value;
            }
        }

        // Write settings first
        if ($settings) {
            foreach ($settings as $key => $value) {
                $output .= $this->_processLine($key, $value);
            }
        }

        // And then sections
        if ($sections) {
            foreach ($sections as $key => $value) {
                $output .= PHP_EOL . sprintf('[%s]', $key) . PHP_EOL;

                if ($value) {
                    foreach ($value as $k => $v) {
                        $output .= $this->_processLine($k, $v);
                    }

                    $output .= PHP_EOL;
                }
            }
        }

        return $output;
    }

    /**
     * Process an individual line and support array indices.
     *
     * @param string $key
     * @param mixed $value
     * @return string
     */
    protected function _processLine($key, $value) {
        $output = '';

        if (is_array($value)) {
            foreach ($value as $v) {
                $output .= sprintf('%s[] = %s', $key, $this->_getValue($v)) . PHP_EOL;
            }
        } else {
            $output .= sprintf('%s = %s', $key, $this->_getValue($value)) . PHP_EOL;
        }

        return $output;
    }

    /**
     * Type cast an INI value to the standard.
     *
     * @param mixed $value
     * @return bool|int|string
     */
    protected function _getValue($value) {
        if (is_numeric($value)) {
            return $value;

        } else if ($value === true || in_array($value, ['on', 'yes', 'true'])) {
            return true;

        } else if ($value === false || in_array($value, ['off', 'no', 'false'])) {
            return false;
        }

        return sprintf('"%s"', $value);
    }

}