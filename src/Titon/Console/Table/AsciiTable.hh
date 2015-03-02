<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Table;

class AsciiTable extends AbstractTable {

    protected ?string $border;

    protected Map<string, string> $characters = Map {
        'corner' => '+',
        'line' => '-',
        'header_line' => '=',
        'border' => '|',
        'padding' => ' ',
    };

    protected ?int $rowWidth;

    protected function pad(string $value, string $pad = ''): string {
        return "$pad$value$pad";
    }

    protected function buildBorder(bool $header = false): string {
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

    protected function buildCell(string $value, int $key): string {
        return $this->pad(str_pad($value, $this->columnWidths->get($key)), $this->characters['padding']);
    }

    protected function buildRow(Vector<string> $data): string {
        $row = [];
        foreach ($data as $index => $value) {
            $row[] = $this->buildCell($value, $index);
        }
        $row = $this->pad(implode($this->characters['border'], $row), $this->characters['border']);

        return $row;
    }

    protected function getRowWidth(): int {
        if (is_null($this->rowWidth)) {
            if (!is_null($this->rows[0])) {
                $this->rowWidth = strlen($this->buildRow($row));
            }
        }

        return $this->rowWidth;
    }

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