<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

/**
 * A `Tree` object will construct the markup for a human readable of nested data.
 *
 * @package Titon\Console
 */
interface Tree {

    /**
     * Retrieve the data structure of the `Tree`.
     *
     * @return \Titon\Console\TreeData
     */
    public function getData(): TreeData;

    /**
     * Build and return the markup for the `Tree`.
     *
     * @return string
     */
    public function render(): string;

    /**
     * Set the data for the tree.
     *
     * @param \Titon\Console\TreeData $data The data for the tree
     *
     * @return $this
     */
    public function setData(TreeData $data): this;
}