<?hh
namespace Titon\Http\Server;

use Titon\Test\TestCase;

class ResponseFactoryTest extends TestCase {


    public function testDownload(): void {
        $this->vfs()->createFile('/download.txt');

        $this->assertInstanceOf('Titon\Http\Server\DownloadResponse', Response::download($this->vfs()->path('/download.txt')));
    }

    public function testJson(): void {
        $this->assertInstanceOf('Titon\Http\Server\JsonResponse', Response::json(['foo' => 'bar']));
    }

    public function testRedirect(): void {
        $this->assertInstanceOf('Titon\Http\Server\RedirectResponse', Response::redirect('/'));
    }

    public function testXml(): void {
        $this->assertInstanceOf('Titon\Http\Server\XmlResponse', Response::xml(['foo' => 'bar']));
    }
}
