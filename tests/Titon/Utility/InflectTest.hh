<?hh
namespace Titon\Utility;

use Titon\Test\TestCase;

class InflectTest extends TestCase {

    public function testCamelCase(): void {
        $camelCase = [
            'foo Bar', 'fOo Bar', 'foo_Bar', ' foo-_--_BAR',
            'foo-BAR', 'FOO-BAR', 'foo     bar   '
        ];

        foreach ($camelCase as $value) {
            $this->assertEquals('FooBar', Inflect::camelCase($value));
        }
    }

    public function testFileName(): void {
        $this->assertEquals('camel-Case.php', Inflect::fileName('camel Case'));
        $this->assertEquals('StuDly-CaSe.php', Inflect::fileName('StuDly CaSe'));
        $this->assertEquals('Title-Case.php', Inflect::fileName('Title Case'));
        $this->assertEquals('Normal-case.php', Inflect::fileName('Normal case'));
        $this->assertEquals('lowercase.php', Inflect::fileName('lowercase'));
        $this->assertEquals('UPPERCASE.php', Inflect::fileName('UPPERCASE'));
        $this->assertEquals('under_score.php', Inflect::fileName('under_score'));
        $this->assertEquals('dash-es.php', Inflect::fileName('dash-es'));
        $this->assertEquals('123-numbers.php', Inflect::fileName('123 numbers'));
        $this->assertEquals('with-EXT.php', Inflect::fileName('with EXT.xml'));
        $this->assertEquals('lots-of-white-space.php', Inflect::fileName('lots  of     white space'));
    }

    public function testFileNameReplaceExt(): void {
        $this->assertEquals('foo.xml', Inflect::fileName('foo.php', 'xml'));
        $this->assertEquals('foo.bar.xml', Inflect::fileName('foo.bar.php', 'xml'));
        $this->assertEquals('foo.bar.xml', Inflect::fileName('foo.bar.xml', 'xml'));
    }

    public function testClassName(): void {
        $this->assertEquals('CamelCase', Inflect::className('camel Case'));
        $this->assertEquals('StudlyCase', Inflect::className('StuDly CaSe'));
        $this->assertEquals('TitleCase', Inflect::className('Title Case'));
        $this->assertEquals('NormalCase', Inflect::className('Normal case'));
        $this->assertEquals('Lowercase', Inflect::className('lowercase'));
        $this->assertEquals('Uppercase', Inflect::className('UPPERCASE'));
        $this->assertEquals('UnderScore', Inflect::className('under_score'));
        $this->assertEquals('DashEs', Inflect::className('dash-es'));
        $this->assertEquals('123Numbers', Inflect::className('123 numbers'));
        $this->assertEquals('WithExtxml', Inflect::className('with EXT.xml'));
        $this->assertEquals('LotsOfWhiteSpace', Inflect::className('lots  of     white space'));
    }

    public function testHyphenate(): void {
        $this->assertEquals('camel-Case', Inflect::hyphenate('camel Case'));
        $this->assertEquals('StuDly-CaSe', Inflect::hyphenate('StuDly CaSe'));
        $this->assertEquals('Title-Case', Inflect::hyphenate('Title Case'));
        $this->assertEquals('Normal-case', Inflect::hyphenate('Normal case'));
        $this->assertEquals('lowercase', Inflect::hyphenate('lowercase'));
        $this->assertEquals('UPPERCASE', Inflect::hyphenate('UPPERCASE'));
        $this->assertEquals('under_score', Inflect::hyphenate('under_score'));
        $this->assertEquals('dash-es', Inflect::hyphenate('dash-es'));
        $this->assertEquals('123-numbers', Inflect::hyphenate('123 numbers'));
        $this->assertEquals('with-EXT.xml', Inflect::hyphenate('with EXT.xml'));
        $this->assertEquals('lots-of-white-space', Inflect::hyphenate('lots  of     white space'));
    }

    public function testNormalCase(): void {
        $this->assertEquals('This is a string with studly case', Inflect::normalCase('This is A sTring wIth sTudly cAse'));
        $this->assertEquals('And this one has underscores', Inflect::normalCase('and_this_ONE_has_underscores'));
        $this->assertEquals('While this one contains -- dashes', Inflect::normalCase('WHILE this one contains -- DASHES'));
        $this->assertEquals('This is a mix of underscores -- and dashes', Inflect::normalCase('This_is A_MIX oF undeRscores -- aNd_dashes'));
        $this->assertEquals('Lastly, this string contains "punctuation"!', Inflect::normalCase('LaStlY, this STRING contains "punctuation"!'));
    }

    public function testRoute(): void {
        $this->assertEquals('camel-case', Inflect::route('camel Case'));
        $this->assertEquals('studly-case', Inflect::route('StuDly CaSe'));
        $this->assertEquals('title-case', Inflect::route('Title Case'));
        $this->assertEquals('normal-case', Inflect::route('Normal case'));
        $this->assertEquals('lowercase', Inflect::route('lowercase'));
        $this->assertEquals('uppercase', Inflect::route('UPPERCASE'));
        $this->assertEquals('under-score', Inflect::route('under_score'));
        $this->assertEquals('dash-es', Inflect::route('dash-es'));
        $this->assertEquals('123-numbers', Inflect::route('123 numbers'));
        $this->assertEquals('with-ext.xml', Inflect::route('with EXT.xml'));
        $this->assertEquals('lots-of-white-space', Inflect::route('lots  of     white space'));
    }

    public function testSlug(): void {
        $this->assertEquals('this-is-a-string-with-studly-case', Inflect::slug('This is A sTring wIth sTudly cAse'));
        $this->assertEquals('andthisonehasunderscores', Inflect::slug('and_this_ONE_has_underscores'));
        $this->assertEquals('while-this-one-contains-__-dashes', Inflect::slug('WHILE this one contains -- DASHES'));
        $this->assertEquals('thisis-amix-of-underscores-__-anddashes', Inflect::slug('This_is A_MIX oF undeRscores -- aNd_dashes'));
        $this->assertEquals('lastly-this-string-contains-punctuation', Inflect::slug('LaStlY, this STRING contains "punctuation"!'));
    }

    public function testSnakeCase(): void {
        $this->assertEquals('camel_case', Inflect::snakeCase('camel Case'));
        $this->assertEquals('stu_dly_ca_se', Inflect::snakeCase('StuDly CaSe'));
        $this->assertEquals('title_case', Inflect::snakeCase('Title Case'));
        $this->assertEquals('normal_case', Inflect::snakeCase('Normal case'));
        $this->assertEquals('lowercase', Inflect::snakeCase('lowercase'));
        $this->assertEquals('u_p_p_e_r_c_a_s_e', Inflect::snakeCase('UPPERCASE'));
        $this->assertEquals('under_score', Inflect::snakeCase('under_score'));
        $this->assertEquals('dash_es', Inflect::snakeCase('dash-es'));
        $this->assertEquals('123_numbers', Inflect::snakeCase('123 numbers'));
        $this->assertEquals('with_e_x_txml', Inflect::snakeCase('with EXT.xml'));
        $this->assertEquals('lots_of_white_space', Inflect::snakeCase('lots  of     white space'));
    }

    public function testTitleCase(): void {
        $this->assertEquals('This Is A String With Studly Case', Inflect::titleCase('This is A sTring wIth sTudly cAse'));
        $this->assertEquals('And This One Has Underscores', Inflect::titleCase('and_this_ONE_has_underscores'));
        $this->assertEquals('While This One Contains -- Dashes', Inflect::titleCase('WHILE this one contains -- DASHES'));
        $this->assertEquals('This Is A Mix Of Underscores -- And Dashes', Inflect::titleCase('This_is A_MIX oF undeRscores -- aNd_dashes'));
        $this->assertEquals('Lastly, This String Contains "punctuation"!', Inflect::titleCase('LaStlY, this STRING contains "punctuation"!'));
    }

    public function testUnderscore(): void {
        $this->assertEquals('camel_case', Inflect::underscore('camel Case'));
        $this->assertEquals('stu_dly_ca_se', Inflect::underscore('StuDly CaSe'));
        $this->assertEquals('title_case', Inflect::underscore('Title Case'));
        $this->assertEquals('normal_case', Inflect::underscore('Normal case'));
        $this->assertEquals('lowercase', Inflect::underscore('lowercase'));
        $this->assertEquals('u_p_p_e_r_c_a_s_e', Inflect::underscore('UPPERCASE'));
        $this->assertEquals('under_score', Inflect::underscore('under_score'));
        $this->assertEquals('dash_es', Inflect::underscore('dash-es'));
        $this->assertEquals('123_numbers', Inflect::underscore('123 numbers'));
        $this->assertEquals('with_e_x_txml', Inflect::underscore('with EXT.xml'));
        $this->assertEquals('lots_of_white_space', Inflect::underscore('lots  of     white space'));
    }

    public function testVariable(): void {
        $this->assertEquals('camelCase', Inflect::variable('camel Case'));
        $this->assertEquals('StuDlyCaSe', Inflect::variable('StuDly CaSe'));
        $this->assertEquals('TitleCase', Inflect::variable('Title Case'));
        $this->assertEquals('Normalcase', Inflect::variable('Normal case'));
        $this->assertEquals('lowercase', Inflect::variable('lowercase'));
        $this->assertEquals('UPPERCASE', Inflect::variable('UPPERCASE'));
        $this->assertEquals('under_score', Inflect::variable('under_score'));
        $this->assertEquals('dashes', Inflect::variable('dash-es'));
        $this->assertEquals('_123numbers', Inflect::variable('123 numbers'));
        $this->assertEquals('withEXTxml', Inflect::variable('with EXT.xml'));
        $this->assertEquals('lotsofwhitespace', Inflect::variable('lots  of     white space'));
    }

}
