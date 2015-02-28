<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

interface Tree {

    protected function build(?array<string> $tree = null, string $prefix = ''): string;

    public function getData(): array<string>;

    public function render(): string;

    public function setData(): this;
}