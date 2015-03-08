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
abstract class AbstractUserInput implements UserInput {

    /**
     * Input values accepted to continue.
     *
     * @var Map<mixed, mixed>
     */
    protected Map<mixed, mixed> $acceptedValues = Map {};

    /**
     * Default value if input given is empty.
     *
     * @var string|null
     */
    protected ?string $default;

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
    public function __construct() {
        $this->input = Input::getInstance();
        $this->output = Output::getInstance();
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