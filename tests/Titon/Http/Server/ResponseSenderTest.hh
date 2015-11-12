<?hh
namespace Titon\Http\Server;

use Titon\Test\TestCase;

class ResponseSenderTest extends TestCase {

    public function testSendBody(): void {
        $this->object->setBody(new MemoryStream('body'));

        ob_start();
        $this->object->sendBody();
        $body = ob_get_clean();

        $this->assertEquals('body', $body);
    }

    public function testSendBodyAndHeaders(): void {
        $this->object->body(new MemoryStream('<html><body>body</body></html>'));
        $this->assertEquals('<html><body>body</body></html>', $this->object->send());
    }

}
