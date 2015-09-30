<?hh
namespace Titon\Http\Server;

use Titon\Http\Http;
use Titon\Test\TestCase;

class JsonResponseTest extends TestCase {

    public function testConstructorEncodes(): void {
        $response = new JsonResponse(['foo' => 'bar']);

        $this->assertEquals(200, $response->getStatusCode());
        $this->assertEquals('application/json; charset=UTF-8', $response->getHeaderLine('Content-Type'));
        $this->assertEquals('{"foo":"bar"}', $response->getBody()->getContents());
    }

    public function testConstructorWrapsJSONP(): void {
        $response = new JsonResponse(['foo' => 'bar'], 200, -1, 'Vendor.API.method');

        $this->assertEquals(200, $response->getStatusCode());
        $this->assertEquals('text/javascript; charset=UTF-8', $response->getHeaderLine('Content-Type'));
        $this->assertEquals('Vendor.API.method({"foo":"bar"});', $response->getBody()->getContents());
    }

    public function testConstructorUsesFlags(): void {
        $response = new JsonResponse(['Carets <>', 'Quotes ""', 'Ampersand &'], 400, JSON_HEX_QUOT);

        $this->assertEquals(400, $response->getStatusCode());
        $this->assertEquals('application/json; charset=UTF-8', $response->getHeaderLine('Content-Type'));
        $this->assertEquals('["Carets <>","Quotes \u0022\u0022","Ampersand &"]', $response->getBody()->getContents());
    }

}
