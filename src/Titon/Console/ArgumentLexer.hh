<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Iterator;

class ArgumentLexer implements Iterator {

    protected array<string> $items = array();

    protected int $position;

    protected int $length = 0;

    protected ?string $current;

    protected bool $first = true;

    public function __construct(array<string> $items = array()) {
        $this->items = $items;
        $this->length = count($items);
        $this->position = 0;
    }

    public function current(): mixed {
        return $this->items[$this->position];
    }

    public function end(): bool {
        return ($this->position + 1) === $this->length;
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

    public function peek(): string {
        return $this->processInput($this->items[0]);
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
            $this->position += 1;
        }

        $this->current = $this->processInput($key);

        if ($this->isShort($key['raw']) && strlen($key['value']) > 1) {
            $exploded = array();
            for ($i = strlen($key['value']); $i > 0; $i--) {
                array_push($exploded, $key['value'][$i - 1]);
            }
            $key['value'] = array_pop($exploded);
            $key['raw'] = '-' . $key['value'];

            foreach ($exploded as $piece) {
                $this->unshift('-' . $piece);
            }
        }
    }

    public function unshift(string $item): void {
        array_unshift($this->items, $item);
        $this->length += 1;
    }

    public function valid(): bool {
        return $this->position < count($this->items);
    }
}