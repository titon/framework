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

abstract class AbstractUserInput implements UserInput {

    protected Map<mixed, mixed> $acceptedValues = Map {};

    protected ?string $default;

    protected Input $input;

    protected Output $output;

    protected bool $strict = true;

    public function __construct() {
        $this->input = new Input();
        $this->output = new Output();
    }

    public function setDefault(string $default): this {
        $this->default = $default;

        return $this;
    }

    public function setStrict(bool $strict): this {
        $this->strict = $strict;

        return $this;
    }
}