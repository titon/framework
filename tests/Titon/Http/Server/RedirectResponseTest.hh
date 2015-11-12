<?hh
namespace Titon\Http\Server;

use Titon\Test\TestCase;

class RedirectResponseTest extends TestCase {

    public function testConstructor(): void {
        $response = new RedirectResponse('/new/url');

        $this->assertEquals(302, $response->getStatusCode());
        $this->assertEquals('/new/url', $response->getHeaderLine('Location'));
        $this->assertEquals('text/html; charset=UTF-8', $response->getHeaderLine('Content-Type'));
        $this->assertEquals($this->nl('<!DOCTYPE html>
            <html>
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
                <meta http-equiv="refresh" content="0; url=/new/url">
                <title>Redirecting</title>
            </head>
            <body></body>
            </html>'), $response->getBody()->getContents());
    }

    /**
     * @expectedException \Titon\Http\Exception\MalformedResponseException
     */
    public function testConstructorInvalidURL(): void {
        new RedirectResponse('');
    }

}
