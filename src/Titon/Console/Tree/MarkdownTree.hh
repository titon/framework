<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Tree;

use Titon\Console\TreeData;

/**
 * Build a tree given an infinitely nested data structure using Markdown syntax.
 *
 * @package Titon\Console\Tree
 */
class MarkdownTree extends AbstractTree {

    /**
     * {@inheritdoc}
     */
    protected function build(TreeData $tree, string $prefix = ''): string {
        if (is_null($tree)) {
            $tree = $this->data;
        }

        $output = [];

        foreach ($tree as $label => $branch) {
            if (is_string($branch)) {
                $label = $branch;
            }

            $output[] = "$prefix- $label";

            if (is_array($branch)) {
                $output[] = $this->build($branch, "$prefix  ");
            }
        }

        return implode("\n", $output);
    }

}
