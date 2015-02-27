<?hh
namespace Titon\Http\Server;

use Titon\Http\Http;
use Titon\Test\TestCase;

class RedirectResponseTest extends TestCase {

    public function testSend(): void {
        $time = time();
        $response = new RedirectResponse('/new/url');
        $response->debug();
        $response->date($time);

        ob_start();
        $body = $response->send();
        ob_end_clean();

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Location' => ['/new/url'],
            'Status-Code' => ['302 Found']
        ], $response->getHeaders());

        $this->assertEquals($this->nl('<!DOCTYPE html>
            <html>
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
                <meta http-equiv="refresh" content="0; url=/new/url">
                <title>Redirecting to /new/url</title>
            </head>
            <body></body>
            </html>'), $body);
    }

    /**
     * @expectedException \Titon\Http\Exception\MalformedResponseException
     */
    public function testSendErrorsNoUrl(): void {
        $response = new RedirectResponse('');
        $response->send();
    }

}
