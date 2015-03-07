<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

class Output {

    const int VERBOSITY_QUIET = 0;
    const int VERBOSITY_NORMAL = 1;
    const int VERBOSITY_VERBOSE = 3;
    const string STREAM_STDOUT = 'php://stdout';
    const string STREAM_STDERR = 'php://stderr';
    const string ANSI_CLEAR_LINE = "\033[K";
    const string LF = PHP_EOL;
    const string CR = "\r";

    protected resource $stderr;

    protected resource $stdout;

    protected Map<string, StyleDefinition> $styles = Map {};

    protected int $systemVerbosity;

    public function __construct(int $verbosity = self::VERBOSITY_NORMAL) {
        $this->stderr = fopen(Output::STREAM_STDERR, 'w');
        $this->stdout = fopen(Output::STREAM_STDOUT, 'w');
        $this->systemVerbosity = $verbosity;
    }

    public function error(string $output = '', int $verbosity = Output::VERBOSITY_NORMAL, int $newLines = 1): void {
        if (!$this->shouldOutput($verbosity)) {
            return;
        }

        $output = $this->format($output);
        $output .= str_repeat("\n", $newLines);

        $this->send($output, $this->stderr);
    }

    public function format(string $message): string {
        $parsedTags = array_unique($this->parseTags($message));
        $retval = $message;
        foreach ($parsedTags as $xmlTag) {
            if (!is_null($this->styles[$xmlTag])) {
                $style = $this->styles[$xmlTag];
                $retval = $style->format($xmlTag, $retval);
                $matches = [];
                $retval = preg_replace_callback('#<[\w-]+?>.*<\/[\w-]+?>#', ($matches) ==> {
                    return sprintf("%s%s%s", $style->getEndCode(), $this->format($matches[0]), $style->getStartCode());
                }, $retval);
            }
        }

        return $retval;
    }

    public function out(string $output = '', int $verbosity = Output::VERBOSITY_NORMAL, int $newLines = 1, string $newlineChar = Output::LF): void {
        if (!$this->shouldOutput($verbosity)) {
            return;
        }

        $output = $this->format($output);
        $output .= str_repeat($newlineChar, $newLines);

        $this->send($output, $this->stdout);
    }

    protected function parseTags(string $stringToParse): array<string> {
        $tagsMatched = [];
        preg_match_all('#<([\w-]*?)>#', $stringToParse, $tagsMatched);

        return $tagsMatched[1];
    }

    public function setFormat(string $element, StyleDefinition $format): this {
        $this->styles[$element] = $format;

        return $this;
    }

    protected function send(string $response, resource $stream): void {
        fwrite($stream, $response);
    }

    protected function shouldOutput(int $verbosity): bool {
        if ($verbosity <= $this->systemVerbosity) {
            return true;
        }

        return false;
    }
}