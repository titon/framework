<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\UserInput;

use Titon\Console\Input;
use Titon\Console\Output;
use Titon\Console\UserInput;

/**
 * `AbstractUserInput` handles core functionality for prompting and accepting
 * the user input.
 *
 * @package Titon\Console\UserInput
 */
abstract class AbstractUserInput<T> implements UserInput {

    /**
     * Input values accepted to continue.
     *
     * @var Map<string, T>
     */
    protected Map<string, T> $acceptedValues = Map {};

    /**
     * Default value if input given is empty.
     *
     * @var string
     */
    protected string $default = '';

    /**
     * `Input` object used for retrieving user input.
     *
     * @var \Titon\Console\Input
     */
    protected Input $input;

    /**
     * `Output` object used for sending output.
     *
     * @var \Titon\Console\Output
     */
    protected Output $output;

    /**
     * If the input should be accepted strictly or not.
     *
     * @var bool
     */
    protected bool $strict = true;

    /**
     * Construct a new `UserInput` object.
     */
    public function __construct(Input $input, Output $output) {
        $this->input = $input;
        $this->output = $output;
    }

    /**
     * Set the values accepted by the user.
     *
     * @param Map<string, T> $choices    Accepted values
     *
     * @return $this
     */
    public function setAcceptedValues(Map<string, T> $choices = Map {}): this {
        $this->acceptedValues = $choices;

        return $this;
    }

    /**
     * Set the default value to use when input is empty.
     *
     * @param string $default   The default value
     *
     * @return $this
     */
    public function setDefault(string $default): this {
        $this->default = $default;

        return $this;
    }

    /**
     * Set if the prompt should run in strict mode.
     *
     * @param bool $strict  Strict value
     *
     * @return $this
     */
    public function setStrict(bool $strict): this {
        $this->strict = $strict;

        return $this;
    }
}