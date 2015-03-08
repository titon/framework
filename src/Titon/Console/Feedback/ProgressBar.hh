<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Feedback;

use Titon\Console\Output;
use Titon\Console\System\SystemFactory;

/**
 * The `ProgressBar` class displays feedback information with a progress bar.
 * Additional information including percentage done, time elapsed, and time
 * remaining is included by default.
 *
 * @package Titon\Console\Feedback
 */
class ProgressBar extends AbstractFeedback {

    /**
     * The 2-string character format to use when constructing the displayed bar.
     *
     * @var string
     */
    protected Vector<string> $characterSequence = Vector {
        '=',
        '>'
    };

    /**
     * {@inheritdoc}
     */
    protected string $prefix = '{:message}  {:percent}% [';

    /**
     * {@inheritdoc}
     */
    protected string $suffix = '] {:elapsed} / {:estimated}';

    /**
     * {@inheritdoc}
     */
    protected function display(bool $finish = false): void {
        $completed = $this->getPercentageComplete();
        $variables = $this->buildOutputVariables();

        // Need to make prefix and suffix before the bar so we know how long
        // to render it.
        $prefix = $this->format($this->prefix, $variables);
        $suffix = $this->format($this->suffix, $variables);

        $size = SystemFactory::factory()->getWidth();
        $size -= strlen($prefix . $suffix);
        if ($size < 0) {
            $size = 0;
        }

        // substr is needed to trim off the bar cap at 100%
        $bar = str_repeat($this->characterSequence[0], floor($completed * $size)) . $this->characterSequence[1];
        $bar = substr(str_pad($bar, $size, ' '), 0, $size);

        $variables = Map {
            'prefix'   => $prefix,
            'feedback' => $bar,
            'suffix'   => $suffix
        };

        $this->output->out($this->format($this->format, $variables), Output::VERBOSITY_NORMAL, 1, Output::CR);
    }

    /**
     * {@inheritdoc}
     */
    public function setCharacterSequence(Vector<string> $characters): this {
        if ($characters->count() !== 2) {
            throw new InvalidCharacterSequence("Display bar must only contain 2 values");
        }

        parent::setCharacterSequence($characters);
    }
}