<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Feedback;

use Titon\Console\Feedback;
use Titon\Console\Output;

/**
 * `AbstractFeedback` class handles core functionality for calculating and
 * displaying the progress information.
 *
 * @package Titon\Console\Feedback
 */
abstract class AbstractFeedback implements Feedback {

    /**
     * Characters used in displaying the feedback in the output.
     *
     * @var string
     */
    protected Vector<string> $characterSequence = Vector {};

    /**
     * The current cycle out of the given total.
     *
     * @var int
     */
    protected int $current = 0;

    /**
     * The format the feedback indicator will be displayed as.
     *
     * @var string
     */
    protected string $format = '{:prefix}{:feedback}{:suffix}';

    /**
     * The current iteration of the feedback used to calculate the speed.
     *
     * @var int
     */
    protected int $iteration = 0;

    /**
     * The interval (in miliseconds) between updates of the indicator.
     *
     * @var int
     */
    protected int $interval = 100;

    /**
     * The max length of the characters in the character sequence.
     *
     * @var int
     */
    protected int $maxLength = 1;

    /**
     * The message to be displayed with the feedback.
     *
     * @var string
     */
    protected string $message;

    /**
     * The `Output` used for displaying the feedback information.
     *
     * @var \Titon\Console\Output
     */
    protected Output $output;

    /**
     * The template used to prefix the output.
     *
     * @var string
     */
    protected string $prefix = '{:message}  {:percent}% [';

    /**
     * The current speed of the feedback.
     *
     * @var float
     */
    protected float $speed = 0.0;

    /**
     * The time the feedback started.
     *
     * @var int
     */
    protected int $start = -1;

    /**
     * The template used to suffix the output.
     *
     * @var string
     */
    protected string $suffix = '] {:elapsed} / {:estimated}';

    /**
     * The current tick used to calculate the speed.
     *
     * @var int
     */
    protected int $tick = -1;

    /**
     * The feedback running time.
     *
     * @var int
     */
    protected int $timer = -1;

    /**
     * The total number of cycles expected for the feedback to take until finished.
     *
     * @var int
     */
    protected int $total = 0;

    /**
     * Create a new instance of the `Feedback`.
     *
     * @param int    $total     The total number of cycles
     * @param string $message   The message to be displayed with the feedback
     * @param int    $interval  The interval the feedback should update in
     */
    public function __construct(int $total = 0, string $message = '', int $interval = 100) {
        $this->output = Output::getInstance();
        $this->message = $message;
        $this->interval = $interval;
        $this->setTotal($total);
    }

    /**
     * {@inheritdoc}
     */
    public function advance(int $increment = 1): void {
        $this->current = min($this->total, $this->current + $increment);

        if ($this->shouldUpdate()) {
            $this->display();
        }
    }

    /**
     * Build and return all variables that are accepted when building the prefix
     * and suffix for the output.
     *
     * @return Map<string, mixed>
     */
    protected function buildOutputVariables(): FeedbackVariables {
        $message = $this->message;
        $percent = str_pad(floor($this->getPercentageComplete() * 100), 3);;
        $estimated = $this->formatTime((int)$this->estimateTimeRemaining());
        $elapsed = str_pad(
            $this->formatTime($this->getElapsedTime()), strlen($estimated)
        );

        $variables = shape(
            'message'   => $message,
            'percent'   => $percent,
            'elapsed'   => $elapsed,
            'estimated' => $estimated,
        );

        return $variables;
    }

    /**
     * Method used to format and output the display of the feedback.
     *
     * @param bool $finish If this is the finishing display of the feedback
     */
    abstract protected function display(bool $finish = false): void;

    /**
     * Given the speed and currently elapsed time, calculate the estimated time
     * remaining.
     *
     * @return float
     */
    protected function estimateTimeRemaining(): float {
        $speed = $this->getSpeed();
        if (is_null($speed) || !$this->getElapsedTime()) {
            return 0.0;
        }

        return round($this->total / $speed);
    }

    /**
     * {@inheritdoc}
     */
    public function finish(): void {
        $this->current = $this->total;
        $this->display(true);
        $this->output->out();
    }

    /**
     * Format the given template to replace denoted variables with their values.
     *
     * @param string $message   The template to format
     * @param        Map $      The variables available
     *
     * @return string
     */
    protected function format(string $message, Map<string, mixed> $args = Map {}): string {
        if ($args->isEmpty()) {
            return $message;
        }

        foreach ($args as $key => $value) {
            $message = str_replace('{:' . $key . '}', $value, $message);
        }

        return $message;
    }

    /**
     * Format the given time for output.
     *
     * @param int $time The timestamp to format
     */
    protected function formatTime(int $time): string {
        return floor($time / 60) . ':' . str_pad(
            $time % 60, 2, 0, STR_PAD_LEFT
        );
    }

    /**
     * Retrieve the current elapsed time.
     *
     * @var int
     */
    protected function getElapsedTime(): int {
        if ($this->start < 0) {
            return 0;
        }

        return (time() - $start);
    }

    /**
     * Retrieve the percentage complete based on the current cycle and the total
     * number of cycles.
     *
     * @return float
     */
    protected function getPercentageComplete(): float {
        if ($this->total == 0) {
            return 1.0;
        }

        return (float)($this->current / $this->total);
    }

    /**
     * Get the current speed of the feedback.
     *
     * @return float
     */
    protected function getSpeed(): float {
        if ($this->start < 0) {
            return 0.0;
        }

        if ($this->tick < 0) {
            $this->tick = $this->start;
        }

        $now = microtime(true);
        $span = $now - $this->tick;
        if ($span > 1) {
            $this->iteration++;
            $this->tick = $now;
            $this->speed = (float)(($this->current / $this->iteration) / $span);
        }

        return $this->speed;
    }

    /**
     * Retrieve the total number of cycles the feedback should take.
     *
     * @return int
     */
    protected function getTotal(): int {
        return number_format($this->total);
    }

    /**
     * Set the characters used in the output.
     *
     * @param Vector<string> $characters    The characters to use
     *
     * @return $this
     */
    public function setCharacterSequence(Vector<string> $characters): this {
        $this->characterSequence = $characters;
        $this->setMaxLength();

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setInterval(int $interval): this {
        $this->interval = $interval;

        return $this;
    }

    /**
     * Set the maximum length of the available character sequence characters.
     *
     * @return $this
     */
    protected function setMaxLength(): this {
        $this->maxLength = max(
            $this->characterSequence->map(($key) ==> strlen($key))->toArray()
        );

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setMessage(string $message): this {
        $this->message = $message;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setPrefix(string $prefix): this {
        $this->prefix = $prefix;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setSuffix(string $sufix): this {
        $this->suffix = $sufix;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setTotal(int $total): this {
        $this->total = (int)$total;

        return $this;
    }

    /**
     * Determine if the feedback should update its output based on the current
     * time, start time, and set interval.
     *
     * @return bool
     */
    protected function shouldUpdate(): bool {
        $now = microtime(true) * 1000;

        if ($this->timer < 0) {
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
