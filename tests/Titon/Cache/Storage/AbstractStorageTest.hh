<?hh
namespace Titon\Cache\Storage;

use Titon\Cache\HitItem;
use Titon\Cache\Item;
use Titon\Test\TestCase;
use Titon\Utility\Config;

/**
 * @property \Titon\Cache\Storage\AbstractStorage $object
 */
abstract class AbstractStorageTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        // Assert true so we know cache is being written
        $this->object->save(new Item('foo', ['username' => 'Titon'], '+5 minutes'));
        $this->object->save(new Item('bar', [['id' => 1], ['id' => 2]], '-1 day')); // Expired, so doesn't save
        $this->object->save(new Item('count', 1, '+5 minutes'));
    }

    protected function tearDown() {
        if ($this->object) {
            $this->object->flush();
            unset($this->object);
        }
    }

    public function testClear() {
        $this->assertTrue($this->object->has('foo'));

        $this->object->clear();

        $this->assertFalse($this->object->has('foo'));
    }

    public function testCommitDeferred() {
        $this->assertEquals(Vector {}, $this->object->getDeferred());

        $item1 = new Item('baz', 123);
        $item2 = new Item('qux', true);

        $this->object->saveDeferred($item1);
        $this->object->saveDeferred($item2);

        $this->assertEquals(Vector {$item1, $item2}, $this->object->getDeferred());

        $this->assertFalse($this->object->has('baz'));
        $this->assertFalse($this->object->has('qux'));

        $this->object->commit();

        $this->assertEquals(Vector {}, $this->object->getDeferred());

        $this->assertTrue($this->object->has('baz'));
        $this->assertTrue($this->object->has('qux'));
    }

    public function testDecrement() {
        $this->assertEquals(1, $this->object->get('count'));
        $this->assertEquals(0, $this->object->decrement('count', 1));
        $this->assertEquals(-5, $this->object->decrement('count', 5));
    }

    public function testDecrementInitialSet() {
        $this->assertSame(-1, $this->object->decrement('missing'));
        $this->assertSame(-6, $this->object->decrement('missing', 5));
    }

    public function testDeleteItem() {
        $this->assertTrue($this->object->has('foo'));

        $this->object->deleteItem('foo');

        $this->assertFalse($this->object->has('foo'));
    }

    public function testDeleteItems() {
        $this->assertTrue($this->object->has('foo'));
        $this->assertTrue($this->object->has('count'));

        $this->object->deleteItems(['foo', 'count']);

        $this->assertFalse($this->object->has('foo'));
        $this->assertFalse($this->object->has('count'));
    }

    public function testFlush() {
        $this->assertTrue($this->object->has('foo'));

        $this->object->flush();

        $this->assertFalse($this->object->has('foo'));
    }

    public function testGet() {
        $this->assertEquals(['username' => 'Titon'], $this->object->get('foo'));
        $this->assertEquals(1, $this->object->get('count'));
    }

    /**
     * @expectedException \Titon\Cache\Exception\MissingItemException
     */
    public function testGetMissingKey() {
        $this->assertEquals(null, $this->object->get('bar'));
    }

    public function testGetItem() {
        $this->assertEquals(new HitItem('foo', ['username' => 'Titon']), $this->object->getItem('foo'));
        $this->assertEquals(new HitItem('count', 1), $this->object->getItem('count'));
    }

    public function testGetItemMissingKey() {
        $item = $this->object->getItem('bar');

        $this->assertInstanceOf('Titon\Cache\MissItem', $item); // Expired
        $this->assertFalse($item->isHit());
    }

    public function testGetSetPrefix() {
        $this->assertNotEquals('', $this->object->getPrefix()); // Set in constructor

        $this->object->setPrefix('prefix-');

        $this->assertEquals('prefix-', $this->object->getPrefix());

        Config::set('cache.prefix', 'global-');

        $this->assertEquals('global-prefix-', $this->object->getPrefix());

        Config::set('cache.prefix', '');
    }

    public function testHas() {
        $this->assertTrue($this->object->has('foo'));
        $this->assertFalse($this->object->has('foobar'));
    }

    public function testIncrement() {
        $this->assertEquals(1, $this->object->get('count'));
        $this->assertEquals(2, $this->object->increment('count', 1));
        $this->assertEquals(7, $this->object->increment('count', 5));
    }

    public function testIncrementInitialSet() {
        $this->assertSame(1, $this->object->increment('missing'));
        $this->assertSame(6, $this->object->increment('missing', 5));
    }

    public function testRemove() {
        $this->assertTrue($this->object->has('foo'));

        $this->object->remove('foo');

        $this->assertFalse($this->object->has('foo'));
    }

    public function testSave() {
        $this->assertFalse($this->object->has('bar'));

        $this->object->save(new Item('bar', 123));

        $this->assertTrue($this->object->has('bar'));
    }

    public function testSaveInvalidExpiration() {
        $this->assertFalse($this->object->has('bar'));

        $this->object->save(new Item('bar', 123, new \DateTime()));

        $this->assertFalse($this->object->has('bar'));
    }

    public function testSet() {
        $this->assertEquals(['username' => 'Titon'], $this->object->get('foo'));

        $this->object->set('foo', ['username' => 'Titon Framework'], strtotime('+10 minutes'));

        $this->assertEquals(['username' => 'Titon Framework'], $this->object->get('foo'));
    }

    public function testStats() {
        $this->assertInstanceOf('HH\Map', $this->object->stats());
    }

    public function testStore() {
        $this->assertEquals('foo', $this->object->store('storeTest', () ==> 'foo'));

        $this->assertEquals('foo', $this->object->store('storeTest', () ==> 'bar'));

        $this->assertEquals('foo', $this->object->store('storeTest', () ==> 'baz'));
    }

}
