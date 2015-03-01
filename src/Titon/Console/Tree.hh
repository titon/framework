<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

interface Tree {

    public function getData(): array<string>;

    public function render(): string;

    public function setData(array<string> $data): this;
}