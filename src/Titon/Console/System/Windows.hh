<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\System;

class Windows extends AbstractSystem {

    protected Vector<string> $modeOutput = Vector {};

    public function getHeight(): int {
        $this->getStats();

        return (int)$this->modeOutput[0];
    }

    public function getWidth(): int {
        $this->getStats();

        return (int)$this->modeOutput[1];
    }

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

    public function supportsAnsi(): bool {
        return (getenv('ANSICON') === true || getenv('ConEmuANSI') === 'ON');
    }
}