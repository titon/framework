<?hh
namespace Titon\Http;

use Titon\Test\TestCase;

class MimeTest extends TestCase {

    public function testGetAll(): void {
        $this->assertEquals(907, count(Mime::getAll()));
    }

    public function testGetAllByType(): void {
        $this->assertEquals(661, count(Mime::getAllByType(Mime::APPLICATION)));
        $this->assertEquals(42, count(Mime::getAllByType(Mime::AUDIO)));
        $this->assertEquals(54, count(Mime::getAllByType(Mime::IMAGE)));
        $this->assertEquals(51, count(Mime::getAllByType(Mime::VIDEO)));
        $this->assertEquals(0, count(Mime::getAllByType(Mime::MULTIPART)));
        $this->assertEquals(77, count(Mime::getAllByType(Mime::TEXT)));
    }

    public function testGetExtByType(): void {
        $this->assertEquals(Vector {'jpe', 'jpeg', 'jpg'}, Mime::getExtByType('image/jpeg'));
        $this->assertEquals(Vector {'m1v', 'm2v', 'mpe', 'mpeg', 'mpg'}, Mime::getExtByType('video/mpeg'));
        $this->assertEquals(Vector {'htc', 'htm', 'html'}, Mime::getExtByType('text/html'));
    }

    public function testGetTypeByExt(): void {
        $this->assertEquals('image/gif', Mime::getTypeByExt('gif'));
        $this->assertEquals('text/html', Mime::getTypeByExt('html'));
    }

    /**
     * @expectedException \Titon\Http\Exception\InvalidExtensionException
     */
    public function testGetTypeByExtInvalidExt(): void {
        Mime::getTypeByExt('image/gif', Mime::getTypeByExt('gf'));
    }

}
