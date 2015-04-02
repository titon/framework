<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Tree;

use Titon\Console\TreeData;

/**
 * Build a human readable ASCII tree given an infinitely nested data structure.
 *
 * @package Titon\Console\Tree
 */
class AsciiTree extends AbstractTree {

    /**
     * {@inheritdoc}
     */
    protected function build(TreeData $tree, string $prefix = ''): string {
        if (is_null($tree)) {
            $tree = $this->data;
        }

        $retval = [];

        $keys = array_keys($tree);
        $branch = array_values($tree);
        for ($i = 0, $count = count($branch); $i < $count; ++$i) {
            $itemPrefix = $prefix;
            $next = $branch[$i];

            if ($i === $count - 1) {
                if (is_array($next)) {
                    $itemPrefix .= '└─┬';
                } else {
                    $itemPrefix .= '└──';
                }
            } else {
                if (is_array($next)) {
                    $itemPrefix .= '├─┬';
                } else {
                    $itemPrefix .= '├──';
                }
            }

            if (is_array($branch[$i])) {
                $retval[] = $itemPrefix . (string)$keys[$i];
            } else {
                $retval[] = $itemPrefix . (string)$branch[$i];
            }

            if (is_array($next)) {
                $retval[] = $this->build($next, $prefix . ($i == $count - 1 ? '  ' : '| '));
            }
        }

        return implode("\n", $retval);
    }
}