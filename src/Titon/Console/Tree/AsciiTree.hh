<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Tree;

/**
 * Build a human readable ASCII tree given an infinitely nested data structure.
 *
 * @package Titon\Console\Tree
 */
class AsciiTree extends AbstractTree {

    /**
     * {@inheritdoc}
     */
    protected function build(Vector<mixed> $tree, string $prefix = ''): string {
        if (is_null($tree)) {
            $tree = $this->data;
        }

        $retval = [];

        $keys = array_keys($tree);
        $branch = array_values($tree);
        for ($i = 0, $count = count($branch); $i < $count; ++$i) {
            if (is_array($branch[$i])) {
                $retval[] = $prefix . '+-' . (string)$keys[$i];
            } else {
                $retval[] = $prefix . '+-' . (string)$branch[$i];
            }

            $next = $branch[$i];
            if ($next instanceof Vector) {
                $retval[] = $this->build($next, $prefix . ($i == $count - 1 ? '  ' : '| '));
            }
        }

        return implode("\n", $retval);
    }
}