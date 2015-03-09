<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Table;

/**
 * The `AsciiTable` object with output a human readable ASCII table in of the
 * provided data.
 *
 * @package Titon\Console\Table
 */
class AsciiTable extends AbstractTable {

    /**
     * A string containing the output for a border.
     *
     * @var string
     */
    protected ?string $border;

    /**
     * A Map containing necessary characters used for building the table.
     *
     * @var Map<string, string>
     */
    protected Map<string, string> $characters = Map {
        'corner'      => '+',
        'line'        => '-',
        'header_line' => '=',
        'border'      => '|',
        'padding'     => ' ',
    };

    /**
     * The integer length of the width each row should be.
     *
     * @var int|null
     */
    protected int $rowWidth = 0;

    /**
     * Given a string value and a padding string, return the value with the pad
     * appended and prepended to the value.
     *
     * @param string $value The value to pad
     * @param string $pad   The string of characters to pad the value with
     *
     * @return string
     */
    protected function pad(string $value, string $pad = ''): string {
        return "$pad$value$pad";
    }

    /**
     * Build a border for the width of the row width for the class and using the
     * class's `characters`.
     *
     * @return string
     */
    protected function buildBorder(): string {
        if (is_null($this->border)) {
            $this->border = '+';
            foreach ($this->columnWidths as $width) {
                $this->border .= $this->characters['padding'];
                $this->border .= str_repeat('-', $width);
                $this->border .= $this->characters['padding'];
                $this->border .= '+';
            }
        }

        return $this->border;
    }

    /**
     * Build a single cell of the table given the data and the key of the column
     * the data should go into.
     *
     * @param string $value The value of the cell
     * @param int    $key   The index of the column
     *
     * @return string
     */
    protected function buildCell(string $value, int $key): string {
        return $this->pad(str_pad($value, $this->columnWidths->get($key)), $this->characters['padding']);
    }

    /**
     * Given a Vector of data, build a single row of the table.
     *
     * @param Vector<mixed> The row data
     *
     * @return string
     */
    protected function buildRow(Vector<mixed> $data): string {
        $row = [];
        foreach ($data as $index => $value) {
            $row[] = $this->buildCell((string)$value, $index);
        }
        $row = $this->pad(implode($this->characters['border'], $row), $this->characters['border']);

        return $row;
    }

    /**
     * Retrieve the width that each row in the table should be.
     *
     * @return int
     */
    protected function getRowWidth(): int {
        if (is_null($this->rowWidth)) {
            if (!is_null($this->rows[0])) {
                $this->rowWidth = strlen($this->buildRow($this->rows[0]));
            }
        }

        return $this->rowWidth;
    }

    /**
     * Build the table and return its markup.
     *
     * @return string
     */
    public function render(): string {
        $retval = Vector {};

        if ($header = $this->buildRow($this->headers)) {
            $retval[] = $this->buildBorder();
            $retval[] = $header;
        }

        $retval[] = $this->buildBorder();

        foreach ($this->rows as $row) {
            $retval[] = $this->buildRow($row);
        }

        $retval[] = $this->buildBorder();

        return implode("\n", $retval);
    }
}