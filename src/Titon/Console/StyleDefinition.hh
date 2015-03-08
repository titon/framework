<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

/**
 * A `StyleDefinition` defines how to style output it applies to.
 *
 * @package Titon\Console
 */
class StyleDefinition {

    /**
     * The background color to apply.
     *
     * @var string
     */
    private string $bgColor;

    protected Map<string, int> $bgColorsMap = Map {
        'black'   => 40,
        'red'     => 41,
        'green'   => 42,
        'brown'   => 43,
        'blue'    => 44,
        'magenta' => 45,
        'cyan'    => 46,
        'white'   => 47,
    };

    /**
     * The foreground color to apply.
     *
     * @var string
     */
    private string $fgColor;

    /**
     * Data structure containing available foreground colors.
     *
     * @var Map<string, string>
     */
    protected Map<string, string> $fgColorsMap = Map {
        'black'        => '0;30',
        'dark_gray'    => '1;30',
        'blue'         => '0;34',
        'light_blue'   => '1;34',
        'green'        => '0;32',
        'light_green'  => '1;32',
        'cyan'         => '0;36',
        'light_cyan'   => '1;36',
        'red'          => '0;31',
        'light_red'    => '1;31',
        'purple'       => '0;35',
        'light_purple' => '1;35',
        'brown'        => '0;33',
        'yellow'       => '1;33',
        'light_gray'   => '0;37',
        'white'        => '1;37',
    };

    /**
     * The various effects to apply.
     *
     * @var Vector<string>
     */
    private Vector<string> $effectsList;

    /**
     * Data structure containing available effects.
     *
     * @var Map<string, int>
     */
    protected Map<string, int> $effectsMap = Map {
        'defaults'  => 0,
        'bold'      => 1,
        'underline' => 4,
        'blink'     => 5,
        'reverse'   => 7,
        'conceal'   => 8,
    };

    /**
     * Create a new `StyleDefinition`.
     *
     * @param string $fgColor           The foreground color of the style
     * @param string $bgColor           The background color of the style
     * @param Vector<string> $effects   The effects of the style
     */
    public function __construct(string $fgColor, string $bgColor = '', Vector<string> $effects = Vector {}) {
        $this->fgColor = $fgColor;
        $this->bgColor = $bgColor;
        $this->effectsList = $effects;
    }

    /**
     * Format the contents between the given XML tag with the style definition.
     *
     * @param string $xmlTag    The XML tag
     * @param string $value     The contents to format
     *
     * @return string
     */
    public function format(string $xmlTag, string $value): string {
        $values = $this->getValueBetweenTags($xmlTag, $value);
        $retval = $value;
        foreach ($values as $val) {
            $valueReplaced = '<' . $xmlTag . '>' . $val . '</' . $xmlTag . '>';
            $valueResult = $this->replaceTagColors($val);

            $retval = str_replace($valueReplaced, $valueResult, $retval);
        }

        return $retval;
    }

    /**
     * Retrieve the background color code of the `StyleDefinition`.
     *
     * @return string
     */
    public function getBgColorCode(): string {
        if (!is_null($bgColor = $this->bgColorsMap->get($this->bgColor))) {
            invariant(is_string($bgColor), "Must be a string.");

            return $bgColor;
        }

        return '';
    }

    /**
     * Retrieve the effects code of the `StyleDefinition`.
     *
     * @return string
     */
    public function getEffectCode(string $effect): int {
        return $this->effectsMap[$effect];
    }

    /**
     * Retrieve the code to end the `StyleDefinition`.
     *
     * @return string
     */
    public function getEndCode(): string {
        return "\033[0m";
    }

    /**
     * Retrieve the foreground color code of the `StyleDefinition`.
     *
     * @return string
     */
    public function getFgColorCode(): string {
        if (!is_null($fgColor = $this->fgColorsMap->get($this->fgColor))) {
            invariant(is_string($fgColor), "Must be a string.");

            return $fgColor;
        }

        return '';
    }

    /**
     * Retrieve the string of effects to apply for the  `StyleDefinition`.
     *
     * @return string
     */
    public function getParsedStringEffects(): string {
        $effects = Vector {};

        if ($this->effectsList->isEmpty()) {
            foreach ($this->effectsList as $effect) {
                $effects[] = $this->getEffectCode($effect);
            }
        }

        return implode(';', $effects);
    }

    /**
     * Retrieve the start code of the `StyleDefinition`.
     *
     * @return string
     */
    public function getStartCode(): string {
        $colors = $this->getBgColorCode() . ';' . $this->getFgColorCode();
        $effects = $this->getParsedStringEffects();
        $effects = $effects ? ';' . $effects : '';

        return sprintf("\033[%sm", $colors . $effects);
    }

    /**
     * Parse and return contents between the given XML tag.
     *
     * @return string
     */
    public function getValueBetweenTags(string $tag, string $stringToParse): array<string> {
        $regexp = '#<' . $tag . '>(.*?)</' . $tag . '>#s';
        $valuesMatched = [];
        preg_match_all($regexp, $stringToParse, $valuesMatched);

        return $valuesMatched[1];
    }

    /**
     * Return the styled text.
     *
     * @param string $text  The text to style
     *
     * @return string
     */
    protected function replaceTagColors(string $text): string {
        return sprintf("%s%s%s", $this->getStartCode(), $text, $this->getEndCode());
    }
}