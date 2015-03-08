<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Iterator;

/**
 * The `InputLexer` handles all parsing and pairing of the provided input.
 *
 * @package Titon\Console
 */
class InputLexer implements Iterator<RawInput> {

    /**
     * Data structure of all items that have yet to be retrieved.
     *
     * @var array<string>
     */
    public array<string> $items;

    /**
     * The current position in the `items` of the lexer.
     *
     * @var int
     */
    protected int $position = 0;

    /**
     * The current length of avaiable values remaining in the lexer.
     *
     * @var int
     */
    protected int $length = 0;

    /**
     * The current value the lexer is pointing to.
     *
     * @var \Titon\Console\RawInput
     */
    protected RawInput $current;

    /**
     * Whether the lexer is on its first item or not.
     *
     * @var bool
     */
    protected bool $first = true;

    /**
     * Construct a new `InputLexer` given the provided structure of inputs.
     *
     * @param array<string> $items  The items to traverse through
     */
    public function __construct(array<string> $items) {
        $this->items = $items;
        $this->length = count($items);
        $this->current = shape(
            'value' => '',
            'raw'   => ''
        );
    }

    /**
     * Retrieve the current item the lexer is pointing to.
     *
     * @return \Titon\Console\RawInput
     */
    public function current(): RawInput {
        return $this->current;
    }

    /**
     * Return whether the lexer has reached the end of its parsable items or not.
     *
     * @return bool
     */
    public function end(): bool {
        return ($this->position + 1) == $this->length;
    }

    /**
     * If the current item is a string of short input values or the string contains
     * a value a flag is representing, separate them and add them to the available
     * items to parse.
     *
     * @return void
     */
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

    /**
     * Return whether the given value is representing notation for an argument.
     *
     * @param string $value The value to check
     *
     * @return bool
     */
    public function isArgument(string $value): bool {
        return $this->isShort($value) || $this->isLong($value);
    }

    /**
     * Determine if the given value is representing a long argument (i.e., --foo).
     *
     * @param string $value The value to check
     *
     * @return bool
     */
    public function isLong(string $value): bool {
         return (0 === strncmp($value, '--', 2));
    }

    /**
     * Determine if the given value is representing a short argument (i.e., -f).
     *
     * @param string $value The value to check
     *
     * @return bool
     */
    public function isShort(string $value): bool {
        return !$this->isLong($value) && (0 === strncmp($value, '-', 1));
    }

    /**
     * Retrieve the current position of the lexer.
     *
     * @return int
     */
    public function key(): int {
        return $this->position;
    }

    /**
     * Progress the lexer to its next item (if available).
     *
     * @return void
     */
    public function next(): void {
        if ($this->valid()) {
            $this->shift();
        }
    }

    /**
     * Peek ahead to the next avaiable item without progressing the lexer.
     *
     * @return \Titon\Console\RawInput|null
     */
    public function peek(): ?RawInput {
        if (count($this->items) > 0) {
            return $this->processInput($this->items[0]);
        }

        return null;
    }

    /**
     * Create and return RawInput given a raw string value.
     *
     * @param string $input The value to create RawInput for
     *
     * @return \Titon\Console\RawInput
     */
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

    /**
     * {@inheritdoc}
     */
    public function rewind(): void {
        $this->shift();
        if ($this->first) {
            $this->position = 0;
            $this->first = false;
        }
    }

    /**
     * Progress the lexer to its next avaiable item. If the item contains a value
     * an argument is representing, separate them and add the value back to the
     * available items to parse.
     *
     * @return void
     */
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

    /**
     * Add an item back to the items that have yet to be parsed.
     *
     * @param string $item  The item to add
     *
     * @return void
     */
    public function unshift(string $item): void {
        array_unshift($this->items, $item);
        $this->length++;
    }

    /**
     * Return whether or not the lexer has any more items to parse.
     *
     * @return bool
     */
    public function valid(): bool {
        return ($this->position < $this->length);
    }
}