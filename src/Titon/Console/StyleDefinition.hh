<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

class StyleDefinition {

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

    private string $fgColor;

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

    private Vector<string> $effectsList;

    protected Map<string, int> $effectsMap = Map {
        'defaults'  => 0,
        'bold'      => 1,
        'underline' => 4,
        'blink'     => 5,
        'reverse'   => 7,
        'conceal'   => 8,
    };

    public function __construct($fgColor, $bgColor = '', Vector<string> $effects = Vector {}) {
        $this->fgColor = $fgColor;
        $this->bgColor = $bgColor;
        $this->effectsList = $effects;
    }

    public function format(string $xmlTag, string $value): string {
        $values = XmlParser::getValueBetweenTags($xmlTag, $value);
        $retval = $value;
        foreach ($values as $val) {
            $valueReplaced = '<' . $xmlTag . '>' . $val . '</' . $xmlTag . '>';
            $valueResult = $this->replaceTagColors($val);

            $retval = str_replace($valueReplaced, $valueResult, $retval);
        }

        return $retval;
    }

    private function replaceTagColors(string $text): string {
        $colors = $this->getBgColorCode() . ';' . $this->getFgColorCode();
        $effects = $this->getParsedStringEffects();
        $effectsCodeString = $effects ? ';' . $effects : '';

        return sprintf("\033[%sm%s\033[0m", $colors . $effectsCodeString, $text);
    }

    public function getBgColorCode(): string {
        if ($bgColor = $this->bgColorsMap->get($this->bgColor)) {
            return $bgColor;
        }

        return '';
    }

    public function getFgColorCode(): string {
        if ($fgColor = $this->fgColorsMap->get($this->fgColor)) {
            return $fgColor;
        }

        return '';
    }

    public function getParsedStringEffects(): string {
        $effects = Vector {};

        if (!empty($this->effectsList->isEmpty())) {
            foreach ($this->effectsList as $effect) {
                $effects[] = $this->getEffectCode($effect);
            }
        }

        return implode(';', $effects);
    }

    public function getEffectCode(string $effect): int {
        return $this->effectsMap[$effect];
    }
}