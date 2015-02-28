<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

interface InputDefinition {

    public function getAlias(): string;

    public function getDescription(): string;

    public function getName(): string;

    public function getValue(): mixed;
}