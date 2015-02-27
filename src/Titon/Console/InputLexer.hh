<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Common\ArgumentList;

class InputLexer implements Iterator<RawInput> {

    public array<string> $items;

    protected int $position = 0;

    protected int $length = 0;

    protected RawInput $current;

    protected bool $first = true;

    public function __construct(array<string> $items) {
        $this->items = $items;
        $this->length = count($items);
        $this->current = shape(
            'value' => '',
            'raw'   => ''
        );
    }

    public function current(): RawInput {
        return $this->current;
    }

    public function end(): bool {
        return ($this->position + 1) == $this->length;
    }

    private function explode(): void {
        if (!$this->isShort($this->current['raw']) || strlen($this->current['value']) <= 1) {
            return;
        }

        $exploded = str_split($this->current['value']);

        $this->current = shape(
            'value' => array_pop($exploded),
            'raw'   => '-' . $this->current['value']
        );

        foreach ($exploded as $piece) {
            $this->unshift('-' . $piece);
        }
    }

    public function isArgument(string $value): bool {
        return $this->isShort($value) || $this->isLong($value);
    }

    public function isLong(string $value): bool {
         return (0 == strncmp($value, '--', 2));
    }

    public function isShort(string $value): bool {
        return !$this->isLong($value) && (0 == strncmp($value, '-', 1));
    }

    public function key(): int {
        return $this->position;
    }

    public function next(): void {
        if ($this->valid()) {
            $this->shift();
        }
    }

    public function peek(): ?RawInput {
        if (count($this->items) > 0) {
            return $this->processInput($this->items[0]);
        }

        return null;
    }

    public function processInput(string $input): RawInput {
        $raw = $input;
        $value = $input;

        if ($this->isLong($input)) {
            $value = substr($input, 2);
        } else {
            if ($this->isShort($input)) {
                $value = substr($input, 1);
            }
        }

        return shape(
            'raw'   => $raw,
            'value' => $value
        );
    }

    public function rewind(): void {
        $this->shift();
        if ($this->first) {
            $this->position = 0;
            $this->first = false;
        }
    }

    public function shift(): void {
        $key = array_shift($this->items);

        $matches = [];
        if (preg_match('#\A([^\s\'"=]+)=(.+?)$#', $key, $matches)) {
            $key = $matches[1];
            array_unshift($this->items, $matches[2]);
        } else {
            $this->position++;
        }

        if (is_null($key)) {
            return;
        }

        $this->current = $this->processInput($key);

        $this->explode();
    }

    public function unshift(string $item): void {
        array_unshift($this->items, $item);
        $this->length++;
    }

    public function valid(): bool {
        return ($this->position < $this->length);
    }
}