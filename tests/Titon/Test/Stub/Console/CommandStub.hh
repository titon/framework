<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Test\Stub\Console;

use Titon\Console\Command\AbstractCommand;
use Titon\Console\Feedback\ProgressBarFeedback;
use Titon\Console\Feedback\WaitFeedback;
use Titon\Console\InputDefinition\Argument;
use Titon\Console\InputDefinition\Option;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\Tree\AsciiTree;
use Titon\Console\Tree\MarkdownTree;
use Titon\Console\Table\AsciiTable;
use Titon\Console\UserInput\Menu;

class CommandStub extends AbstractCommand {

    public function configure(): void {
        $this->setName('demo')->setDescription('Provide demos for all built in functionalities.');

        $this->addFlag(new Flag('table', 'Output a nice sample table'));

        $this->addOption(new Option('tree', 'Output either an `ascii` or `markdown` tree'));

        $this->addFlag(new Flag('greet', 'Output a friendly greeting!'));
        $this->addArgument(new Argument('name', 'The name of the person I am greeting.'));

        $this->addOption(new Option('feedback', 'Display either a `progress` bar or a `spinner`.'));

        $this->addFlag(new Flag('menu', 'Present a menu to help guide you through the examples.'));
    }

    public function run(): void {
        if (($type = $this->getOption('tree', '')) !== '') {
            invariant(is_string($type), 'Must be a string.');
            $this->runTreeExample($type);

        } else if ($this->getFlag('table') === 1) {
            $this->runTableExample();

        } else if ($this->getFlag('greet')) {
            if (($name = $this->getArgument('name', '')) !== '') {
                $this->out(sprintf('Hello, %s!', $name));
            } else {
                $this->out('Hello, world!');
            }

        } else if (($type = $this->getOption('feedback', '')) !== '') {
            invariant(is_string($type), 'Must be a string.');
            $this->runFeedbackExample($type);

        } else if ($this->getFlag('menu')) {
            $this->runUserInputExamples();

        } else {
            $this->runUserInputExamples();
        }
    }

    protected function runFeedbackExample(string $type): void {
        if ($type === 'progress') {
            $feedback = $this->progressBar(100, 'Please wait');
        } else if ($type === 'spinner') {
            $feedback = $this->wait(100, 'Please wait ');
        } else {
            return;
        }

        for ($i = 0; $i < 100; $i++) {
            $feedback->advance();
            usleep(10000);
        }
    }

    protected function runTableExample(): void {
        $data = Vector{
            Map {
                'Company'          => 'Apple',
                'Operating System' => 'Mac OS X',
                'Latest Release'   => 'Yosemite',
            },
            Map {
                'Company'          => 'Microsoft',
                'Operating System' => 'Windows',
                'Latest Release'   => '10',
            },
            Map {
                'Company'          => 'RedHat',
                'Operating System' => 'RHEL',
                'Latest Release'   => '7',
            },
        };

        $table = new AsciiTable();
        $table->setData($data);

        $this->out($table->render());
    }

    protected function runTreeExample(string $type): void {
        $data = [
            'Titon' => [
                'Console' => [
                    'Command',
                    'Exception',
                    'Feedback',
                    'InputDefinition',
                    'System',
                    'Table',
                    'Tree' => [
                        'AbstractTree.hh',
                        'AsciiTree.hh',
                        'MarkdownTree.hh'
                    ],
                    'UserInput',
                ],
                0 => 'Utility'
            ],
        ];

        if ($type === 'ascii') {
            $tree = new AsciiTree($data);
        } else if ($type === 'markdown') {
            $tree = new MarkdownTree($data);
        } else {
            $this->error(sprintf('The table type %s is not supported.', $type));

            return;
        }

        $this->out($tree->render());
    }

    protected function runUserInputExamples(): void {
        $confirm = $this->confirm('y');
        if (!$confirm->confirmed('<warning>Are you sure you want a menu?</warning>')) {
            return;
        }

        $menu = $this->menu(Map {
            '0' => 'ASCII Tree',
            '1' => 'Markdown Tree',
            '2' => 'Table',
            '3' => 'Greeting',
            '4' => 'Progress bar',
            '5' => 'Spinner',
            '6' => 'Repeat this menu',
            '7' => 'Quit'
        }, 'Please select from the options below');

        $answer = $menu->prompt('Enter a selection:');

        switch ((int)$answer) {
            case 0:
                $this->runTreeExample('ascii');
                break;
            case 1:
                $this->runTreeExample('markdown');
                break;
            case 2:
                $this->runTableExample();
                break;
            case 3:
                $this->out('Hello, world!');
                break;
            case 4:
                $this->runFeedbackExample('progress');
                break;
            case 5:
                $this->runFeedbackExample('spinner');
                break;
            case 6;
                $this->runUserInputExamples();
                break;
            default:
                $this->out('<info>Bye!</info>');
                return;
        }
    }
}
