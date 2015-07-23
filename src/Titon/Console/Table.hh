<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

/**
 * A `Table` object will construct the markup for a human readable (or otherwise
 * parsable) representation of tabular data.
 *
 * @package Titon\Console
 */
interface Table {

    /**
     * Add a row of data to the end of the existing data.
     *
     * @param Vector<mixed> $row    A Vector containing the row of data
     *
     * @return $this
     */
    public function addRow(Vector<mixed> $row): this;

    /**
     * Build and return the markup for the `Table`.
     *
     * @return string
     */
    public function render(): string;

    /**
     * Set the data of the table with a Vector of column name and value Maps.
     * This method overwrites any existing rows in the table.
     *
     * @param Vector<Map<string, string>> $data A Vector containing Maps of column
     *                                          name and data key-value pairs.
     *
     * @return $this
     */
    public function setData(Vector<Map<string, mixed>> $data): this;

    /**
     * Set the column names for the table.
     *
     * @param Vector<string> $headers   A Vector containing column names
     *
     * @return $this
     */
    public function setHeaders(Vector<mixed> $headers): this;

    /**
     * Set the data for the rows in the table with a Vector containing a Vector
     * for each row in the table.
     *
     * @param Vector<Vector<mixed> The Vector containin the row data
     *
     * @return $this
     */
    public function setRows(Vector<Vector<mixed>> $rows): this;
}