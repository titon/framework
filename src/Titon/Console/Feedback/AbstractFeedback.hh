<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Feedback;

use Titon\Console\Feedback;
use Titon\Console\Output;

abstract class AbstractFeedback implements Feedback {

    protected int $current = 0;

    protected string $format = '{:prefix}{:feedback}{:suffix}';

    protected int $interval;

    protected string $message;

    protected Output $output;

    protected string $prefix = '{:message}  {:percent}% [';

    protected ?int $start;

    protected string $suffix = '] {:elapsed} / {:estimated}';

    protected ?int $timer;

    protected int $total = 0;

    public function __construct(int $total = 0, string $message = '', int $interval = 100) {
        $this->output = new Output();
        $this->message = $message;
        $this->interval = $interval;
        $this->setTotal($total);
    }

    public function advance(int $increment = 1): void {
        $this->current = min($this->total, $this->current + $increment);

        if ($this->shouldUpdate()) {
            $this->display();
        }
    }

    protected function buildOutputVariables(): Map<string, mixed> {
        $message = $this->message;
        $percent = str_pad(floor($this->getPercentageComplete() * 100), 3);;
        $estimated = $this->formatTime((int)$this->estimateTimeRemaining());
        $elapsed = str_pad(
            $this->formatTime($this->getElapsedTime()), strlen($estimated)
        );

        $variables = Map {
            'message'   => $message,
            'percent'   => $percent,
            'elapsed'   => $elapsed,
            'estimated' => $estimated,
        };

        return $variables;
    }

    protected function current(): int {
        $size = strlen($this->getTotal());

        return str_pad(number_format($this->current), $size);
    }

    abstract protected function display(bool $finish = false): void;

    protected function estimateTimeRemaining(): float {
        $speed = $this->getSpeed();
        if (is_null($speed) || !$this->getElapsedTime()) {
            return 0.0;
        }

        return round($this->total / $speed);
    }

    public function finish(): void {
        $this->current = $this->total;
        $this->display(true);
        $this->output->out();
    }

    protected function format(string $message, Map<string, mixed> $args = Map {}): string {
        if ($args->isEmpty()) {
            return $message;
        }

        foreach ($args as $key => $value) {
            $message = str_replace('{:' . $key . '}', $value, $message);
        }

        return $message;
    }

    protected function formatTime(int $time): string {
        return floor($time / 60) . ':' . str_pad(
            $time % 60, 2, 0, STR_PAD_LEFT
        );
    }

    protected function getElapsedTime(): int {
        if (is_null($this->start)) {
            return 0;
        }

        return (time() - $this->start);
    }

    protected function getPercentageComplete(): float {
        if ($this->total == 0) {
            return 1.0;
        }

        return (float)($this->current / $this->total);
    }

    protected function getSpeed(): float {
        static $tick, $iteration = 0, $speed = 0;

        if (is_null($this->start)) {
            return 0.0;
        }

        if (is_null($tick)) {
            $tick = $this->start;
        }

        $now = microtime(true);
        $span = $now - $tick;
        if ($span > 1) {
            $iteration++;
            $tick = $now;
            $speed = ($this->current / $iteration) / $span;
        }

        return (float)$speed;
    }

    protected function getTotal(): mixed {
        return number_format($this->total);
    }

    public function setMessage(string $message): this {
        $this->message = $message;

        return $this;
    }

    public function setPrefix(string $prefix): this {
        $this->prefix = $prefix;

        return $this;
    }

    public function setSuffix(string $sufix): this {
        $this->suffix = $sufix;

        return $this;
    }

    public function setTotal(int $total): this {
        $this->total = (int)$total;

        return $this;
    }

    protected function shouldUpdate(): bool {
        $now = microtime(true) * 1000;

        if (is_null($this->timer)) {
            $this->start = (int)(($this->timer = $now) / 1000);

            return true;
        }

        if (($now - $this->timer) > $this->interval) {
            $this->timer = $now;

            return true;
        }

        return false;
    }
}
