<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Writer;

use Titon\Io\ResourceMap;

/**
 * A file writer that generates INI files.
 *
 * @package Titon\Io\Writer
 */
class IniWriter extends AbstractWriter {

    /**
     * {@inheritdoc}
     */
    public function getResourceExt(): string {
        return 'ini';
    }

    /**
     * {@inheritdoc}
     */
    public function writeResource(ResourceMap $data): bool {
        return $this->write($this->_process($data));
    }

    /**
     * Process a multi-dimensional array into an INI format.
     *
     * @param \Titon\Io\ResourceMap $data
     * @return string
     */
    protected function _process(ResourceMap $data): string {
        $sections = Map {};
        $settings = Map {};
        $output = '';

        // Parse out the sections and settings
        foreach ($data as $key => $value) {
            if ($value instanceof Map) {
                $sections[$key] = $value;
            } else {
                $settings[$key] = $value;
            }
        }

        // Write settings first
        foreach ($settings as $key => $value) {
            $output .= $this->_processLine($key, $value);
        }

        // And then sections
        foreach ($sections as $key => $settings) {
            $output .= PHP_EOL . sprintf('[%s]', $key) . PHP_EOL;

            foreach ($settings as $k => $v) {
                $output .= $this->_processLine($k, $v);
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
    protected function _processLine(string $key, mixed $value): string {
        $output = '';

        if ($value instanceof Vector) {
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
     * @return string
     */
    protected function _getValue(mixed $value): string {
        if (is_numeric($value)) {
            return (string) $value;

        } else if ($value === true || in_array($value, ['on', 'yes', 'true'])) {
            return '1';

        } else if ($value === false || in_array($value, ['off', 'no', 'false'])) {
            return '0';
        }

        return sprintf('"%s"', $value);
    }

}