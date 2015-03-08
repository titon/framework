<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\UserInput;

use Titon\Console\Output;

class Confirm extends AbstractUserInput {

    protected string $message;

    public function __construct(?string $default = null) {
        parent::__construct();

        $this->acceptedValues = Map {
            0 => 'y',
            1 => 'yes',
            2 => 'n',
            3 => 'no'
        };

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
    }

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

    public function prompt(string $message): string {
        $message = "$message $this->message";

        do {
            $this->output->out("$this->message ", Output::VERBOSITY_NORMAL, 0);
            $input = $this->input->getUserInput();
            if ($input === '' && !is_null($this->default)) {
                $input = $this->default;
            }
        } while (!$this->acceptedValues->contains($input));

        return $input;
    }
}