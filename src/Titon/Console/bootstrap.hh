<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

/**
 * --------------------------------------------------------------
 *  Type Aliases
 * --------------------------------------------------------------
 *
 * Defines type aliases that are used by the console package.
 */

namespace Titon\Console {
    use Titon\Console\Command;
    use Titon\Console\InputDefinition\Flag;
    use Titon\Console\InputDefinition\Option;
    use Titon\Console\InputDefinition\Argument;

    type Input = shape(
        'raw' => string,
        'value' => string
    );
    type CommandMap = Map<string, Command>;
    type FlagMap = Map<string, Flag>;
    type OptionMap = Map<string, Option>;
    type ArgumentMap = Map<string, Argument>;
}
