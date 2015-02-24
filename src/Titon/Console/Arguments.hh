<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Common\ArgumentList;
use Titon\Console\InputDefinition\Argument;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;
use Titon\Console\Exception\MissingValueException;

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
    public function __construct(?ArgumentList $args = null) {
        if (is_null($args) === 0) {
            $args = array_slice($_SERVER['argv'], 1);
        }

        $this->input = new ArgumentLexer($args);
    }

    public function addArgument(Argument $argument): this {
        $this->arguments[$argument->getName()] = $argument;

        return $this;
    }

    public function addFlag(Flag $flag): this {
        $this->flags[$flag->getName()] = $flag;

        return $this;
    }

    public function addOption(Option $option): this {
        $this->options[$option->getName()] = $option;

        return $this;
    }

    public function getArgument(string $key): ?Argument {
        if (!is_null($argument = $this->arguments->get($key))) {
            return $argument;
        }

        return null;
    }

    public function getFlag(string $key): ?Flag {
        if (!is_null($flag = $this->flags->get($key))) {
            return $flag;
        }

        foreach ($this->flags as $flag) {
            if ($key === $flag->getName() || $key === $flag->getAlias()) {
                return $flag;
            }
        }

        return null;
    }

    public function getOption(string $key): ?Option {
        if (!is_null($option = $this->options->get($key))) {
            return $option;
        }

        return null;
    }

    public function parse() {
        foreach ($this->input as $val) {
            if ($this->parseFlag($val)) {
                continue;
            }
            if ($this->parseOption($val)) {
                continue;
            }
            if ($this->parseArgument($val)) {
                continue;
            }
        }
    }

    public function parseArgument(Input $key): bool {
        foreach ($this->arguments as $argument) {
            if (is_null($argument->getValue())) {
                $argument->setValue($key['raw']);

                return true;
            }
        }

        return false;
    }

    public function parseFlag(Input $key): bool {
        $key = $key['value'];
        if (is_null($flag = $this->flags->get($key))) {
            return false;
        }

        if ($this->flags[$key]->isStackable()) {
            $this->flags[$key]->increaseValue();
        } else {
            $this->flags[$key]->setValue(1);
        }

        return true;
    }

    public function parseOption(Input $key): bool {
        $key = $key['value'];
        if (is_null($option = $this->options->get($key))) {
            return false;
        }

        // Peak ahead to make sure we get a value.
        $nextValue = $this->input->peek();
        if (!$this->input->end() && $this->input->isArgument($nextValue['raw'])) {
            throw new MissingValueException("No value is present for option $key");
        }

        $values = Vector {};
        foreach ($this->input as $value) {
            $values[] = $value['raw'];
            break;

//            $nextValue = $this->input->peek();
//            if (!$this->input->end() && $this->input->isArgument($nextValue['raw'])) {
//                break;
//            }
        }

        $value = join($values, ' ');

        if (!$value && $value !== false) {
            $value = 1;
        }

        $option->setValue($value);

        return true;
    }
}