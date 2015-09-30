<?hh
namespace Titon\Http\Server;

use Titon\Test\TestCase;

class XmlResponseTest extends TestCase {

    public function testConstructor(): void {
        $response = new XmlResponse(['foo' => 'bar']);

        $this->assertEquals(200, $response->getStatusCode());
        $this->assertEquals('application/xml; charset=UTF-8', $response->getHeaderLine('Content-Type'));
        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root>' . PHP_EOL .
            '    <foo>bar</foo>' . PHP_EOL .
            '</root>' . PHP_EOL
        , $response->getBody()->getContents());
    }

    public function testConstructorDocumentRoot(): void {
        $response = new XmlResponse(['foo' => 'bar'], 404, 'data');

        $this->assertEquals(404, $response->getStatusCode());
        $this->assertEquals('application/xml; charset=UTF-8', $response->getHeaderLine('Content-Type'));
        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<data>' . PHP_EOL .
            '    <foo>bar</foo>' . PHP_EOL .
            '</data>' . PHP_EOL
        , $response->getBody()->getContents());
    }

}
