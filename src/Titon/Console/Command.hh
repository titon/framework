<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Console\InputDefinition\Argument;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;

interface Command {

    public function configure(): void;

    public function getArguments(): InputBag<Argument>;

    public function getDescription(): string;

    public function getFlags(): InputBag<Flag>;

    public function getName(): string;

    public function getOptions(): InputBag<Option>;

    public function run(): void;
}