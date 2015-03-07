<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Feedback;

use Titon\Console\Output;
use Titon\Console\System\SystemFactory;

class Wait extends AbstractFeedback {

    protected Vector<string> $characterSequence = Vector {
        '-',
        '\\',
        '|',
        '/'
    };

    protected int $iteration = 0;

    protected int $maxLength = 1;

    protected string $prefix = " {:message}";

    protected string $suffix = "";

    public function __construct(int $total = 0, string $message = '', int $interval = 100) {
        parent::__construct($total, $message, $interval);
    }

    public function display(bool $finish = false): void {
        $variables = $this->buildOutputVariables();

        $index = $this->iteration++ % $this->characterSequence->count();
        $feedback = str_pad($this->characterSequence[$index], $this->maxLength + 1);

        $prefix = $this->format($this->prefix, $variables);
        $suffix = $this->format($this->suffix, $variables);

        $variables = Map {
            'prefix'   => $prefix,
            'feedback' => $feedback,
            'suffix'   => $suffix
        };

        $this->output->out(
            str_pad($this->format(
                $this->format, $variables
            ), SystemFactory::factory()->getWidth()),
            1,
            Output::VERBOSITY_NORMAL,
            Output::CR
        );
    }

    public function setCharacterSequence(Vector<string> $characters): this {
        $this->characterSequence = $characters;
        $this->setMaxLength();

        return $this;
    }

    protected function setMaxLength(): this {
        $this->maxLength = max(array_map(($key) ==> {
            return strlen($key);
        }, $this->characterSequence));

        return $this;
    }
}