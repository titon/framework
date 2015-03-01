<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Table;

class AsciiTable extends AbstractTable {

    protected Map<string, string> $characters = array(
        'corner'      => '+',
        'line'        => '-',
        'header_line' => '=',
        'border'      => '|',
        'padding'     => ' ',
    );

    protected ?int $rowWidth;

    protected function pad(string $value, string $pad = ''): string {
        return "$pad$value$pad";
    }

    protected function buildBorder(bool $header = false): string {
        if ($header) {
            return str_repeat($this->characters['header_line'], $this->getRowWidth());
        }

        return str_repeat($this->characters['line'], $this->getRowWidth());
    }

    protected function buildCell(string $value, int $key): string {
        return $this->pad(str_pad($value, $this->columnWidths->get($key)), $this->characters['padding']);
    }

    protected function buildHeader(): string {
        $retval = Vector {};

        if (!$this->headers->isEmpty()) {
            $headers = [];
            foreach ($this->headers as $index => $column) {
                $headers[] = $this->characters['padding'] . str_pad($column, $this->columnWidths->get($index)) . $this->characters['padding'];
            }
            $headers = implode($this->characters['border'], $headers);
            if ($headers) {
                $headers = $this->pad($headers, $this->characters['border']);
                $border = $this->buildBorder(true);
                $retval[] = $border;
                $retval[] = $headers;
                $retval[] = $border;
            }
        } else {
            $retval[] = $this->buildBorder();
        }

        return implode("\n", $retval);
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
            $this->rowWidth = strlen($this->buildRow($this->rows->get(0)));
        }

        return $this->rowWidth;
    }

    public function render(): string {
        $retval = Vector {};

        $retval[] = $this->buildHeader();

        foreach ($this->rows as $row) {
            $retval[] = $this->buildRow($row);
            $retval[] = $this->buildBorder();
        }

        return implode("\n", $retval);
    }
}