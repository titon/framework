<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Tree;

use Titon\Console\Tree;
use Titon\Console\TreeData;

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
     * @var \Titon\Console\TreeData
     */
    protected TreeData $data = [];

    /**
     * Construct a new instance of a tree.
     *
     * @param \Titon\Console\TreeData $data The data to render
     */
    public function __construct(?TreeData $data = null) {
        if (!is_null($data)) {
            $this->data = $data;
        }
    }

    /**
     * Recursively build the tree and each branch and prepend necessary markup
     * for the output.
     *
     * @param \Titon\Console\TreeData $tree The tree or branch to build
     * @param string $prefix        The markup prefix for the tree / branch
     *
     * @return string
     */
    abstract protected function build(TreeData $tree, string $prefix = ''): string;

    /**
     * Retrieve the data of the tree.
     *
     * @return \Titon\Console\TreeData
     */
    public function getData(): TreeData {
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
    public function setData(TreeData $data): this {
        $this->data = $data;

        return $this;
    }

}
