<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Table;

use Titon\Console\Table;

/**
 * The `AbstractTable` class provides the core functionality for building and
 * displaying tabular data.
 *
 * @package Titon\Console\Table
 */
abstract class AbstractTable implements Table {

    /**
     * Data structure that holds the width of each column.
     *
     * @var Vector<int>
     */
    protected Vector<int> $columnWidths = Vector {};

    /**
     * Data structure holding the header names of each column.
     *
     * @var Vector<string>
     */
    protected Vector<string> $headers = Vector {};

    /**
     * Data structure holding the data for each row in the table.
     *
     * @var Vector<Vector<string>>
     */
    protected Vector<Vector<mixed>> $rows = Vector {};

    /**
     * Append a new row of data to the end of the existing rows.
     *
     * @param Vector<string> $row   Vector containing the row data
     *
     * @return $this
     */
    public function addRow(Vector<mixed> $row): this {
        $this->setColumnWidths($row);
        $this->rows[] = $row;

        return $this;
    }

    /**
     * Given the row of data, adjust the column width accordingly so that the
     * columns width is that of the maximum data field size.
     *
     * @param Vector<string> $row   Vector containing the row data
     *
     * @return $this
     */
    protected function setColumnWidths(Vector<mixed> $row): void {
        foreach ($row as $index => $value) {
            $width = strlen($value);
            if (is_null($currentWidth = $this->columnWidths->get($index)) || $width > $currentWidth) {
                if ($this->columnWidths->count() === $index) {
                    $this->columnWidths[] = $width;
                } else {
                    $this->columnWidths[$index] = $width;
                }
            }
        }
    }

    /**
     * Set the data of the table with a Vector of column name and value Maps.
     * This method overwrites any existing rows in the table.
     *
     * @param Vector<Map<string, string>> $data A Vector containing Maps of column
     *                                          name and data key-value pairs.
     *
     * @return $this
     */
    public function setData(Vector<Map<string, mixed>> $data): this {
        $rows = Vector {};
        $headers = Vector {};
        foreach ($data as $row) {
            foreach ($row as $column => $value) {
                if ($headers->linearSearch($column) < 0) {
                    $headers[] = $column;
                }
            }

            $rows[] = $row->values();
        }

        $this->setRows($rows);
        $this->setHeaders($headers);

        return $this;
    }

    /**
     * Set the column names for the table.
     *
     * @param Vector<string> $headers   A Vector containing column names
     *
     * @return $this
     */
    public function setHeaders(Vector<string> $headers): this {
        $this->setColumnWidths($headers);
        $this->headers = $headers;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setRows(Vector<Vector<mixed>> $rows): this {
        $this->rows->clear();
        foreach ($rows as $row) {
            $this->addRow($row);
        }

        return $this;
    }
}