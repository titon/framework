<?hh
namespace Titon\Event;

use Titon\Test\TestCase;

/**
 * @property \Titon\Event\EmittableStub $object
 */
class EmittableTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new EmittableStub();
    }

    public function testEmit() {
        $this->assertInstanceOf('Titon\Event\Event', $this->object->emit('event.test'));
    }

    public function testOnOff() {
        $obj = function(Event $event) { };

        $this->object->on('event.test', $obj);

        $this->assertEquals(Vector {
            shape('callback' => $obj, 'priority' => 100, 'once' => false),
        }, $this->object->getEmitter()->getObservers('event.test'));

        $this->object->off('event.test', $obj);

        $this->assertEquals(Vector {}, $this->object->getEmitter()->getObservers('event.test'));
    }

    public function testOnce() {
        $count = 0;
        $obj = function(Event $event) use (&$count) { $count++; };

        $this->object->once('event.test', $obj);

        $this->object->emit('event.test');
        $this->object->emit('event.test');

        $this->assertEquals(1, $count);
    }

}

class EmittableStub {
    use Emittable;
}