<?hh
namespace Titon\Http;

use Titon\Http\Stream\MemoryStream;
use Titon\Test\TestCase;

/**
 * @property \Titon\Http\Message $object
 */
class MessageTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new MessageStub();
    }

    public function testAddHeader() {
        $this->object->addHeader('Accept-Charset', 'utf-8');
        $this->object->addHeader('Accept-Charset', 'utf-16');

        $this->assertEquals(['utf-8', 'utf-16'], $this->object->getHeaderAsArray('Accept-Charset'));
    }

    public function testAddHeaders() {
        $this->object->addHeader('Accept-Charset', 'utf-8');
        $this->object->addHeader('Accept-Language', 'en');
        $this->object->addHeaders([
            'Accept-Charset' => 'utf-16',
            'Accept-Language' => ['fr', 'de']
        ]);

        $this->assertEquals(['utf-8', 'utf-16'], $this->object->getHeaderAsArray('Accept-Charset'));
        $this->assertEquals(['en', 'fr, de'], $this->object->getHeaderAsArray('Accept-Language'));
    }

    public function testHasHeader() {
        $this->assertFalse($this->object->hasHeader('Accept-Charset'));
        $this->object->addHeader('Accept-Charset', 'utf-8');
        $this->assertTrue($this->object->hasHeader('Accept-Charset'));
    }

    public function testGetSetBody() {
        $this->assertEquals(null, $this->object->getBody());

        $stream = new MemoryStream('A body can go here!');
        $this->object->setBody($stream);

        $this->assertEquals($stream, $this->object->getBody());
        $this->assertEquals('A body can go here!', (string) $this->object->getBody());
    }

    public function testGetHeader() {
        $this->assertEquals('', $this->object->getHeader('Accept-Charset'));
        $this->object->addHeader('Accept-Charset', 'utf-8');
        $this->object->addHeader('Accept-Charset', 'utf-16');

        $this->assertEquals('utf-8, utf-16', $this->object->getHeader('Accept-Charset'));
        $this->assertEquals(['utf-8', 'utf-16'], $this->object->getHeaderAsArray('Accept-Charset'));
    }

    public function testGetHeaders() {
        $this->object->addHeader('Accept-Charset', 'utf-8');
        $this->object->addHeader('Accept-Language', 'en');
        $this->object->addHeaders([
            'Accept-Charset' => 'utf-16',
            'Accept-Language' => 'fr'
        ]);
        $this->object->setHeader('Content-Type', 'text/html');

        $this->assertEquals([
            'Accept-Charset' => ['utf-8', 'utf-16'],
            'Accept-Language' => ['en', 'fr'],
            'Content-Type' => ['text/html']
        ], $this->object->getHeaders());
    }

    public function testSetHeader() {
        $this->object->setHeader('Content-Type', 'text/html');
        $this->assertEquals('text/html', $this->object->getHeader('Content-Type'));

        $this->object->setHeader('Content-Type', 'text/xml');
        $this->assertEquals('text/xml', $this->object->getHeader('Content-Type'));
    }

    public function testSetHeaders() {
        $this->object->setHeader('Content-Type', 'text/html');
        $this->object->setHeader('Content-Length', 100);

        $this->assertEquals([
            'Content-Type' => ['text/html'],
            'Content-Length' => [100]
        ], $this->object->getHeaders());

        $this->object->setHeaders([
            'Content-Type' => 'text/xml',
            'Content-Length' => 125
        ]);

        $this->assertEquals([
            'Content-Type' => ['text/xml'],
            'Content-Length' => [125]
        ], $this->object->getHeaders());
    }

    public function testRemoveHeader() {
        $this->object->setHeader('Content-Type', 'text/html');
        $this->assertTrue($this->object->hasHeader('Content-Type'));

        $this->object->removeHeader('Content-Type');
        $this->assertFalse($this->object->hasHeader('Content-Type'));
    }

    public function testRemoveHeaders() {
        $this->object->setHeader('Content-Type', 'text/html');
        $this->object->setHeader('Content-Length', 100);

        $this->assertEquals([
            'Content-Type' => ['text/html'],
            'Content-Length' => [100]
        ], $this->object->getHeaders());

        $this->object->removeHeaders(['Content-Type', 'Content-Length']);

        $this->assertEquals([], $this->object->getHeaders());
    }

}

class MessageStub extends Message {

    public function getProtocolVersion() {}

}