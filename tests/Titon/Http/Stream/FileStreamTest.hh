<?hh
namespace Titon\Http\Stream;

use Titon\Test\TestCase;

/**
 * @property \Titon\Http\Stream\FileStream $object
 */
class FileStreamTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new FileStream(TEMP_DIR . '/utility/barbarian.xml');
    }

    protected function tearDown() {
        parent::tearDown();

        $this->object->close();
    }

    /**
     * @expectedException \Titon\Http\Exception\InvalidFileException
     */
    public function testConstructorErrorsOnInvalidPath() {
        new FileStream('foo');
    }

    public function testGetMode() {
        $this->assertEquals('r+b', $this->object->getMode());
    }

    public function testGetSize() {
        $this->assertEquals(1361, $this->object->getSize());
    }

    public function testIsLocal() {
        $this->assertTrue($this->object->isLocal());
    }

}