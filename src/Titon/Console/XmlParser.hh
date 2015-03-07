<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

class XmlParser {

    public static function parseTags(string $stringToParse): array<string> {
        preg_match_all('#<([\w-]*?)>#', $stringToParse, $tagsMatched);

        return $tagsMatched[1];
    }

    public function getValueBetweenTags(string $tag, string $stringToParse): array<string> {
        $regexp = '#<' . $tag . '>(.*?)</' . $tag . '>#s';
        preg_match_all($regexp, $stringToParse, $valuesMatched);

        return $valuesMatched[1];
    }
}