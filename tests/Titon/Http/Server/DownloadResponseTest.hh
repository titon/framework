<?hh
namespace Titon\Http\Server;

use Titon\Http\Http;
use Titon\Test\TestCase;
use Titon\Utility\State\Server;

class DownloadResponseTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createDirectory('/http/');
        $this->vfs()->createFile('/http/download.txt', 'This will be downloaded! Let\'s fluff this file with even more data to increase the file size.');
    }

    /**
     * @expectedException \Titon\Common\Exception\MissingFileException
     */
    public function testMissingFileErrors(): void {
        new DownloadResponse($this->vfs()->path('/http/download-missing.txt'));
    }

    /**
     * @expectedException \Titon\Http\Exception\InvalidFileException
     */
    public function testUnwritableFileErrors(): void {
        $this->vfs()->createFile('/http/download-unwritable.txt', '')->chmod(0300);

        new DownloadResponse($this->vfs()->path('/http/download-unwritable.txt'));
    }

    public function testSend(): void {
        $time = time();
        $response = new DownloadResponse($this->vfs()->path('/http/download.txt'));
        $response->prepare(Request::createFromGlobals());
        $response->date($time);

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
            'Content-Length' => [93],
        ], $response->getHeaders());

        $this->assertEquals('This will be downloaded! Let\'s fluff this file with even more data to increase the file size.', $body);
    }

    public function testSendNoType(): void {
        $this->vfs()->createFile('/http/download', 'This will be downloaded! Let\'s fluff this file with even more data to increase the file size.');

        $time = time();
        $response = new DownloadResponse($this->vfs()->path('/http/download'));
        $response->prepare(Request::createFromGlobals());
        $response->date($time);

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
            'Content-Length' => [93],
        ], $response->getHeaders());

        $this->assertEquals('This will be downloaded! Let\'s fluff this file with even more data to increase the file size.', $body);
    }

    public function testSendConfig(): void {
        $time = time();
        $response = Response::download($this->vfs()->path('/http/download.txt'), 'custom-filename.txt', true, true);
        $response->prepare(Request::createFromGlobals());
        $response->date($time);

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
            'Last-Modified' => [gmdate(Http::DATE_FORMAT, filemtime($this->vfs()->path('/http/download.txt')))],
            'ETag' => ['"3cefcf43cb525cb668db0cb67cccc41a8f90a727"'],
            'Content-Length' => [93],
        ], $response->getHeaders());

        $this->assertEquals('This will be downloaded! Let\'s fluff this file with even more data to increase the file size.', $body);
    }

    public function testFileRange(): void {
        $_SERVER['HTTP_RANGE'] = 'bytes=0-5';
        Server::initialize($_SERVER);

        $response = new DownloadResponse($this->vfs()->path('/http/download.txt'));
        $response->prepare(Request::createFromGlobals());

        ob_start();
        $response->send();
        ob_end_clean();

        $this->assertEquals(206, $response->getStatusCode());
        $this->assertEquals(6, $response->getHeader('Content-Length'));
        $this->assertEquals('bytes 0-5/93', $response->getHeader('Content-Range'));
    }

    public function testInvalidFileRange(): void {
        $_SERVER['HTTP_RANGE'] = 'bytes=5-3';
        Server::initialize($_SERVER);

        $response = new DownloadResponse($this->vfs()->path('/http/download.txt'));
        $response->prepare(Request::createFromGlobals());

        ob_start();
        $response->send();
        ob_end_clean();

        $this->assertEquals(416, $response->getStatusCode());
        $this->assertEquals(null, $response->getHeader('Content-Length'));
        $this->assertEquals(null, $response->getHeader('Content-Range'));
    }

    public function testSetFileRange(): void {
        $_SERVER['HTTP_RANGE'] = 'bytes=0-19';
        Server::initialize($_SERVER);

        $path = $this->vfs()->path('/http/download.txt');

        $response = new DownloadResponse($path);
        $response->prepare(Request::createFromGlobals());
        $request = $response->getRequest();

        invariant($request instanceof Request, 'Must be a Request.');

        // Valid starting range
        $response->setFileRange($path);

        $this->assertEquals(206, $response->getStatusCode());
        $this->assertEquals(20, $response->getHeader('Content-Length'));
        $this->assertEquals('bytes 0-19/93', $response->getHeader('Content-Range'));

        // No starting range
        $request->headers->set('Range', ['bytes=-35']);
        $response->setFileRange($path);

        $this->assertEquals(206, $response->getStatusCode());
        $this->assertEquals(36, $response->getHeader('Content-Length'));
        $this->assertEquals('bytes 0-35/93', $response->getHeader('Content-Range'));

        // No ending range
        $request->headers->set('Range', ['bytes=45-']);
        $response->setFileRange($path);

        $this->assertEquals(206, $response->getStatusCode());
        $this->assertEquals(48, $response->getHeader('Content-Length'));
        $this->assertEquals('bytes 45-92/93', $response->getHeader('Content-Range'));

        // Valid ending range
        $request->headers->set('Range', ['bytes=33-92']);
        $response->setFileRange($path);

        $this->assertEquals(206, $response->getStatusCode());
        $this->assertEquals(60, $response->getHeader('Content-Length'));
        $this->assertEquals('bytes 33-92/93', $response->getHeader('Content-Range'));

        // No ranges at all
        $request->headers->set('Range', ['bytes=-']);
        $response->setFileRange($path);

        $this->assertEquals(206, $response->getStatusCode());
        $this->assertEquals(93, $response->getHeader('Content-Length'));
        $this->assertEquals('bytes 0-92/93', $response->getHeader('Content-Range'));

        // Invalid ranges
        $request->headers->set('Range', ['bytes=100-0']);
        $response->setFileRange($path);

        $this->assertEquals(416, $response->getStatusCode());

        $request->headers->set('Range', ['bytes=0-125']);
        $response->setFileRange($path);

        $this->assertEquals(416, $response->getStatusCode());
    }

    public function testSetFileRangeIfRange(): void {
        $_SERVER['HTTP_RANGE'] = 'bytes=0-19';
        $_SERVER['HTTP_IF_RANGE'] = 'ETAG';
        $_SERVER['HTTP_ETAG'] = 'ETAG';
        Server::initialize($_SERVER);

        $path = $this->vfs()->path('/http/download.txt');

        $response = new DownloadResponse($path);
        $response->prepare(Request::createFromGlobals());
        $response->setFileRange($path);

        $this->assertEquals(200, $response->getStatusCode());
        $this->assertEquals(null, $response->getHeader('Content-Length'));
        $this->assertEquals(null, $response->getHeader('Content-Range'));
    }

}
