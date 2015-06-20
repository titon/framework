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
        $this->assertEquals('0', Inflector::ordinal('0'));
        $this->assertEquals('1st', Inflector::ordinal('1'));
        $this->assertEquals('2nd', Inflector::ordinal('2'));
        $this->assertEquals('3rd', Inflector::ordinal('3'));
        $this->assertEquals('4th', Inflector::ordinal('4'));
        $this->assertEquals('5th', Inflector::ordinal('5'));
        $this->assertEquals('10th', Inflector::ordinal('10'));

        // teens
        $this->assertEquals('11th', Inflector::ordinal('11'));
        $this->assertEquals('12th', Inflector::ordinal('12'));
        $this->assertEquals('14th', Inflector::ordinal('14'));
        $this->assertEquals('15th', Inflector::ordinal('15'));
        $this->assertEquals('18th', Inflector::ordinal('18'));
        $this->assertEquals('20th', Inflector::ordinal('20'));

        // high numbers
        $this->assertEquals('91st', Inflector::ordinal('91'));
        $this->assertEquals('342nd', Inflector::ordinal('342'));
        $this->assertEquals('8534th', Inflector::ordinal('8534'));
        $this->assertEquals('92343rd', Inflector::ordinal('92343'));
        $this->assertEquals('678420th', Inflector::ordinal('678420'));

        // negative
        $this->assertEquals('-1st', Inflector::ordinal('-1'));
        $this->assertEquals('-22nd', Inflector::ordinal('-22'));
        $this->assertEquals('-333rd', Inflector::ordinal('-333'));
        $this->assertEquals('-16th', Inflector::ordinal('-16'));

        // casting
        $this->assertEquals('11th', Inflector::ordinal('11th'));
        $this->assertEquals('98th', Inflector::ordinal('98s'));
        $this->assertEquals('438th', Inflector::ordinal('438lbs'));
        $this->assertEquals('-12th', Inflector::ordinal('-12$'));
    }

    public function testPluralize(): void {
        // irregular
        $this->assertEquals('opuses', Inflector::pluralize('opus'));
        $this->assertEquals('penises', Inflector::pluralize('penis'));
        $this->assertEquals('loaves', Inflector::pluralize('loaf'));
        $this->assertEquals('mythoi', Inflector::pluralize('mythos'));
        $this->assertEquals('men', Inflector::pluralize('man'));

        // uninflected
        $this->assertEquals('information', Inflector::pluralize('information'));
        $this->assertEquals('corps', Inflector::pluralize('corps'));
        $this->assertEquals('gallows', Inflector::pluralize('gallows'));
        $this->assertEquals('maltese', Inflector::pluralize('maltese'));
        $this->assertEquals('rice', Inflector::pluralize('rice'));

        // plural
        $this->assertEquals('matrices', Inflector::pluralize('matrix'));
        $this->assertEquals('buses', Inflector::pluralize('bus'));
        $this->assertEquals('perches', Inflector::pluralize('perch'));
        $this->assertEquals('people', Inflector::pluralize('person'));
        $this->assertEquals('bananas', Inflector::pluralize('banana'));

        // already plural
        $this->assertEquals('opuses', Inflector::pluralize('opuses'));
        $this->assertEquals('penises', Inflector::pluralize('penises'));
        $this->assertEquals('loaves', Inflector::pluralize('loaves'));
        $this->assertEquals('mythoi', Inflector::pluralize('mythoi'));
        $this->assertEquals('men', Inflector::pluralize('men'));
    }

    public function testSingularize(): void {
        // irregular
        $this->assertEquals('atlas', Inflector::singularize('atlases'));
        $this->assertEquals('corpus', Inflector::singularize('corpuses'));
        $this->assertEquals('octopus', Inflector::singularize('octopuses'));
        $this->assertEquals('ox', Inflector::singularize('oxen'));
        $this->assertEquals('goose', Inflector::singularize('geese'));

        // uninflected
        $this->assertEquals('money', Inflector::singularize('money'));
        $this->assertEquals('flounder', Inflector::singularize('flounder'));
        $this->assertEquals('moose', Inflector::singularize('moose'));
        $this->assertEquals('species', Inflector::singularize('species'));
        $this->assertEquals('wildebeest', Inflector::singularize('wildebeest'));

        // singular
        $this->assertEquals('quiz', Inflector::singularize('quizzes'));
        $this->assertEquals('alias', Inflector::singularize('aliases'));
        $this->assertEquals('shoe', Inflector::singularize('shoes'));
        $this->assertEquals('person', Inflector::singularize('people'));
        $this->assertEquals('apple', Inflector::singularize('apples'));

        // already singular
        $this->assertEquals('atlas', Inflector::singularize('atlas'));
        $this->assertEquals('corpus', Inflector::singularize('corpus'));
        $this->assertEquals('octopus', Inflector::singularize('octopus'));
        $this->assertEquals('ox', Inflector::singularize('ox'));
        $this->assertEquals('goose', Inflector::singularize('goose'));
    }

    public function testTransliterate(): void {
        $this->markTestSkipped('Hack currently has issues with accented characters.');

        $this->assertEquals('Ingles', Inflector::transliterate('Inglés'));
        $this->assertEquals('Uber', Inflector::transliterate('Über'));
    }

}
