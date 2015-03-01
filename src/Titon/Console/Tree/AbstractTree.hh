<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Tree;

use Titon\Console\Tree;

abstract class AbstractTree implements Tree {

    protected array<string> $data = [];

    public function __construct(array<string> $data = []) {
        $this->data = $data;
    }

    abstract protected function build(array<string> $tree, string $prefix = ''): string;

    public function getData(): array<string> {
        return $this->data;
    }

    public function render(): string {
        return $this->build($this->data);
    }

    public function setData(array<string> $data): this {
        $this->data = $data;

        return $this;
    }
}