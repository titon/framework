<?hh
namespace Titon\Utility;

use Titon\Test\TestCase;

class InflectorTest extends TestCase {

    public function testCamelCase() {
        $camelCase = [
            'foo Bar', 'fOo Bar', 'foo_Bar', ' foo-_--_BAR',
            'foo-BAR', 'FOO-BAR', 'foo     bar   '
        ];

        foreach ($camelCase as $value) {
            $this->assertEquals('FooBar', Inflector::camelCase($value));
        }
    }

    public function testFilename() {
        $this->assertEquals('CamelCase.php', Inflector::fileName('camel Case'));
        $this->assertEquals('StudlyCase.php', Inflector::fileName('StuDly CaSe'));
        $this->assertEquals('TitleCase.php', Inflector::fileName('Title Case'));
        $this->assertEquals('NormalCase.php', Inflector::fileName('Normal case'));
        $this->assertEquals('Lowercase.php', Inflector::fileName('lowercase'));
        $this->assertEquals('Uppercase.php', Inflector::fileName('UPPERCASE'));
        $this->assertEquals('UnderScore.php', Inflector::fileName('under_score'));
        $this->assertEquals('DashEs.php', Inflector::fileName('dash-es'));
        $this->assertEquals('123Numbers.php', Inflector::fileName('123 numbers'));
        $this->assertEquals('WithExt.php', Inflector::fileName('with EXT.xml'));
        $this->assertEquals('LotsOfWhiteSpace.php', Inflector::fileName('lots  of     white space'));

        // Alternate extension and lowercase first character
        $this->assertEquals('camelCase.xml', Inflector::fileName('camel Case', 'xml', false));
        $this->assertEquals('studlyCase.xml', Inflector::fileName('StuDly CaSe', 'xml', false));
        $this->assertEquals('titleCase.xml', Inflector::fileName('Title Case', 'xml', false));
        $this->assertEquals('normalCase.xml', Inflector::fileName('Normal case', 'xml', false));
        $this->assertEquals('lowercase.xml', Inflector::fileName('lowercase', 'xml', false));
        $this->assertEquals('uppercase.xml', Inflector::fileName('UPPERCASE', 'xml', false));
        $this->assertEquals('underScore.xml', Inflector::fileName('under_score', 'xml', false));
        $this->assertEquals('dashEs.xml', Inflector::fileName('dash-es', 'xml', false));
        $this->assertEquals('123Numbers.xml', Inflector::fileName('123 numbers', 'xml', false));
        $this->assertEquals('withExt.xml', Inflector::fileName('with EXT.xml', 'xml', false));
        $this->assertEquals('lotsOfWhiteSpace.xml', Inflector::fileName('lots  of     white space', 'xml', false));
    }

    public function testClassName() {
        $this->assertEquals('CamelCase', Inflector::className('camel Case'));
        $this->assertEquals('StudlyCase', Inflector::className('StuDly CaSe'));
        $this->assertEquals('TitleCase', Inflector::className('Title Case'));
        $this->assertEquals('NormalCase', Inflector::className('Normal case'));
        $this->assertEquals('Lowercase', Inflector::className('lowercase'));
        $this->assertEquals('Uppercase', Inflector::className('UPPERCASE'));
        $this->assertEquals('UnderScore', Inflector::className('under_score'));
        $this->assertEquals('DashEs', Inflector::className('dash-es'));
        $this->assertEquals('123Numbers', Inflector::className('123 numbers'));
        $this->assertEquals('WithExtxml', Inflector::className('with EXT.xml'));
        $this->assertEquals('LotsOfWhiteSpace', Inflector::className('lots  of     white space'));
    }

    public function testHyphenate() {
        $this->assertEquals('camel-Case', Inflector::hyphenate('camel Case'));
        $this->assertEquals('StuDly-CaSe', Inflector::hyphenate('StuDly CaSe'));
        $this->assertEquals('Title-Case', Inflector::hyphenate('Title Case'));
        $this->assertEquals('Normal-case', Inflector::hyphenate('Normal case'));
        $this->assertEquals('lowercase', Inflector::hyphenate('lowercase'));
        $this->assertEquals('UPPERCASE', Inflector::hyphenate('UPPERCASE'));
        $this->assertEquals('under_score', Inflector::hyphenate('under_score'));
        $this->assertEquals('dash-es', Inflector::hyphenate('dash-es'));
        $this->assertEquals('123-numbers', Inflector::hyphenate('123 numbers'));
        $this->assertEquals('with-EXT.xml', Inflector::hyphenate('with EXT.xml'));
        $this->assertEquals('lots-of-white-space', Inflector::hyphenate('lots  of     white space'));
    }

    public function testNormalCase() {
        $this->assertEquals('This is a string with studly case', Inflector::normalCase('This is A sTring wIth sTudly cAse'));
        $this->assertEquals('And this one has underscores', Inflector::normalCase('and_this_ONE_has_underscores'));
        $this->assertEquals('While this one contains -- dashes', Inflector::normalCase('WHILE this one contains -- DASHES'));
        $this->assertEquals('This is a mix of underscores -- and dashes', Inflector::normalCase('This_is A_MIX oF undeRscores -- aNd_dashes'));
        $this->assertEquals('Lastly, this string contains "punctuation"!', Inflector::normalCase('LaStlY, this STRING contains "punctuation"!'));
    }

    public function testRoute() {
        $this->assertEquals('camel-case', Inflector::route('camel Case'));
        $this->assertEquals('studly-case', Inflector::route('StuDly CaSe'));
        $this->assertEquals('title-case', Inflector::route('Title Case'));
        $this->assertEquals('normal-case', Inflector::route('Normal case'));
        $this->assertEquals('lowercase', Inflector::route('lowercase'));
        $this->assertEquals('uppercase', Inflector::route('UPPERCASE'));
        $this->assertEquals('under-score', Inflector::route('under_score'));
        $this->assertEquals('dash-es', Inflector::route('dash-es'));
        $this->assertEquals('123-numbers', Inflector::route('123 numbers'));
        $this->assertEquals('with-ext.xml', Inflector::route('with EXT.xml'));
        $this->assertEquals('lots-of-white-space', Inflector::route('lots  of     white space'));
    }

    public function testSlug() {
        $this->assertEquals('this-is-a-string-with-studly-case', Inflector::slug('This is A sTring wIth sTudly cAse'));
        $this->assertEquals('andthisonehasunderscores', Inflector::slug('and_this_ONE_has_underscores'));
        $this->assertEquals('while-this-one-contains-__-dashes', Inflector::slug('WHILE this one contains -- DASHES'));
        $this->assertEquals('thisis-amix-of-underscores-__-anddashes', Inflector::slug('This_is A_MIX oF undeRscores -- aNd_dashes'));
        $this->assertEquals('lastly-this-string-contains-punctuation', Inflector::slug('LaStlY, this STRING contains "punctuation"!'));
    }

    public function testSnakeCase() {
        $this->assertEquals('camel_case', Inflector::snakeCase('camel Case'));
        $this->assertEquals('stu_dly_ca_se', Inflector::snakeCase('StuDly CaSe'));
        $this->assertEquals('title_case', Inflector::snakeCase('Title Case'));
        $this->assertEquals('normal_case', Inflector::snakeCase('Normal case'));
        $this->assertEquals('lowercase', Inflector::snakeCase('lowercase'));
        $this->assertEquals('u_p_p_e_r_c_a_s_e', Inflector::snakeCase('UPPERCASE'));
        $this->assertEquals('under_score', Inflector::snakeCase('under_score'));
        $this->assertEquals('dashes', Inflector::snakeCase('dash-es'));
        $this->assertEquals('123_numbers', Inflector::snakeCase('123 numbers'));
        $this->assertEquals('with_e_x_txml', Inflector::snakeCase('with EXT.xml'));
        $this->assertEquals('lots_of_white_space', Inflector::snakeCase('lots  of     white space'));
    }

    public function testTableName() {
        $this->assertEquals('camelCase', Inflector::tableName('camel Case'));
        $this->assertEquals('studlyCase', Inflector::tableName('StuDly CaSe'));
        $this->assertEquals('titleCase', Inflector::tableName('Title Case'));
        $this->assertEquals('normalCase', Inflector::tableName('Normal case'));
        $this->assertEquals('lowercase', Inflector::tableName('lowercase'));
        $this->assertEquals('uppercase', Inflector::tableName('UPPERCASE'));
        $this->assertEquals('underScore', Inflector::tableName('under_score'));
        $this->assertEquals('dashEs', Inflector::tableName('dash-es'));
        $this->assertEquals('123Numbers', Inflector::tableName('123 numbers'));
        $this->assertEquals('withExtxml', Inflector::tableName('with EXT.xml'));
        $this->assertEquals('lotsOfWhiteSpace', Inflector::tableName('lots  of     white space'));
    }

    public function testTitleCase() {
        $this->assertEquals('This Is A String With Studly Case', Inflector::titleCase('This is A sTring wIth sTudly cAse'));
        $this->assertEquals('And This One Has Underscores', Inflector::titleCase('and_this_ONE_has_underscores'));
        $this->assertEquals('While This One Contains -- Dashes', Inflector::titleCase('WHILE this one contains -- DASHES'));
        $this->assertEquals('This Is A Mix Of Underscores -- And Dashes', Inflector::titleCase('This_is A_MIX oF undeRscores -- aNd_dashes'));
        $this->assertEquals('Lastly, This String Contains "punctuation"!', Inflector::titleCase('LaStlY, this STRING contains "punctuation"!'));
    }

    public function testUnderscore() {
        $this->assertEquals('camel_case', Inflector::underscore('camel Case'));
        $this->assertEquals('stu_dly_ca_se', Inflector::underscore('StuDly CaSe'));
        $this->assertEquals('title_case', Inflector::underscore('Title Case'));
        $this->assertEquals('normal_case', Inflector::underscore('Normal case'));
        $this->assertEquals('lowercase', Inflector::underscore('lowercase'));
        $this->assertEquals('u_p_p_e_r_c_a_s_e', Inflector::underscore('UPPERCASE'));
        $this->assertEquals('under_score', Inflector::underscore('under_score'));
        $this->assertEquals('dashes', Inflector::underscore('dash-es'));
        $this->assertEquals('123_numbers', Inflector::underscore('123 numbers'));
        $this->assertEquals('with_e_x_txml', Inflector::underscore('with EXT.xml'));
        $this->assertEquals('lots_of_white_space', Inflector::underscore('lots  of     white space'));
    }

    public function testVariable() {
        $this->assertEquals('camelCase', Inflector::variable('camel Case'));
        $this->assertEquals('StuDlyCaSe', Inflector::variable('StuDly CaSe'));
        $this->assertEquals('TitleCase', Inflector::variable('Title Case'));
        $this->assertEquals('Normalcase', Inflector::variable('Normal case'));
        $this->assertEquals('lowercase', Inflector::variable('lowercase'));
        $this->assertEquals('UPPERCASE', Inflector::variable('UPPERCASE'));
        $this->assertEquals('under_score', Inflector::variable('under_score'));
        $this->assertEquals('dashes', Inflector::variable('dash-es'));
        $this->assertEquals('_123numbers', Inflector::variable('123 numbers'));
        $this->assertEquals('withEXTxml', Inflector::variable('with EXT.xml'));
        $this->assertEquals('lotsofwhitespace', Inflector::variable('lots  of     white space'));
    }

}
