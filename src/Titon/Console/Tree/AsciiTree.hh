<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Tree;

class AsciiTree extends AbstractTree {

    protected function build(?array<string> $tree = null, string $prefix = ''): string {
        if (is_null($tree)) {
            $tree = $this->data;
        }

        $retval = [];

        $keys = array_keys($tree);
        $branch = array_values($tree);
        for ($i = 0, $count = count($branch); $i < $count; ++$i) {
            if (is_array($branch[$i])) {
                $retval[] = $prefix . '+-' . $keys[$i];
            } else {
                $retval[] = $prefix . '+-' . $branch[$i];
            }

            if (is_array($branch[$i])) {
                $retval[] = $this->build($branch[$i], $prefix . ($i == $count - 1 ? '  ' : '| '));
            }
        }

        return implode("\n", $retval);
    }
}