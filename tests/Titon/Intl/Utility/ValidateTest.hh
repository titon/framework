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
class ValidateTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Translator(new MessageLoader(new HackReader()));
        $this->object->addResourcePaths('common', Set {SRC_DIR . '/Titon/Intl/'});
        $this->object->addLocale(new Locale('en_US'));
        $this->object->localize('en_US');

        Depository::getInstance()->register('Titon\Intl\Translator', $this->object);
    }

    protected function tearDown(): void {
        Depository::getInstance()->clear();
    }

    public function testCurrency(): void {
        $this->assertTrue(Validate::currency('$1,000.00'));
        $this->assertTrue(Validate::currency('$343'));
        $this->assertTrue(Validate::currency('$193,482.33'));

        $this->assertFalse(Validate::currency('2,392.23'));
        $this->assertFalse(Validate::currency('2325'));
        $this->assertFalse(Validate::currency('$ten'));
        $this->assertFalse(Validate::currency('$1923.2'));
    }

    public function testPhone(): void {
        $this->assertTrue(Validate::phone('666-1337'));
        $this->assertTrue(Validate::phone('(888)666-1337'));
        $this->assertTrue(Validate::phone('(888) 666-1337'));
        $this->assertTrue(Validate::phone('1 (888) 666-1337'));
        $this->assertTrue(Validate::phone('+1 (888) 666-1337'));

        $this->assertFalse(Validate::phone('666.1337'));
        $this->assertFalse(Validate::phone('888-666.1337'));
        $this->assertFalse(Validate::phone('1 888.666.1337'));
        $this->assertFalse(Validate::phone('666-ABMS'));
    }

    public function testPostalCode(): void {
        $this->assertTrue(Validate::postalCode('38842'));
        $this->assertTrue(Validate::postalCode('38842-0384'));

        $this->assertFalse(Validate::postalCode('3842'));
        $this->assertFalse(Validate::postalCode('38842.0384'));
        $this->assertFalse(Validate::postalCode('AksiS-0384'));
    }

    public function testSsn(): void {
        $this->assertTrue(Validate::ssn('666-10-1337'));
        $this->assertTrue(Validate::ssn('384-29-3481'));

        $this->assertFalse(Validate::ssn('66-10-1337'));
        $this->assertFalse(Validate::ssn('384-29-341'));
        $this->assertFalse(Validate::ssn('666.10.1337'));
        $this->assertFalse(Validate::ssn('AHE-29-34P1'));
    }

}
