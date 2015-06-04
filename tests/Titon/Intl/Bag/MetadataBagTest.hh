<?hh
namespace Titon\Intl\Bag;

use Titon\Intl\Locale;
use Titon\Intl\PluralRule;
use Titon\Test\TestCase;

/**
 * @property \Titon\Intl\Bag\MetadataBag $object
 */
class MetadataBagTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $locale = new Locale('en_US');
        $locale->addResourcePath('core', SRC_DIR . '/Titon/Intl/');

        $this->object = $locale->getMetadata();
    }

    public function testGetCode(): void {
        $this->assertEquals('en_US', $this->object->getCode());
    }

    public function testGtISO2Code(): void {
        $this->assertEquals('en', $this->object->getISO2Code());
    }

    public function testGetISO3Codes(): void {
        $this->assertEquals(Vector {'eng'}, $this->object->getISO3Codes());
    }

    public function testGetTimezone(): void {
        $this->assertEquals('America/New_York', $this->object->getTimezone());
    }

    public function testGetParentCode(): void {
        $this->assertEquals('en', $this->object->getParentCode());
    }

    public function testGetPluralRule(): void {
        $this->assertEquals(PluralRule::RULE_2, $this->object->getPluralRule());
    }

    public function testGetTitle(): void {
        $this->assertEquals('English (United States)', $this->object->getTitle());
    }

}
