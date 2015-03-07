<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\System;

class Windows extends AbstractSystem {

    protected Map<string, string> $modeOutput = Map {};

    public function getHeight(): int {
        $this->getStats();

        return $this->modeOutput[0];
    }

    public function getWidth(): int {
        $this->getStats();

        return $this->modeOutput[1];
    }

    public function getStats(): Map<string, string> {
        if ($this->modeOutput->isEmpty()) {
            exec('mode', $output);
            $output = implode("\n", $output);
            preg_match_all('/.*:\s*(\d+)/', $output, $matches);
            $this->modeOutput = (!empty($matches[1])) ? $matches[1] : [];
        }

        return $this->modeOutput;
    }

    public function supportsAnsi(): bool {
        return (getenv('ANSICON') === true || getenv('ConEmuANSI') === 'ON');
    }
}