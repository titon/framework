<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Table;

/**
 * The `TabDelimitedTable` class builds and outputs a table with values tab-delimited
 * for use in other applications.
 *
 * @package Titon\Console\Table
 */
class TabDelimitedTable extends AbstractTable {

    /**
     * Build the table and return its markup.
     *
     * @return string
     */
    public function render(): string {
        $retval = Vector {};

        $retval[] = implode("\t", $this->headers);
        foreach ($this->rows as $row) {
            $retval[] = implode("\t", $row);
        }

        return trim(implode("\n", $retval));
    }
}