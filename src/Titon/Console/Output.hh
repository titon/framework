<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Kernel\Output as KernelOutput;
use Titon\Console\System\SystemFactory;
use Exception;

/**
 * The `Output` class handles formatting and sending data to the ouput stream.
 *
 * @package Titon\Console
 */
class Output implements KernelOutput {

    const int VERBOSITY_QUIET = 0;
    const int VERBOSITY_NORMAL = 1;
    const int VERBOSITY_VERBOSE = 3;

    const string STREAM_STDOUT = 'php://stdout';
    const string STREAM_STDERR = 'php://stderr';

    const string ANSI_CLEAR_LINE = "\033[K";
    const string LF = PHP_EOL;
    const string CR = "\r";

    /**
     * Flag to determine if we should always output ANSI.
     *
     * @var bool
     */
    protected bool $forceAnsi = false;

    /**
     * The singleton instance.
     *
     * @var \Titon\Console\Output|null
     */
    protected static ?Output $instance;

    /**
     * The resource handle for standard error.
     *
     * @var resource
     */
    protected resource $stderr;

    /**
     * The resource handle for standard output.
     *
     * @var resource
     */
    protected resource $stdout;

    /**
     * Data structure containing available styles to apply to output.
     *
     * @var \Titon\Console\StyleMap
     */
    protected StyleMap $styles = Map {};

    /**
     * Flag to determine if we should never output ANSI.
     *
     * @var bool
     */
    protected bool $suppressAnsi = false;

    /**
     * The global verbosity level for the `Output`.
     *
     * @var int
     */
    protected int $verbosity;

    /**
     * Construct a new `Output` object.
     *
     * @param int $verbosity    The verbosity level for the `Output`
     */
    public function __construct(int $verbosity = self::VERBOSITY_NORMAL) {
        $this->stderr = fopen(Output::STREAM_STDERR, 'w');
        $this->stdout = fopen(Output::STREAM_STDOUT, 'w');
        $this->verbosity = $verbosity;

        if (is_null(self::$instance)) {
            self::$instance = $this;
        }
    }

    /**
     * Remove a specific element's style.
     *
     * @param string $element   The element to remove
     *
     * @return $this
     */
    public function clearStyle(string $element): this {
        $this->styles->remove($element);

        return $this;
    }

    /**
     * Send output to the error stream.
     *
     * @param string $output        The contents to output
     * @param int    $newLines      The number of new lines to append to the output
     * @param int    $verbosity     The verbosity level of the output
     * @param string $newlineChar   The new line character to use
     * @return $this
     */
    public function error(string $output = '', int $newLines = 1, int $verbosity = Output::VERBOSITY_NORMAL, string $newlineChar = Output::LF): this {
        if (!$this->shouldOutput($verbosity)) {
            return $this;
        }

        $output = $this->format($output);
        $output .= str_repeat("\n", $newLines);

        fwrite($this->stderr, $output);

        return $this;
    }

    /**
     * Set flag if ANSI output should be forced.
     *
     * @param bool $suppress    If ANSI should be forced
     *
     * @return $this
     */
    public function setForceAnsi(bool $force): this {
        $this->forceAnsi = $force;

        return $this;
    }

    /**
     * Format contents by parsing the style tags and applying necessary formatting.
     *
     * @param string $message   The message to format
     *
     * @string
     */
    public function format(string $message): string {
        $parsedTags = $this->parseTags($message);
        $output = $message;

        foreach ($parsedTags as $xmlTag) {
            if (!is_null($this->styles->get($xmlTag))) {
                $outputAnsi = $this->isAnsiAllowed();
                $style = $this->styles[$xmlTag];
                $output = $style->format($xmlTag, $output, $outputAnsi);

                $matches = [];
                $output = preg_replace_callback('#<[\w-]+?>.*<\/[\w-]+?>#', $matches ==> {
                    if ($outputAnsi) {
                        return sprintf("%s%s%s", $style->getEndCode(), $this->format($matches[0]), $style->getStartCode());
                    }

                    return sprintf("%s", $this->format($matches[0]));
                }, $output);
            }
        }

        return $output;
    }

    /**
     * Detect the current state of ANSI.
     *
     * @return bool
     */
    public function isAnsiAllowed(): bool {
        $allowed = false;

        if ($this->forceAnsi) {
            $allowed = true;
        } else if ($this->suppressAnsi) {
            $allowed = false;
        } else if (SystemFactory::factory()->supportsAnsi()) {
            $allowed = true;
        }

        return $allowed;
    }

    /**
     * Create and return the singleton instance.
     *
     * @return \Titon\Console\Output
     */
    public static function getInstance(): Output {
        if (is_null(self::$instance)) {
            self::$instance = new Output();
        }

        return self::$instance;
    }

    /**
     * Send output to the standard output stream.
     *
     * @param string $output        The contents to output
     * @param int    $newLines      The number of new lines to append to the output
     * @param int    $verbosity     The verbosity level of the output
     * @param string $newlineChar   The new line character to use
     * @return $this
     */
    public function out(string $output = '', int $newLines = 1, int $verbosity = Output::VERBOSITY_NORMAL, string $newlineChar = Output::LF): this {
        if (!$this->shouldOutput($verbosity)) {
            return $this;
        }

        $output = $this->format($output);
        $output .= str_repeat($newlineChar, $newLines);

        fwrite($this->stdout, $output);

        return $this;
    }

    /**
     * Parse all available style tags from the given contents.
     *
     * @param string $stringToParse The contents to parse
     *
     * @return Set<string>
     */
    protected function parseTags(string $stringToParse): Set<string> {
        $tagsMatched = [];
        preg_match_all('#<([\w-]*?)>#', $stringToParse, $tagsMatched);

        return new Set($tagsMatched[1]);
    }

    /**
     * Basic `Exception` renderer to handle outputting of uncaught exceptions
     * thrown in `Command` objects.
     *
     * @param Exception $exception  The `Exception` thrown
     */
    public function renderException(Exception $exception): void {
        $class = explode("\\", get_class($exception));
        $class = $class[count($class) - 1];

        $message = explode('{{BREAK}}', wordwrap($exception->getMessage(), 40, "{{BREAK}}"));
        array_unshift($message, "[$class]");

        $length = max(array_map($key ==> strlen($key), $message));

        $this->error(Output::LF);
        $this->error("<exception>  " . str_pad("", $length) . "  </exception>");

        foreach ($message as $line) {
            $line = str_pad($line, $length);
            $this->error("<exception>  $line  </exception>");
        }

        $this->error("<exception>  " . str_pad("", $length) . "  </exception>");
        $this->error(Output::LF);
    }

    /**
     * Currently unused: Send the response through the given output stream.
     */
    public function send(): void {
    }

    /**
     * Assign a `StyleDefinition` to an XML tag.
     *
     * @param string          $element  The element to assign
     * @param StyleDefinition $format   The style to apply to the given tag
     */
    public function setStyle(string $element, StyleDefinition $format): this {
        $this->styles[$element] = $format;

        return $this;
    }

    /**
     * Set flag if ANSI output should be suppressed.
     *
     * @param bool $suppress    If ANSI should be suppressed
     *
     * @return $this
     */
    public function setSuppressAnsi(bool $suppress): this {
        $this->suppressAnsi = $suppress;

        return $this;
    }

    /**
     * Set the global verbosity of the `Output`.
     *
     * @param int $verbosity    The verbosity to set
     *
     * @return $this
     */
    public function setVerbosity(int $verbosity): this {
        $this->verbosity = $verbosity;

        return $this;
    }

    /**
     * Determine how the given verbosity compares to the class's verbosity level.
     *
     * @param int $verbosity    The verbosity to check
     *
     * @return bool
     */
    protected function shouldOutput(int $verbosity): bool {
        return ($verbosity <= $this->verbosity);
    }

}
