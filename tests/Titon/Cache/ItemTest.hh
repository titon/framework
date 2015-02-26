<?hh
namespace Titon\Cache;

use Titon\Test\TestCase;
use \DateTime;

/**
 * @property \Titon\Cache\Item $object
 */
class ItemTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Item('foo', 'bar');
    }

    public function testGetSetValue(): void {
        $this->assertEquals('bar', $this->object->get());

        $this->object->set(123);
        $this->assertEquals(123, $this->object->get());
    }

    public function testGetSetExpiration(): void {
        $this->object->setExpiration();
        $this->assertEquals(new DateTime('@' . strtotime('+1 hour')), $this->object->getExpiration());

        $this->object->setExpiration(300);
        $this->assertEquals(new DateTime('@' . (time() + 300)), $this->object->getExpiration());

        $this->object->setExpiration('+5 days');
        $this->assertEquals(new DateTime('@' . strtotime('+5 days')), $this->object->getExpiration());
    }

    public function testGetSetKey(): void {
        $this->assertEquals('foo', $this->object->getKey());

        $this->object->setKey('baz');
        $this->assertEquals('baz', $this->object->getKey());
    }

    public function testIsHit(): void {
        $this->assertFalse($this->object->isHit());

        $hit = new HitItem('foo', 'bar');
        $this->assertTrue($hit->isHit());

        $miss = new MissItem('foo', 'bar');
        $this->assertFalse($miss->isHit());
    }

}
