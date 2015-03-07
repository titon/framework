<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Feedback;

use Titon\Console\Output;
use Titon\Console\System\SystemFactory;

class ProgressBar extends AbstractFeedback {

    protected string $bar = '=>';

    protected string $prefix = '{:message}  {:percent}% [';

    protected string $suffix = '] {:elapsed} / {:estimated}';

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
        $bar = str_repeat($this->bar[0], floor($completed * $size)) . $this->bar[1];
        $bar = substr(str_pad($bar, $size, ' '), 0, $size);

        $variables = Map {
            'prefix'   => $prefix,
            'feedback' => $bar,
            'suffix'   => $suffix
        };

        $this->output->out($this->format($this->format, $variables), Output::VERBOSITY_NORMAL, 1, Output::CR);
    }
}