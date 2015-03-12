<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Feedback;

use Titon\Utility\Str;
use Titon\Console\Output;
use Titon\Console\System\SystemFactory;

/**
 * The `Wait` class displays feedback by cycling through a series of characters.
 *
 * @package Titon\Console\Feedback
 */
class Wait extends AbstractFeedback {

    /**
     * {@inheritdoc}
     */
    protected Vector<string> $characterSequence = Vector {
        '-',
        '\\',
        '|',
        '/'
    };

    /**
     * {@inheritdoc}
     */
    protected string $prefix = " {message}";

    /**
     * {@inheritdoc}
     */
    protected string $suffix = "";

    /**
     * {@inheritdoc}
     */
    public function __construct(Output $output, int $total = 0, string $message = '', int $interval = 100) {
        parent::__construct($output, $total, $message, $interval);
        $this->iteration = 0;
    }

    /**
     * {@inheritdoc}
     */
    public function display(bool $finish = false): void {
        $variables = $this->buildOutputVariables();

        $index = $this->iteration++ % $this->characterSequence->count();
        $feedback = str_pad($this->characterSequence[$index], $this->maxLength + 1);

        $prefix = Str::insert($this->prefix, $variables);
        $suffix = Str::insert($this->suffix, $variables);

        $variables = Map {
            'prefix'   => $prefix,
            'feedback' => $feedback,
            'suffix'   => $suffix
        };

        $eol = Output::CR;
        if ($finish === true) {
            $eol = Output::LF;
        }

        $this->output->out(
            str_pad(Str::insert(
                $this->format, $variables
            ), SystemFactory::factory()->getWidth()),
            1,
            Output::VERBOSITY_NORMAL,
            $eol
        );
    }
}