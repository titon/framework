<?hh
namespace Titon\Http\Bag;

use Titon\Test\TestCase;

/**
 * @property \Titon\Http\Bag\HeaderBag $object
 */
class HeaderBagTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new HeaderBag();
    }

    public function testAppend(): void {
        $this->assertEquals([], $this->object->get('foo'));

        $this->object->append('foo', 'bar');

        $this->assertEquals(['bar'], $this->object->get('foo'));

        $this->object->append('foo', 'baz');

        $this->assertEquals(['bar', 'baz'], $this->object->get('foo'));
    }

    public function testKeyFormatsCorrectly(): void {
        $this->assertEquals('Content-Length', $this->object->key('CONTENT_LENGTH'));
        $this->assertEquals('Content-Type', $this->object->key('Content-TYPE'));
        $this->assertEquals('Location', $this->object->key('location'));
        $this->assertEquals('Last-Modified', $this->object->key('LAST ModiFIed'));
        $this->assertEquals('WWW-Authenticate', $this->object->key('WwW_Authenticate'));
    }

}
