<?hh
namespace Titon\G11n;

use Titon\Test\TestCase;

/**
 * @property \Titon\G11n\Catalog $object
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
     * @expectedException \Titon\G11n\Exception\MissingMessageException
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
        $this->assertEquals(['domain' => 'domain', 'catalog' => 'catalog', 'key' => 'id'], Catalog::parseKey('domain.catalog.id'));
        $this->assertEquals(['domain' => 'domain', 'catalog' => 'catalog', 'key' => 'id.multi.part'], Catalog::parseKey('domain.catalog.id.multi.part'));
        $this->assertEquals(['domain' => 'domain', 'catalog' => 'catalog', 'key' => 'id-dashed'], Catalog::parseKey('domain.catalog.id-dashed'));
        $this->assertEquals(['domain' => 'domain', 'catalog' => 'catalog', 'key' => 'idspecial27304characters'], Catalog::parseKey('domain.catalog.id * special )*&2)*7304 characters'));
        $this->assertEquals(['domain' => 'Domain', 'catalog' => 'Catalog', 'key' => 'id.CamelCase'], Catalog::parseKey('Domain.Catalog.id.CamelCase'));
        $this->assertEquals(['domain' => 'd', 'catalog' => 'c', 'key' => 'i'], Catalog::parseKey('d.c.i'));
        $this->assertEquals(['domain' => 1, 'catalog' => 2, 'key' => 3], Catalog::parseKey('1.2.3'));

        $this->assertEquals(['domain' => 'common', 'catalog' => 'catalog', 'key' => 'id'], Catalog::parseKey('catalog.id'));
        $this->assertEquals(['domain' => 'common', 'catalog' => 'root', 'key' => 'id'], Catalog::parseKey('root.id'));
        $this->assertEquals(['domain' => 'common', 'catalog' => 'test', 'key' => 'key'], Catalog::parseKey('test.key'));
        $this->assertEquals(['domain' => 'common', 'catalog' => 1, 'key' => 2], Catalog::parseKey('1.2'));
    }

    /**
     * @expectedException \Titon\G11n\Exception\InvalidMessageKeyException
     */
    public function testParseKeyThrowsInvalidKey(): void {
        Catalog::parseKey('noModuleOrCatalog');
    }

}
