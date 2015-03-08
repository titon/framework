<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\UserInput;

use Titon\Console\Output;

class Confirm extends AbstractUserInput {

    public function __construct(?string $default = null) {
        parent::__construct();

        $this->acceptedValues = Vector {
            'y',
            'yes',
            'n',
            'no'
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

    public function confirmed(): bool {
        $input = $this->prompt();

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
        $this->message = "$message $this->message";

        do {
            $this->output->out("$this->message ", Output::VERBOSITY_NORMAL, 0);
            $input = $this->input->getUserInput();
            if ($input === '' && !is_null($this->default)) {
                $input = $this->default;
            }
        } while ($this->acceptedValues->linearSearch(strtolower($input)) < 0);

        return $input;
    }
}