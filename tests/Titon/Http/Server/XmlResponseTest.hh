<?hh
namespace Titon\Http\Server;

use Titon\Http\Http;
use Titon\Test\TestCase;

class XmlResponseTest extends TestCase {

    public function testSend(): void {
        $time = time();
        $response = new XmlResponse(['foo' => 'bar']);
        $response->debug();
        $response->date($time);

        ob_start();
        $body = $response->send();
        ob_end_clean();

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Connection' => ['keep-alive'],
            'Content-Type' => ['application/xml; charset=UTF-8'],
            'Status-Code' => ['200 OK'],
            'Content-Length' => [73],
        ], $response->getHeaders());

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root>' . PHP_EOL .
            '    <foo>bar</foo>' . PHP_EOL .
            '</root>' . PHP_EOL
        , $body);
    }

    public function testRoot(): void {
        $time = time();
        $response = new XmlResponse(['foo' => 'bar'], 200, 'data');
        $response->debug();
        $response->date($time);

        ob_start();
        $body = $response->send();
        ob_end_clean();

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Connection' => ['keep-alive'],
            'Content-Type' => ['application/xml; charset=UTF-8'],
            'Status-Code' => ['200 OK'],
            'Content-Length' => [73],
        ], $response->getHeaders());

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<data>' . PHP_EOL .
            '    <foo>bar</foo>' . PHP_EOL .
            '</data>' . PHP_EOL
        , $body);
    }

}
