<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\UserInput;

use Titon\Console\Output;

/**
 * The `Prompt` class presents the user with a basic prompt and accepts any input
 * unless predetermined choices are given.
 *
 * @package Titon\Console\UserInput
 */
class Prompt extends AbstractUserInput {

    /**
     * If the prompt is set to show value hints, this string contains those hints
     * to output when presenting the user with the prompt.
     *
     * @var string
     */
    protected string $hint = '';

    /**
     * Construct a new `Prompt` object with accepted values (if any).
     *
     * @param Map<mixed, mixed> $choices    Predetermined accepted choices
     */
    public function __construct(Map<mixed, mixed> $choices = Map {}, ?string $default = null) {
        parent::__construct();

        $this->default = $default;
        $this->acceptedValues = $choices;
    }

    /**
     * {@inheritdoc}
     */
    public function prompt(string $message): mixed {
        $keys = $this->acceptedValues->keys();
        $values = $this->acceptedValues->values();
        if ($this->hint !== '') {
            $message .= " $this->hint";
        }

        while (true) {
            $this->output->out("$message: ", Output::VERBOSITY_NORMAL, 0);
            $input = $this->input->getUserInput();

            if ($input === '' && !is_null($this->default)) {
                $input = $this->default;
            }

            if ($this->acceptedValues->isEmpty()) {
                return $input;
            }

            if (is_numeric($input)) {
                $input = (int)$input;
                $input--;
                if (!is_null($values[$input])) {
                    return $keys[$input];
                }

                if ($input < 0 || $input >= $values->count()) {
                    $this->output->error('Invalid menu selection: out of range');
                }
            } else {
                foreach ($values as $index => $val) {
                    if ($this->strict) {
                        if ($input === $val) {
                            return $keys[$index];
                        }
                    } else {
                        if (strtolower($input) === strtolower($val)) {
                            return $keys[$index];
                        }
                    }
                }
            }
        }
    }

    /**
     * Set whether the message presented at the prompt should show the predetermined
     * accepted values (if any).
     *
     * @param bool $showHint    Boolean to show the hints or not
     *
     * @return $this
     */
    public function showHints(bool $showHint = true): this {
        if ($showHint === true) {
            $retval = Vector {};
            foreach ($this->acceptedValues as $k => $v) {
                $retval[] = $v;
            }
            $retval = implode('/', $retval);

            $this->hint = "[$retval]";
        } else {
            $this->hint = '';
        }

        return $this;
    }
}