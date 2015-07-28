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

    public function testPlural(): void {
        // irregular
        $this->assertEquals('opuses', Inflect::plural('opus'));
        $this->assertEquals('penises', Inflect::plural('penis'));
        $this->assertEquals('loaves', Inflect::plural('loaf'));
        $this->assertEquals('mythoi', Inflect::plural('mythos'));
        $this->assertEquals('men', Inflect::plural('man'));

        // uninflected
        $this->assertEquals('information', Inflect::plural('information'));
        $this->assertEquals('corps', Inflect::plural('corps'));
        $this->assertEquals('gallows', Inflect::plural('gallows'));
        $this->assertEquals('maltese', Inflect::plural('maltese'));
        $this->assertEquals('rice', Inflect::plural('rice'));

        // plural
        $this->assertEquals('matrices', Inflect::plural('matrix'));
        $this->assertEquals('buses', Inflect::plural('bus'));
        $this->assertEquals('perches', Inflect::plural('perch'));
        $this->assertEquals('people', Inflect::plural('person'));
        $this->assertEquals('bananas', Inflect::plural('banana'));

        // already plural
        $this->assertEquals('opuses', Inflect::plural('opuses'));
        $this->assertEquals('penises', Inflect::plural('penises'));
        $this->assertEquals('loaves', Inflect::plural('loaves'));
        $this->assertEquals('mythoi', Inflect::plural('mythoi'));
        $this->assertEquals('men', Inflect::plural('men'));
    }

    public function testSingular(): void {
        // irregular
        $this->assertEquals('atlas', Inflect::singular('atlases'));
        $this->assertEquals('corpus', Inflect::singular('corpuses'));
        $this->assertEquals('octopus', Inflect::singular('octopuses'));
        $this->assertEquals('ox', Inflect::singular('oxen'));
        $this->assertEquals('goose', Inflect::singular('geese'));

        // uninflected
        $this->assertEquals('money', Inflect::singular('money'));
        $this->assertEquals('flounder', Inflect::singular('flounder'));
        $this->assertEquals('moose', Inflect::singular('moose'));
        $this->assertEquals('species', Inflect::singular('species'));
        $this->assertEquals('wildebeest', Inflect::singular('wildebeest'));

        // singular
        $this->assertEquals('quiz', Inflect::singular('quizzes'));
        $this->assertEquals('alias', Inflect::singular('aliases'));
        $this->assertEquals('shoe', Inflect::singular('shoes'));
        $this->assertEquals('person', Inflect::singular('people'));
        $this->assertEquals('apple', Inflect::singular('apples'));

        // already singular
        $this->assertEquals('atlas', Inflect::singular('atlas'));
        $this->assertEquals('corpus', Inflect::singular('corpus'));
        $this->assertEquals('octopus', Inflect::singular('octopus'));
        $this->assertEquals('ox', Inflect::singular('ox'));
        $this->assertEquals('goose', Inflect::singular('goose'));
    }

    public function testTransliterate(): void {
        $this->markTestSkipped('Hack currently has issues with accented characters.');

        $this->assertEquals('Ingles', Inflect::transliterate('Inglés'));
        $this->assertEquals('Uber', Inflect::transliterate('Über'));
    }

}
