<?hh
namespace Titon\Http\Stream;

use Titon\Test\TestCase;

/**
 * @property \Titon\Http\Stream\FileStream $object
 */
class FileStreamTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new FileStream(TEMP_DIR . '/type/barbarian.xml');
    }

    protected function tearDown() {
        parent::tearDown();

        $this->object->close();
    }

    /**
     * @expectedException \Titon\Common\Exception\MissingFileException
     */
    public function testConstructorErrorsOnInvalidPath() {
        new FileStream('foo');
    }

    public function testGetMode() {
        $this->assertEquals('r+b', $this->object->getMode());
    }

    public function testGetSize() {
        $this->assertGreaterThan(1300, $this->object->getSize()); // Depending on the lookup method, this value can change
    }

    public function testIsLocal() {
        $this->assertTrue($this->object->isLocal());
    }

}