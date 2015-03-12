<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\System;

/**
 * Windows-specific information
 *
 * @package Titon\Console\System
 */
class WindowsSystem extends AbstractSystem {

    /**
     * Data structure containing the relevant output from the `mode` command.
     *
     * @var Vector<string>
     */
    protected Vector<string> $modeOutput = Vector {};

    /**
     * {@inheritdoc}
     */
    public function getHeight(): int {
        $this->getStats();

        return (int)$this->modeOutput[0];
    }

    /**
     * {@inheritdoc}
     */
    public function getWidth(): int {
        $this->getStats();

        return (int)$this->modeOutput[1];
    }

    /**
     * Get the width and height of the current terminal window from the `mode`
     * command.
     *
     * @return Vector<string>
     */
    public function getStats(): Vector<string> {
        if ($this->modeOutput->isEmpty()) {
            $output = '';
            exec('mode', $output);
            $output = implode("\n", $output);
            $matches = [];
            preg_match_all('/.*:\s*(\d+)/', $output, $matches);
            $this->modeOutput = new Vector((count($matches[1]) > 0) ? $matches[1] : []);
        }

        return $this->modeOutput;
    }

    /**
     * {@inheritdoc}
     */
    public function supportsAnsi(): bool {
        return (getenv('ANSICON') === true || getenv('ConEmuANSI') === 'ON');
    }
}