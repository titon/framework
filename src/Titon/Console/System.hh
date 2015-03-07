<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

interface System {

    public function getHeight(): int;

    public function getWidth(): int;

    public function isPiped(): bool;

    public function supportsAnsi(): bool;
}