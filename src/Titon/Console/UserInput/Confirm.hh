<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\UserInput;

use Titon\Console\Input;
use Titon\Console\Output;

/**
 * @todo
 */
class Confirm extends AbstractUserInput<bool> {

    /**
     * The message to be appended to the prompt message containing the accepted
     * values.
     *
     * @var string
     */
    protected string $message = '';

    /**
     * @todo
     */
    public function __construct(Input $input, Output $output) {
        parent::__construct($input, $output);

        $this->acceptedValues = Map {
            'y'   => true,
            'yes' => true,
            'n'   => false,
            'no'  => false
        };
    }

    /**
     * Prompt the user for input and return the boolean value if the user has
     * confirmed or not.
     *
     * @param string $message   The message to prompt the user with
     *
     * @return bool
     */
    public function confirmed(string $message): bool {
        $input = $this->prompt($message);
        $retval = $this->acceptedValues[strtolower($input)];

        invariant(is_bool($retval), "Must be a boolean value.");

        return $retval;
    }

    /**
     * {@inheritdoc}
     */
    public function prompt(string $message): string {
        $message = "$message $this->message";

        do {
            $this->output->out("$message ", Output::VERBOSITY_NORMAL, 0);
            $input = $this->input->getUserInput();

            if ($input === '' && $this->default !== '') {
                $input = $this->default;
            }
        } while (!$this->acceptedValues->contains(strtolower($input)));

        return $input;
    }

    /**
     * {@inheritdoc}
     */
    <<__Override>>
    public function setDefault(string $default = ''): this {
        switch (strtolower($default)) {
            case 'y':
            case 'yes':
                $this->default = $default;
                $message = " [Y/n]";
                break;
            case 'n':
            case 'no':
                $this->default = $default;
                $message = " [y/N]";
                break;
            default:
                $message = " [y/n]";
                break;
        }

        $this->message = $message;

        return $this;
    }

}
