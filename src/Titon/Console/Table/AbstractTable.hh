<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Table;

use Titon\Console\Table;

abstract class AbstractTable implements Table {

    protected Vector<int> $columnWidths = Vector {};

    protected Vector<string> $headers = Vector {};

    protected Vector<Vector<string>> $rows = Vector {};

    public function addRow(Vector<string> $row): this {
        $this->setColumnWidths($row);
        $this->rows[] = $row;

        return $this;
    }

    public function setColumnWidths(Vector<string> $row): void {
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

    public function setData(Vector<Map<string, string>> $data): this {
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

    public function setHeaders(Vector<string> $headers): this {
        $this->setColumnWidths($headers);
        $this->headers = $headers;

        return $this;
    }

    public function setRows(Vector<Vector<string>> $rows): this {
        $this->rows->clear();
        foreach ($rows as $row) {
            $this->addRow($row);
        }

        return $this;
    }
}