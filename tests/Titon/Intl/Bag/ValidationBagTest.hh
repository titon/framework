<?hh
namespace Titon\Intl\Bag;

use Titon\Intl\Locale;
use Titon\Test\TestCase;

/**
 * @property \Titon\Intl\Bag\ValidationBag $object
 */
class ValidationBagTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $locale = new Locale('en');
        $locale->addResourcePath('core', SRC_DIR . '/Titon/Intl/');

        $this->object = $locale->getValidationRules();
    }

    public function testGetCurrency(): void {
        $this->assertEquals('/^\$[0-9,]+(?:\.[0-9]{2})?$/', $this->object->getCurrency());
    }

    public function testGetPhone(): void {
        $this->assertEquals('/^(?:\+?1)?\s?(?:\([0-9]{3}\))?\s?[0-9]{3}-[0-9]{4}$/', $this->object->getPhone());
    }

    public function testGetPostalCode(): void {
        $this->assertEquals('/^[0-9]{5}(?:-[0-9]{4})?$/', $this->object->getPostalCode());
    }

    public function testGetSSN(): void {
        $this->assertEquals( '/^[0-9]{3}-[0-9]{2}-[0-9]{4}$/', $this->object->getSSN());
    }

}
