<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Tree;

/**
 * Build a tree given an infinitely nested data structure using Markdown syntax.
 *
 * @package Titon\Console\Tree
 */
class MarkdownTree extends AbstractTree {

    /**
     * {@inheritdoc}
     */
    protected function build(array<mixed> $tree, string $prefix = ''): string {
        if (is_null($tree)) {
            $tree = $this->data;
        }

        $retval = [];

        foreach ($tree as $label => $branch) {
            if (is_string($branch)) {
                $label = $branch;
            }

            $retval[] = "$prefix- $label";

            if (is_array($branch)) {
                $retval[] = $this->build($branch, "$prefix  ");
            }
        }

        return implode("\n", $retval);
    }
}