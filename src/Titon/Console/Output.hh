<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

class Output {

    const VERBOSITY_QUIET = 0;
    const VERBOSITY_NORMAL = 1;
    const VERBOSITY_VERBOSE = 3;
    const STREAM_STDOUT = 'php://stdout';
    const STREAM_STDERR = 'php://stderr';
    const ANSI_CLEAR_LINE = "\033[K";

    protected resource $stderr;

    protected resource $stdout;

    protected Map<sting, StyleFormatter> $styles = Map {};

    protected int $systemVerbosity;

    public function __construct(int $verbosity = self::VERBOSITY_NORMAL) {
        $this->stderr = fopen(Output::STREAM_STDERR, 'w');
        $this->stdout = fopen(Output::STREAM_STDOUT, 'w');
        $this->systemVerbosity = $verbosity;
    }

    public function error(string $output, int $verbosity = Output::VERBOSITY_NORMAL, int $newLines = 1): void {
        if (!$this->shouldOutput($verbosity)) {
            return;
        }

        $output = $this->format($output);
        $output .= str_repeat("\n", $newLines);

        fwrite($this->stderr, $output);
    }

    public function out(string $output, int $verbosity = Output::VERBOSITY_NORMAL, int $newLines = 1): void {
        if (!$this->shouldOutput($verbosity)) {
            return;
        }

        $output = $this->format($output);
        $output .= str_repeat("\n", $newLines);

        fwrite($this->stdout, $output);
    }

    public function format(string $message): string {
        $parsedTags = array_unique(XmlParser::parseTags($message));
        $formatMessage = $message;
        foreach ($parsedTags as $xmlTag) {
            if (!empty($this->styles[$xmlTag])) {
                $formatter = $this->styles[$xmlTag];
                $formatMessage = $formatter->format($xmlTag, $formatMessage);
                $formatMessage = preg_replace_callback('#<[\w-]+?>.*<\/[\w-]+?>#', function($matches) use ($formatter) {
                    return $formatter->getEndCode() . $this->format($matches[0]) . $formatter->getStartCode();
                }, $formatMessage);
            }
        }

        return $formatMessage;
    }

    public function setFormat(string $element, StyleDefinition $format): this {
        $this->styles[$element] = $format;
    }

    protected function shouldOutput(int $verbosity): bool {
        if ($verbosity <= $this->systemVerbosity) {
            return true;
        }

        return false;
    }
}