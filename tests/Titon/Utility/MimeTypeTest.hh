<?hh
namespace Titon\Utility;

use Titon\Test\TestCase;

class MimeTypeTest extends TestCase {

    public function testGetAll(): void {
        $this->assertEquals(907, count(MimeType::getAll()));
    }

    public function testGetAllByType(): void {
        $this->assertEquals(661, count(MimeType::getAllByType(MimeType::APPLICATION)));
        $this->assertEquals(42, count(MimeType::getAllByType(MimeType::AUDIO)));
        $this->assertEquals(54, count(MimeType::getAllByType(MimeType::IMAGE)));
        $this->assertEquals(51, count(MimeType::getAllByType(MimeType::VIDEO)));
        $this->assertEquals(0, count(MimeType::getAllByType(MimeType::MULTIPART)));
        $this->assertEquals(77, count(MimeType::getAllByType(MimeType::TEXT)));
    }

    public function testGetExtByType(): void {
        $this->assertEquals(Vector {'jpe', 'jpeg', 'jpg'}, MimeType::getExtByType('image/jpeg'));
        $this->assertEquals(Vector {'m1v', 'm2v', 'mpe', 'mpeg', 'mpg'}, MimeType::getExtByType('video/mpeg'));
        $this->assertEquals(Vector {'htc', 'htm', 'html'}, MimeType::getExtByType('text/html'));
    }

    public function testGetTypeByExt(): void {
        $this->assertEquals('image/gif', MimeType::getTypeByExt('gif'));
        $this->assertEquals('text/html', MimeType::getTypeByExt('html'));
    }

    /**
     * @expectedException \Titon\Http\Exception\InvalidExtensionException
     */
    public function testGetTypeByExtInvalidExt(): void {
        MimeType::getTypeByExt('image/gif');
    }

}
