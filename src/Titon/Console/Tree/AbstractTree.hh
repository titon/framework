<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Tree;

use Titon\Console\Tree;

/**
 * The `AbstractTree` class provides core functionality for building a tree given
 * a data structure.
 *
 * @package Titon\Console\Tree
 */
abstract class AbstractTree implements Tree {

    /**
     * The data for the tree.
     *
     * @var array<mixed>
     */
    protected array<mixed> $data = [];

    /**
     * Construct a new instance of a tree.
     *
     * @param array<mixed> $data    The data to render
     */
    public function __construct(array<mixed> $data = []) {
        $this->data = $data;
    }

    /**
     * Recursively build the tree and each branch and prepend necessary markup
     * for the output.
     *
     * @param array<mixed> $tree    The tree or branch to build
     * @param string $prefix        The markup prefix for the tree / branch
     *
     * @retun string
     */
    abstract protected function build(array<mixed> $tree, string $prefix = ''): string;

    /**
     * Retrieve the data of the tree.
     *
     * @return array<mixed>
     */
    public function getData(): array<mixed> {
        return $this->data;
    }

    /**
     * Render the tree.
     *
     * @return string
     */
    public function render(): string {
        return $this->build($this->data);
    }

    /**
     * {@inheritdoc}
     */
    public function setData(array<mixed> $data): this {
        $this->data = $data;

        return $this;
    }
}