<?hh
namespace Titon\Http;

use Titon\Test\Stub\Http\ResponseStub;
use Titon\Test\TestCase;

/**
 * @property \Titon\Http\AbstractResponse $object
 */
class ResponseTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new ResponseStub();
    }

    public function testGetReasonPhrase(): void {
        $this->assertEquals('OK', $this->object->getReasonPhrase());
    }

    public function testGetReasonPhraseFromHeaders(): void {
        $this->object->getHeaderBag()->set('Reason-Phrase', ['Not OK']);

        $this->assertEquals('Not OK', $this->object->getReasonPhrase());
    }

    public function testGetStatusCode(): void {
        $this->assertEquals(200, $this->object->getStatusCode());
    }

    public function testWithStatus(): void {
        $object = $this->object->withStatus(404);

        $this->assertNotSame($object, $this->object);

        $this->assertEquals(200, $this->object->getStatusCode());
        $this->assertEquals(404, $object->getStatusCode());
    }

    public function testWithStatusCustomReasonPhrase(): void {
        $this->object->getHeaderBag()->set('Reason-Phrase', ['Missing']);

        $object = $this->object->withStatus(404, 'Oops');

        $this->assertNotSame($object, $this->object);

        $this->assertEquals('Missing', $this->object->getReasonPhrase());
        $this->assertEquals('Oops', $object->getReasonPhrase());
    }

}
