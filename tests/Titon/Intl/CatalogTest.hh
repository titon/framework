<?hh
namespace Titon\Intl;

use Titon\Test\TestCase;

/**
 * @property \Titon\Intl\Catalog $object
 */
class CatalogTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Catalog('catalog', 'domain', Map {
            'foo' => 'This is a message'
        });
    }

    public function testGetDomain(): void {
        $this->assertEquals('domain', $this->object->getDomain());
    }

    public function testGetMessage(): void {
        $this->assertEquals('This is a message', $this->object->getMessage('foo'));
    }

    /**
     * @expectedException \Titon\Intl\Exception\MissingMessageException
     */
    public function testGetMessageThrowsMissingException(): void {
        $this->object->getMessage('missing');
    }

    public function testGetMessages(): void {
        $this->assertEquals(Map {
            'foo' => 'This is a message'
        }, $this->object->getMessages());
    }

    public function testGetName(): void {
        $this->assertEquals('catalog', $this->object->getName());
    }

    public function testParseKey(): void {
        $this->assertEquals(['domain' => 'domain', 'catalog' => 'catalog', 'id' => 'id'], Catalog::parseKey('domain.catalog.id'));
        $this->assertEquals(['domain' => 'domain', 'catalog' => 'catalog', 'id' => 'id.multi.part'], Catalog::parseKey('domain.catalog.id.multi.part'));
        $this->assertEquals(['domain' => 'domain', 'catalog' => 'catalog', 'id' => 'id-dashed'], Catalog::parseKey('domain.catalog.id-dashed'));
        $this->assertEquals(['domain' => 'domain', 'catalog' => 'catalog', 'id' => 'idspecial27304characters'], Catalog::parseKey('domain.catalog.id * special )*&2)*7304 characters'));
        $this->assertEquals(['domain' => 'Domain', 'catalog' => 'Catalog', 'id' => 'id.CamelCase'], Catalog::parseKey('Domain.Catalog.id.CamelCase'));
        $this->assertEquals(['domain' => 'd', 'catalog' => 'c', 'id' => 'i'], Catalog::parseKey('d.c.i'));
        $this->assertEquals(['domain' => 1, 'catalog' => 2, 'id' => 3], Catalog::parseKey('1.2.3'));

        $this->assertEquals(['domain' => 'common', 'catalog' => 'catalog', 'id' => 'id'], Catalog::parseKey('catalog.id'));
        $this->assertEquals(['domain' => 'common', 'catalog' => 'root', 'id' => 'id'], Catalog::parseKey('root.id'));
        $this->assertEquals(['domain' => 'common', 'catalog' => 'test', 'id' => 'key'], Catalog::parseKey('test.key'));
        $this->assertEquals(['domain' => 'common', 'catalog' => 1, 'id' => 2], Catalog::parseKey('1.2'));

        $this->assertEquals(['domain' => 'common', 'catalog' => 'default', 'id' => 'yes'], Catalog::parseKey('yes'));
        $this->assertEquals(['domain' => 'common', 'catalog' => 'default', 'id' => 'hello'], Catalog::parseKey('hello'));
    }

    /**
     * @expectedException \Titon\Intl\Exception\InvalidMessageKeyException
     */
    public function testParseKeyThrowsInvalidKey(): void {
        Catalog::parseKey('');
    }

}
