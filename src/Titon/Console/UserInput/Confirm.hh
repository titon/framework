<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\UserInput;

use Titon\Console\Output;

class Confirm extends AbstractUserInput {

    /**
     * The message to be appened to the prompt message containing the accepted
     * values.
     *
     * @var string
     */
    protected string $message = '';

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

        switch (strtolower($input)) {
            case 'y':
            case 'yes':
                return true;
            case 'n':
            case 'no':
            default:
                return false;
        }
    }

    /**
     * {@inheritdoc}
     */
    public function prompt(string $message): string {
        $message = "$message $this->message";

        do {
            $this->output->out("$this->message ", Output::VERBOSITY_NORMAL, 0);
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