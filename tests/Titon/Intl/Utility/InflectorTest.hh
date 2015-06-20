<?hh
namespace Titon\Intl\Utility;

use Titon\Context\Depository;
use Titon\Intl\Locale;
use Titon\Intl\MessageLoader;
use Titon\Intl\Translator;
use Titon\Io\Reader\HackReader;
use Titon\Test\TestCase;

/**
 * @property \Titon\Intl\Translator $object
 */
class InflectorTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Translator(new MessageLoader(Vector {new HackReader()}));
        $this->object->addResourcePaths('common', Set {SRC_DIR . '/Titon/Intl/'});
        $this->object->addLocale(new Locale('en_US'));
        $this->object->localize('en_US');

        Depository::getInstance()->register('Titon\Intl\Translator', $this->object);
    }

    protected function tearDown(): void {
        Depository::getInstance()->clear();
    }

    public function testOrdinal(): void {
        $this->assertEquals('0', Inflect::ordinal('0'));
        $this->assertEquals('1st', Inflect::ordinal('1'));
        $this->assertEquals('2nd', Inflect::ordinal('2'));
        $this->assertEquals('3rd', Inflect::ordinal('3'));
        $this->assertEquals('4th', Inflect::ordinal('4'));
        $this->assertEquals('5th', Inflect::ordinal('5'));
        $this->assertEquals('10th', Inflect::ordinal('10'));

        // teens
        $this->assertEquals('11th', Inflect::ordinal('11'));
        $this->assertEquals('12th', Inflect::ordinal('12'));
        $this->assertEquals('14th', Inflect::ordinal('14'));
        $this->assertEquals('15th', Inflect::ordinal('15'));
        $this->assertEquals('18th', Inflect::ordinal('18'));
        $this->assertEquals('20th', Inflect::ordinal('20'));

        // high numbers
        $this->assertEquals('91st', Inflect::ordinal('91'));
        $this->assertEquals('342nd', Inflect::ordinal('342'));
        $this->assertEquals('8534th', Inflect::ordinal('8534'));
        $this->assertEquals('92343rd', Inflect::ordinal('92343'));
        $this->assertEquals('678420th', Inflect::ordinal('678420'));

        // negative
        $this->assertEquals('-1st', Inflect::ordinal('-1'));
        $this->assertEquals('-22nd', Inflect::ordinal('-22'));
        $this->assertEquals('-333rd', Inflect::ordinal('-333'));
        $this->assertEquals('-16th', Inflect::ordinal('-16'));

        // casting
        $this->assertEquals('11th', Inflect::ordinal('11th'));
        $this->assertEquals('98th', Inflect::ordinal('98s'));
        $this->assertEquals('438th', Inflect::ordinal('438lbs'));
        $this->assertEquals('-12th', Inflect::ordinal('-12$'));
    }

    public function testPluralize(): void {
        // irregular
        $this->assertEquals('opuses', Inflect::pluralize('opus'));
        $this->assertEquals('penises', Inflect::pluralize('penis'));
        $this->assertEquals('loaves', Inflect::pluralize('loaf'));
        $this->assertEquals('mythoi', Inflect::pluralize('mythos'));
        $this->assertEquals('men', Inflect::pluralize('man'));

        // uninflected
        $this->assertEquals('information', Inflect::pluralize('information'));
        $this->assertEquals('corps', Inflect::pluralize('corps'));
        $this->assertEquals('gallows', Inflect::pluralize('gallows'));
        $this->assertEquals('maltese', Inflect::pluralize('maltese'));
        $this->assertEquals('rice', Inflect::pluralize('rice'));

        // plural
        $this->assertEquals('matrices', Inflect::pluralize('matrix'));
        $this->assertEquals('buses', Inflect::pluralize('bus'));
        $this->assertEquals('perches', Inflect::pluralize('perch'));
        $this->assertEquals('people', Inflect::pluralize('person'));
        $this->assertEquals('bananas', Inflect::pluralize('banana'));

        // already plural
        $this->assertEquals('opuses', Inflect::pluralize('opuses'));
        $this->assertEquals('penises', Inflect::pluralize('penises'));
        $this->assertEquals('loaves', Inflect::pluralize('loaves'));
        $this->assertEquals('mythoi', Inflect::pluralize('mythoi'));
        $this->assertEquals('men', Inflect::pluralize('men'));
    }

    public function testSingularize(): void {
        // irregular
        $this->assertEquals('atlas', Inflect::singularize('atlases'));
        $this->assertEquals('corpus', Inflect::singularize('corpuses'));
        $this->assertEquals('octopus', Inflect::singularize('octopuses'));
        $this->assertEquals('ox', Inflect::singularize('oxen'));
        $this->assertEquals('goose', Inflect::singularize('geese'));

        // uninflected
        $this->assertEquals('money', Inflect::singularize('money'));
        $this->assertEquals('flounder', Inflect::singularize('flounder'));
        $this->assertEquals('moose', Inflect::singularize('moose'));
        $this->assertEquals('species', Inflect::singularize('species'));
        $this->assertEquals('wildebeest', Inflect::singularize('wildebeest'));

        // singular
        $this->assertEquals('quiz', Inflect::singularize('quizzes'));
        $this->assertEquals('alias', Inflect::singularize('aliases'));
        $this->assertEquals('shoe', Inflect::singularize('shoes'));
        $this->assertEquals('person', Inflect::singularize('people'));
        $this->assertEquals('apple', Inflect::singularize('apples'));

        // already singular
        $this->assertEquals('atlas', Inflect::singularize('atlas'));
        $this->assertEquals('corpus', Inflect::singularize('corpus'));
        $this->assertEquals('octopus', Inflect::singularize('octopus'));
        $this->assertEquals('ox', Inflect::singularize('ox'));
        $this->assertEquals('goose', Inflect::singularize('goose'));
    }

    public function testTransliterate(): void {
        $this->markTestSkipped('Hack currently has issues with accented characters.');

        $this->assertEquals('Ingles', Inflect::transliterate('Inglés'));
        $this->assertEquals('Uber', Inflect::transliterate('Über'));
    }

}
