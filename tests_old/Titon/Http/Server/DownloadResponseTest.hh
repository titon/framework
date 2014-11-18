<?hh
namespace Titon\Http\Server;

use Titon\Http\Http;
use Titon\Test\TestCase;
use VirtualFileSystem\FileSystem;

class DownloadResponseTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->vfs = new FileSystem();
        $this->vfs->createDirectory('/http/');
        $this->vfs->createFile('/http/download.txt', 'This will be downloaded! Let\'s fluff this file with even more data to increase the file size.');
    }

    /**
     * @expectedException \Titon\Http\Exception\InvalidFileException
     */
    public function testMissingFileErrors() {
        new DownloadResponse($this->vfs->path('/http/download-missing.txt'));
    }

    /**
     * @expectedException \Titon\Http\Exception\InvalidFileException
     */
    public function testUnwritableFileErrors() {
        $this->vfs->createFile('/http/download-unwritable.txt', '')->chmod(0300);

        new DownloadResponse($this->vfs->path('/http/download-unwritable.txt'));
    }

    public function testSend() {
        $time = time();
        $response = new DownloadResponse($this->vfs->path('/http/download.txt'));
        $response->prepare(Request::createFromGlobals());

        ob_start();
        $body = $response->send();
        ob_end_clean();

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Connection' => ['keep-alive'],
            'Content-Type' => ['text/plain; charset=UTF-8'],
            'Status-Code' => ['200 OK'],
            'Content-Disposition' => ['attachment; filename="download.txt"'],
            'Accept-Ranges' => ['bytes'],
            'Content-Transfer-Encoding' => ['binary'],
            'Last-Modified' => [gmdate(Http::DATE_FORMAT, filemtime($this->vfs->path('/http/download.txt')))],
            'Content-Length' => [123],
        ], $response->getHeaders());

        $this->assertEquals('This will be downloaded! Let\'s fluff this file with even more data to increase the file size.', $body);
    }

    public function testSendNoType() {
        $this->vfs->createFile('/http/download', 'This will be downloaded! Let\'s fluff this file with even more data to increase the file size.');

        $time = time();
        $response = new DownloadResponse($this->vfs->path('/http/download'));
        $response->prepare(Request::createFromGlobals());

        ob_start();
        $body = $response->send();
        ob_end_clean();

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Connection' => ['keep-alive'],
            'Content-Type' => ['application/octet-stream'],
            'Status-Code' => ['200 OK'],
            'Content-Disposition' => ['attachment; filename="download"'],
            'Accept-Ranges' => ['bytes'],
            'Content-Transfer-Encoding' => ['binary'],
            'Last-Modified' => [gmdate(Http::DATE_FORMAT, filemtime($this->vfs->path('/http/download')))],
            'Content-Length' => [123],
        ], $response->getHeaders());

        $this->assertEquals('This will be downloaded! Let\'s fluff this file with even more data to increase the file size.', $body);
    }

    public function testSendConfig() {
        $time = time();
        $response = new DownloadResponse($this->vfs->path('/http/download.txt'), 200, Map {
            'autoEtag' => true,
            'autoModified' => true,
            'dispositionName' => 'custom-filename.txt'
        });
        $response->prepare(Request::registry());

        ob_start();
        $body = $response->send();
        ob_end_clean();

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Connection' => ['keep-alive'],
            'Content-Type' => ['text/plain; charset=UTF-8'],
            'Status-Code' => ['200 OK'],
            'Content-Disposition' => ['attachment; filename="custom-filename.txt"'],
            'Accept-Ranges' => ['bytes'],
            'Content-Transfer-Encoding' => ['binary'],
            'Last-Modified' => [gmdate(Http::DATE_FORMAT, filemtime($this->vfs->path('/http/download.txt')))],
            'ETag' => ['"3cefcf43cb525cb668db0cb67cccc41a8f90a727"'],
            'Content-Length' => [123],
        ], $response->getHeaders());

        $this->assertEquals('This will be downloaded! Let\'s fluff this file with even more data to increase the file size.', $body);
    }

    public function testFileRange() {
        $response = new DownloadResponse($this->vfs->path('/http/download.txt'));
        $response->prepare(new Request([], [], [], [], ['HTTP_RANGE' => 'bytes=0-5']));

        ob_start();
        $response->send();
        ob_end_clean();

        $this->assertEquals(206, $response->getStatusCode());
        $this->assertEquals(6, $response->getHeader('Content-Length'));
        $this->assertEquals('bytes 0-5/123', $response->getHeader('Content-Range'));
    }

    public function testInvalidFileRange() {
        $response = new DownloadResponse($this->vfs->path('/http/download.txt'));
        $response->prepare(new Request([], [], [], [], ['HTTP_RANGE' => 'bytes=5-3']));

        ob_start();
        $response->send();
        ob_end_clean();

        $this->assertEquals(416, $response->getStatusCode());
        $this->assertEquals(null, $response->getHeader('Content-Length'));
        $this->assertEquals(null, $response->getHeader('Content-Range'));
    }

    public function testSetFileRange() {
        $path = $this->vfs->path('/http/download.txt');

        $response = new DownloadResponse($path);

        // Valid starting range
        $response->prepare(new Request([], [], [], [], ['HTTP_RANGE' => 'bytes=0-19']));
        $response->setFileRange($path);

        $this->assertEquals(206, $response->getStatusCode());
        $this->assertEquals(20, $response->getHeader('Content-Length'));
        $this->assertEquals('bytes 0-19/123', $response->getHeader('Content-Range'));

        // No starting range
        $response->getRequest()->setHeader('Range', 'bytes=-35');
        $response->setFileRange($path);

        $this->assertEquals(206, $response->getStatusCode());
        $this->assertEquals(36, $response->getHeader('Content-Length'));
        $this->assertEquals('bytes 0-35/123', $response->getHeader('Content-Range'));

        // No ending range
        $response->getRequest()->setHeader('Range', 'bytes=45-');
        $response->setFileRange($path);

        $this->assertEquals(206, $response->getStatusCode());
        $this->assertEquals(78, $response->getHeader('Content-Length'));
        $this->assertEquals('bytes 45-122/123', $response->getHeader('Content-Range'));

        // Valid ending range
        $response->getRequest()->setHeader('Range', 'bytes=33-92');
        $response->setFileRange($path);

        $this->assertEquals(206, $response->getStatusCode());
        $this->assertEquals(60, $response->getHeader('Content-Length'));
        $this->assertEquals('bytes 33-92/123', $response->getHeader('Content-Range'));

        // No ranges at all
        $response->getRequest()->setHeader('Range', 'bytes=-');
        $response->setFileRange($path);

        $this->assertEquals(206, $response->getStatusCode());
        $this->assertEquals(123, $response->getHeader('Content-Length'));
        $this->assertEquals('bytes 0-122/123', $response->getHeader('Content-Range'));

        // Invalid ranges
        $response->getRequest()->setHeader('Range', 'bytes=100-0');
        $response->setFileRange($path);

        $this->assertEquals(416, $response->getStatusCode());

        $response->getRequest()->setHeader('Range', 'bytes=0-125');
        $response->setFileRange($path);

        $this->assertEquals(416, $response->getStatusCode());
    }

    public function testSetFileRangeIfRange() {
        $path = $this->vfs->path('/http/download.txt');

        $response = new DownloadResponse($path);
        $response->prepare(new Request([], [], [], [], ['HTTP_RANGE' => 'bytes=0-19', 'HTTP_IF_RANGE' => 'ETAG', 'HTTP_ETAG' => 'ETAG']));
        $response->setFileRange($path);

        $this->assertEquals(200, $response->getStatusCode());
        $this->assertEquals(null, $response->getHeader('Content-Length'));
        $this->assertEquals(null, $response->getHeader('Content-Range'));
    }

}