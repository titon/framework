<?hh
namespace Titon\Http\Server;

use Titon\Http\Http;
use Titon\Test\TestCase;

class JsonResponseTest extends TestCase {

    public function testSend() {
        $time = time();
        $response = new JsonResponse(['foo' => 'bar']);
        $response->debug();
        $response->date($time);

        ob_start();
        $body = $response->send();
        ob_end_clean();

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Connection' => ['keep-alive'],
            'Content-Type' => ['application/json; charset=UTF-8'],
            'Status-Code' => ['200 OK'],
            'Content-Length' => [13],
        ], $response->getHeaders());

        $this->assertEquals('{"foo":"bar"}', $body);
    }

    public function testSendCallback() {
        $time = time();
        $response = new JsonResponse(['foo' => 'bar']);
        $response->debug();
        $response->setCallback('Vendor.API.method');
        $response->date($time);

        ob_start();
        $body = $response->send();
        ob_end_clean();

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Connection' => ['keep-alive'],
            'Content-Type' => ['text/javascript; charset=UTF-8'],
            'Status-Code' => ['200 OK'],
            'Content-Length' => [33],
        ], $response->getHeaders());

        $this->assertEquals('Vendor.API.method({"foo":"bar"});', $body);
    }

    public function testFlags() {
        $time = time();
        $response = new JsonResponse(['Carets <>', 'Quotes ""', 'Ampersand &'], 200, JSON_HEX_QUOT);
        $response->debug();
        $response->date($time);

        ob_start();
        $body = $response->send();
        ob_end_clean();

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Connection' => ['keep-alive'],
            'Content-Type' => ['application/json; charset=UTF-8'],
            'Status-Code' => ['200 OK'],
            'Content-Length' => [49],
        ], $response->getHeaders());

        $this->assertEquals('["Carets <>","Quotes \u0022\u0022","Ampersand &"]', $body);
    }

}
