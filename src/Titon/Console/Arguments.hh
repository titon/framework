<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Iterator;

class Arguments {

    protected FlagMap $flags = Map {};

    protected OptionMap $options = Map {};

    protected ArgumentMap $arguments = Map {};

    protected CommandMap $commands = Map {};

    protected ArgumentLexer $input;

    /**
     * Initializes the argument parser. If you wish to change the default behaviour
     * you may pass an array of options as the first argument. Valid options are
     * `'help'` and `'strict'`, each a boolean.
     *
     * `'help'` is `true` by default, `'strict'` is false by default.
     *
     * @param  array $options An array of options for this parser.
     */
    public function __construct(array<string> $args = []) {
        if (count($args) === 0) {
            $args = array_slice($_SERVER['argv'], 1);
        }

        $this->input = new ArgumentLexer($args);
    }

    public function parse() {
        foreach ($this->input as $val) {
            if ($this->parseFlag($val)) {
                continue;
            }
            if ($this->parseOption($val)) {
                continue;
            }
//            if ($this->parseArgument($val)) {
//                continue;
//            }
        }
    }

    public function parseFlag($key): bool {
        if ($this->flags->contains($key)) {
            return false;
        }

        if ($this->flags[$key]->isStackable()) {
            $this->flags[$key]->increaseValue();
        } else {
            $this->flags[$key]->setValue(1);
        }

        return true;
    }

    public function parseOption(string $key): bool {
        if (!$this->options->contains($key))) {
            return false;
        }

        // Peak ahead to make sure we get a value.
        if (!$this->input->end() && !$this->input->isArgument($this->input->peek()))
            $this->options[$key]->setValue($this->options[$key]->getDefault());

            return true;
        }

        $values = [];
        foreach ($this->input as $value) {
            array_push($values, $value);
            if (!$this->input->end() && $this->input->isArgument($this->input->peek())) {
                break;
            }
        }

        $value = join($values, ' ');

        if (!$value && $value !== false) {
            $value = 1;
        }

        $this->options[$key]->setValue($value);

        return true;
    }
}